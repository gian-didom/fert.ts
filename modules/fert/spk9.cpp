#include <iostream>
#include <cstddef>
#include <algorithm>
#include <iterator>
#include <tuple>
#include <math.h>
#include "SIMD.hpp"
#include "spk9.hpp"

fert::Cspk9::Cspk9(char *buffer, int dataBeg, int dataEnd)
{
    int offset = (dataEnd - 2) * 8;
    degree = *(double *)&buffer[offset];
    N = (int)*(double *)&buffer[offset + 8];

    int cycleOffset;
    int bufferOffset;

    ephdim = 8 * N;
    ephData = new double[ephdim]{0};
    times = new double[N];
    for (int i = 0; i < N; i++)
    {
        bufferOffset = 6 * i * 8 + (dataBeg - 1) * 8;
        cycleOffset = 8 * i;

        for (int j = 0; j < 6; j++)
        {

            ephData[cycleOffset + (j + j / 3)] = *(double *)&buffer[bufferOffset + j * 8];
        }
        times[i] = *(double *)&buffer[6 * N * 8 + i * 8 + (dataBeg - 1) * 8];
    }
}

fert::Cspk9::~Cspk9()
{
    delete[] ephData;
    delete[] times;
}

fert::simd::simd_vec4d fert::Cspk9::mmgetStateR(double et) const
{
    int it = (std::upper_bound(times, times + N, et) - times) - 1 - degree / 2;
    if (it < 0)
    {
        it = 0;
    }
    else if (it + degree + 1 > N)
    {
        it = N - (degree + 1);
    }
    int internal_offset = it * 8;
    return interpolateR(et, (ephData + internal_offset), (times + it));
}

fert::simd::simd_vec4d fert::Cspk9::interpolateR(const double et, const double *data, const double *times) const
{
    const double *locationr = std::next(data, 0);

    const fert::simd::simd_vec4d x = fert::simd::broadcast(et);

    fert::simd::simd_vec4d tempr[degree + 1];
    for (int i = 0; i <= degree; ++i, std::advance(locationr, 8))
    {
        tempr[i] = fert::simd::simd_vec4d::load_unaligned(locationr);
    }

    for (int j = 1; j <= degree; ++j)
    {
        for (int i = 0; i <= degree - j; ++i)
        {
            fert::simd::simd_vec4d invden = fert::simd::broadcast(1 / (times[i] - times[i + j]));
            fert::simd::simd_vec4d c1 = x - fert::simd::broadcast(times[i + j]);
            fert::simd::simd_vec4d c2 = fert::simd::broadcast(times[i]) - x;
            tempr[i] = (c1 * tempr[i] + c2 * tempr[i + 1]) * invden;
        }
    }

    return tempr[0];
}

std::tuple<fert::simd::simd_vec4d, fert::simd::simd_vec4d> fert::Cspk9::mmgetStateRV(double et) const
{
    int it = (std::upper_bound(times, times + N, et) - times) - 1 - degree / 2;
    if (it < 0)
    {
        it = 0;
    }
    else if (it + degree + 1 > N)
    {
        it = N - (degree + 1);
    }
    int internal_offset = it * 8;
    return interpolateRV(et, (ephData + internal_offset), (times + it));
}

std::tuple<fert::simd::simd_vec4d, fert::simd::simd_vec4d> fert::Cspk9::interpolateRV(const double et, const double *data, const double *times) const
{
    const double *locationr = std::next(data, 0);
    const double *locationv = std::next(data, 4);

    const fert::simd::simd_vec4d x = fert::simd::broadcast(et);

    fert::simd::simd_vec4d tempr[degree + 1];
    fert::simd::simd_vec4d tempv[degree + 1];
    for (int i = 0; i <= degree; ++i, std::advance(locationr, 8), std::advance(locationv, 8))
    {
        tempr[i] = fert::simd::simd_vec4d::load_unaligned(locationr);
        tempv[i] = fert::simd::simd_vec4d::load_unaligned(locationv);
    }

    for (int j = 1; j <= degree; ++j)
    {
        for (int i = 0; i <= degree - j; ++i)
        {
            fert::simd::simd_vec4d invden = fert::simd::broadcast(1 / (times[i] - times[i + j]));
            fert::simd::simd_vec4d c1 = x - fert::simd::broadcast(times[i + j]);
            fert::simd::simd_vec4d c2 = fert::simd::broadcast(times[i]) - x;

            tempr[i] = (c1 * tempr[i] + c2 * tempr[i + 1]) * invden;
            tempv[i] = (c1 * tempv[i] + c2 * tempv[i + 1]) * invden;
        }
    }
    fert::simd::simd_vec4d rmm = tempr[0];
    fert::simd::simd_vec4d vmm = tempv[0];
    return std::make_tuple(rmm, vmm);
}

std::tuple<fert::simd::simd_vec4d, fert::simd::simd_vec4d, fert::simd::simd_vec4d> fert::Cspk9::mmgetStateRVA(double et) const
{
    int it = (std::upper_bound(times, times + N, et) - times) - 1 - degree / 2;
    if (it < 0)
    {
        it = 0;
    }
    else if (it + degree + 1 > N)
    {
        it = N - (degree + 1);
    }
    int internal_offset = it * 8;
    return interpolateRVA(et, (ephData + internal_offset), (times + it));
}

std::tuple<fert::simd::simd_vec4d, fert::simd::simd_vec4d, fert::simd::simd_vec4d> fert::Cspk9::interpolateRVA(const double et, const double *data, const double *times) const
{
    const double *locationr = std::next(data, 0);
    const double *locationv = std::next(data, 4);

    const fert::simd::simd_vec4d x = fert::simd::broadcast(et);

    fert::simd::simd_vec4d tempr[degree + 1];
    fert::simd::simd_vec4d tempv[degree + 1];
    fert::simd::simd_vec4d tempa[degree + 1];
    for (int i = 0; i <= degree; ++i, std::advance(locationr, 8), std::advance(locationv, 8))
    {
        tempr[i] = fert::simd::simd_vec4d::load_unaligned(locationr);
        tempv[i] = fert::simd::simd_vec4d::load_unaligned(locationv);
        tempa[i] = fert::simd::zero();
    }

    for (int j = 1; j <= degree; ++j)
    {
        for (int i = 0; i <= degree - j; ++i)
        {
            fert::simd::simd_vec4d invden = fert::simd::broadcast(1 / (times[i] - times[i + j]));
            fert::simd::simd_vec4d c1 = x - fert::simd::broadcast(times[i + j]);
            fert::simd::simd_vec4d c2 = fert::simd::broadcast(times[i]) - x;
            tempa[i] = (c1.fmadd(tempa[i], tempv[i]) + c2 * tempa[i + 1] - tempv[i + 1]) * invden;
            tempr[i] = (c1 * tempr[i] + c2 * tempr[i + 1]) * invden;
            tempv[i] = (c1 * tempv[i] + c2 * tempv[i + 1]) * invden;
        }
    }

    fert::simd::simd_vec4d rmm = tempr[0];
    fert::simd::simd_vec4d vmm = tempv[0];
    fert::simd::simd_vec4d amm = tempa[0];
    return std::make_tuple(rmm, vmm, amm);
}