#include <iostream>
#include <cstddef>
#include <algorithm>
#include <iterator>
#include <tuple>
#include <math.h>
#include "spk.hpp"

void fert::Cspk::getState(double et, double *r) const
{
    fert::simd::simd_vec4d rmm = mmgetStateR(et);
    rmm.store_unaligned(r);
}

void fert::Cspk::getState(double et, double *r, double *v) const
{
    auto [rmm, vmm] = mmgetStateRV(et);
    rmm.store_unaligned(r);
    vmm.store_unaligned(v);
}

void fert::Cspk::getState(double et, double *r, double *v, double *a) const
{
    auto [rmm, vmm, amm] = mmgetStateRVA(et);
    rmm.store_unaligned(r);
    vmm.store_unaligned(v);
    amm.store_unaligned(a);
}

fert::CspkCheb::CspkCheb(char *buffer, int dataBeg, int dataEnd, int nStates)
{
    nStatesMod = 4 * nStates / 3;
    int offset = (dataEnd - 4) * 8;
    init = *(double *)&buffer[offset];
    intlen = *(double *)&buffer[offset + 8];
    rsize = (int)*(double *)&buffer[offset + 16];
    n = (int)*(double *)&buffer[offset + 24];
    degree = (rsize - 2) / nStates;

    int cycleOffset;
    int bufferOffset;

    ephdim = n * (rsize + (nStates / 3) * degree);
    ephData = new double[ephdim];
    for (int i = 0; i < n; i++)
    {
        bufferOffset = rsize * i * 8 + 16 + (dataBeg - 1) * 8;
        cycleOffset = (degree * (4 * nStates / 3) + 2) * i + 2;
        ephData[cycleOffset - 2] = *(double *)&buffer[bufferOffset - 16]; // Save tm
        ephData[cycleOffset - 1] = *(double *)&buffer[bufferOffset - 8];  // Save tr

        for (int j = 0; j < degree; j++)
        {
            for (int k = 0; k < nStates; k++)
            {
                ephData[cycleOffset + ((4 * nStates / 3) * j + (k + k / 3))] = *(double *)&buffer[bufferOffset + (j + k * degree) * 8];
            }
        }
    }
}

fert::CspkCheb::~CspkCheb()
{
    init = 0;
    intlen = 0;
    rsize = 0;
    n = 0;
    degree = 0;
    ephdim = 0;
    delete[] ephData;
}

fert::simd::simd_vec4d fert::CspkCheb::mmgetStateR(double et) const
{
    int internal_offset = floor((et - init) / intlen) * (nStatesMod * degree + 2) + 2;
    double tau = (et - *(ephData + internal_offset - 2)) / *(ephData + internal_offset - 1);

    return interpolateR(tau, (ephData + internal_offset), degree - 1, *(ephData + internal_offset - 1));
}

std::tuple<fert::simd::simd_vec4d, fert::simd::simd_vec4d> fert::CspkCheb::mmgetStateRV(double et) const
{
    int internal_offset = floor((et - init) / intlen) * (nStatesMod * degree + 2) + 2;
    double tau = (et - *(ephData + internal_offset - 2)) / *(ephData + internal_offset - 1);

    return interpolateRV(tau, (ephData + internal_offset), degree - 1, *(ephData + internal_offset - 1));
}

std::tuple<fert::simd::simd_vec4d, fert::simd::simd_vec4d, fert::simd::simd_vec4d> fert::CspkCheb::mmgetStateRVA(double et) const
{
    int internal_offset = floor((et - init) / intlen) * (nStatesMod * degree + 2) + 2;
    double tau = (et - *(ephData + internal_offset - 2)) / *(ephData + internal_offset - 1);

    return interpolateRVA(tau, (ephData + internal_offset), degree - 1, *(ephData + internal_offset - 1));
}