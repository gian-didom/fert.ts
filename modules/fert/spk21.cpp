#include <iostream>
#include <cstddef>
#include <algorithm>
#include <iterator>
#include <tuple>
#include <math.h>
#include "SIMD.hpp"
#include "spk21.hpp"

fert::Cspk21::Cspk21(char *buffer, int dataBeg, int dataEnd)
{
    int offset = (dataEnd - 2) * 8;
    degSize = (int)*(double *)&buffer[offset];
    N = (int)*(double *)&buffer[offset + 8];

    int bufferOffset;

    rsize = degSize * 4 + 11;
    times = new double[N];
    dt = new double[degSize * N];
    x0 = new double[8 * N]{0};
    coeffs = new double[4 * degSize * N]{0};
    kqmax = new int[N];

    for (int i = 0; i < N; ++i)
    {
        bufferOffset = rsize * i * 8 + (dataBeg - 1) * 8;

        times[i] = *(double *)&buffer[bufferOffset];
        for (int j = 0; j < degSize; ++j)
        {
            dt[degSize * i + j] = *(double *)&buffer[bufferOffset + (j + 1) * 8];
        }

        for (int j = 0; j < 6; ++j)
        {
            x0[8 * i + (j + 7 * (j % 2)) / 2] = *(double *)&buffer[bufferOffset + (j + 1 + degSize) * 8];
        }

        for (int j = 0; j < degSize; ++j)
        {
            for (int k = 0; k < 3; ++k)
            {
                coeffs[4 * degSize * i + j * 4 + k] = *(double *)&buffer[bufferOffset + (7 + degSize) * 8 + (k * degSize + j) * 8];
            }
        }

        kqmax[i] = (int)*(double *)&buffer[bufferOffset + (7 + 4 * degSize) * 8];
    }

    wini = new double[degSize + 1];
    for (int j = 0; j <= degSize; ++j)
    {
        wini[j] = 1 / ((double)(j + 1));
    }
}

fert::Cspk21::~Cspk21()
{
    delete[] times;
    delete[] dt;
    delete[] x0;
    delete[] coeffs;
    delete[] kqmax;
    delete[] wini;
}

fert::simd::simd_vec4d fert::Cspk21::mmgetStateR(double et) const
{
    int it = (std::upper_bound(times, times + N, et) - times);
    double delta = et - times[it];

    return interpolateR(delta, (dt + degSize * it), (x0 + 8 * it), (x0 + 8 * it + 4), (coeffs + 4 * degSize * it), kqmax[it]);
}

fert::simd::simd_vec4d fert::Cspk21::interpolateR(const double delta, const double *dt, const double *x0, const double *v0, const double *coeffs, const int kqmax) const
{
    double tp = delta;
    int m = kqmax - 2;

    double *fc = new double[m + 2]{0};
    double *wc = new double[m + 1]{0};
    double w[kqmax + 1];
    std::copy(wini, wini + kqmax, w);

    fert::simd::simd_vec4d deltat = fert::simd::broadcast(delta);
    fert::simd::simd_vec4d mmx0 = fert::simd::simd_vec4d::load_unaligned(x0);
    fert::simd::simd_vec4d mmv0 = fert::simd::simd_vec4d::load_unaligned(v0);
    const double *location = std::next(coeffs, 4 * kqmax - 8);

    fc[0] = 1;
    for (int j = 0; j < m; ++j)
    {
        double ruru = 1 / dt[j];
        fc[j + 1] = tp * ruru;
        wc[j] = delta * ruru;
        tp = delta + dt[j];
    }

    int jx = 0;
    for (int ks = kqmax - 1; ks >= 2; --ks)
    {
        jx++;
        for (int j = 0; j < jx; ++j)
        {
            w[j + ks] = fc[j + 1] * w[j + ks - 1] - wc[j] * w[j + ks];
        }
    }

    fert::simd::simd_vec4d Psum = fert::simd::zero();

    for (int j = kqmax - 2; j >= 0; --j, std::advance(location, -4))
    {
        Psum = fert::simd::simd_vec4d::load_unaligned(location).fmadd(fert::simd::broadcast(w[j + 1]), Psum);
    }

    delete[] fc;
    delete[] wc;

    return Psum.fmadd(deltat, mmv0).fmadd(deltat, mmx0);
}

std::tuple<fert::simd::simd_vec4d, fert::simd::simd_vec4d> fert::Cspk21::mmgetStateRV(double et) const
{
    int it = (std::upper_bound(times, times + N, et) - times);
    double delta = et - times[it];
    return interpolateRV(delta, (dt + degSize * it), (x0 + 8 * it), (x0 + 8 * it + 4), (coeffs + 4 * degSize * it), kqmax[it]);
}

std::tuple<fert::simd::simd_vec4d, fert::simd::simd_vec4d> fert::Cspk21::interpolateRV(const double delta, const double *dt, const double *x0, const double *v0, const double *coeffs, const int kqmax) const
{
    double tp = delta;
    int m = kqmax - 2;

    double *fc = new double[m + 2]{0};
    double *wc = new double[m + 1]{0};
    double w[kqmax + 1];
    std::copy(wini, wini + kqmax, w);

    fert::simd::simd_vec4d deltat = fert::simd::broadcast(delta);
    fert::simd::simd_vec4d mmx0 = fert::simd::simd_vec4d::load_unaligned(x0);
    fert::simd::simd_vec4d mmv0 = fert::simd::simd_vec4d::load_unaligned(v0);
    const double *location = std::next(coeffs, 4 * kqmax - 8);

    fc[0] = 1;
    for (int j = 0; j < m; ++j)
    {
        double ruru = 1 / dt[j];
        fc[j + 1] = tp * ruru;
        wc[j] = delta * ruru;
        tp = delta + dt[j];
    }

    int jx = 0;
    for (int ks = kqmax - 1; ks >= 2; --ks)
    {
        jx++;
        for (int j = 0; j < jx; ++j)
        {
            w[j + ks] = fc[j + 1] * w[j + ks - 1] - wc[j] * w[j + ks];
        }
    }

    double wv[degSize + 1];
    wv[0] = w[0];
    for (int j = 0; j < (jx + 1); ++j)
    {
        wv[j + 1] = fc[j + 1] * wv[j] - wc[j] * w[j + 1];
    }

    fert::simd::simd_vec4d Psum = fert::simd::zero();
    fert::simd::simd_vec4d Vsum = fert::simd::zero();

    for (int j = kqmax - 2; j >= 0; --j, std::advance(location, -4))
    {
        fert::simd::simd_vec4d coeff = fert::simd::simd_vec4d::load_unaligned(location);
        Psum = coeff.fmadd(fert::simd::broadcast(w[j + 1]), Psum);
        Vsum = coeff.fmadd(fert::simd::broadcast(wv[j]), Vsum);
    }

    fert::simd::simd_vec4d rmm = Psum.fmadd(deltat, mmv0).fmadd(deltat, mmx0);
    fert::simd::simd_vec4d vmm = Vsum.fmadd(deltat, mmv0);

    delete[] fc;
    delete[] wc;

    return std::make_tuple(rmm, vmm);
}

std::tuple<fert::simd::simd_vec4d, fert::simd::simd_vec4d, fert::simd::simd_vec4d> fert::Cspk21::mmgetStateRVA(double et) const
{
    int it = (std::upper_bound(times, times + N, et) - times);
    double delta = et - times[it];
    return interpolateRVA(delta, (dt + degSize * it), (x0 + 8 * it), (x0 + 8 * it + 4), (coeffs + 4 * degSize * it), kqmax[it]);
}

std::tuple<fert::simd::simd_vec4d, fert::simd::simd_vec4d, fert::simd::simd_vec4d> fert::Cspk21::interpolateRVA(const double delta, const double *dt, const double *x0, const double *v0, const double *coeffs, const int kqmax) const
{
    double tp = delta;
    int m = kqmax - 2;

    double *fc = new double[m + 2]{0};
    double *wc = new double[m + 1]{0};
    double w[kqmax + 1];
    std::copy(wini, wini + kqmax, w);

    fert::simd::simd_vec4d deltat = fert::simd::broadcast(delta);
    fert::simd::simd_vec4d mmx0 = fert::simd::simd_vec4d::load_unaligned(x0);
    fert::simd::simd_vec4d mmv0 = fert::simd::simd_vec4d::load_unaligned(v0);
    const double *location = std::next(coeffs, 4 * kqmax - 8);

    fc[0] = 1;
    for (int j = 0; j < m; ++j)
    {
        double ruru = 1 / dt[j];
        fc[j + 1] = tp * ruru;
        wc[j] = delta * ruru;
        tp = delta + dt[j];
    }

    int jx = 0;
    for (int ks = kqmax - 1; ks >= 2; --ks)
    {
        jx++;
        for (int j = 0; j < jx; ++j)
        {
            w[j + ks] = fc[j + 1] * w[j + ks - 1] - wc[j] * w[j + ks];
        }
    }

    double wv[degSize + 1];
    wv[0] = w[0];
    for (int j = 0; j < (jx + 1); ++j)
    {
        wv[j + 1] = fc[j + 1] * wv[j] - wc[j] * w[j + 1];
    }

    double wa[degSize + 1];
    wa[0] = 1;
    for (int j = 0; j < (jx + 1); ++j)
    {
        wa[j + 1] = fc[j + 1] * wa[j];
    }

    fert::simd::simd_vec4d Psum = fert::simd::zero();
    fert::simd::simd_vec4d Vsum = fert::simd::zero();
    fert::simd::simd_vec4d Asum = fert::simd::zero();

    for (int j = kqmax - 2; j >= 0; --j, std::advance(location, -4))
    {
        fert::simd::simd_vec4d coeff = fert::simd::simd_vec4d::load_unaligned(location);
        Psum = coeff.fmadd(fert::simd::broadcast(w[j + 1]), Psum);
        Vsum = coeff.fmadd(fert::simd::broadcast(wv[j]), Vsum);
        Asum = coeff.fmadd(fert::simd::broadcast(wa[j]), Asum);
    }
    fert::simd::simd_vec4d rmm = Psum.fmadd(deltat, mmv0).fmadd(deltat, mmx0);
    fert::simd::simd_vec4d vmm = Vsum.fmadd(deltat, mmv0);

    delete[] fc;
    delete[] wc;

    return std::make_tuple(rmm, vmm, Asum);
}