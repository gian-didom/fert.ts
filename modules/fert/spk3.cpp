#include <iostream>
#include <cstddef>
#include <algorithm>
#include <iterator>
#include <tuple>
#include <math.h>
#include "spk3.hpp"

fert::simd::simd_vec4d fert::Cspk3::interpolateR(const double tau, const double *data, const int degree, const double trad) const
{
    const double *locationr = std::next(data, 8 * degree);
    const fert::simd::simd_vec4d ymm_x = fert::simd::broadcast(tau);
    const fert::simd::simd_vec4d ymm_twos = fert::simd::broadcast(2);
    const fert::simd::simd_vec4d ymm_twox = fert::simd::broadcast(2 * tau);

    fert::simd::simd_vec4d ymm_BKp2 = fert::simd::simd_vec4d::load_unaligned(locationr);
    std::advance(locationr, -8);

    fert::simd::simd_vec4d ymm_BKp1 = ymm_twox.fmadd(ymm_BKp2, fert::simd::simd_vec4d::load_unaligned(locationr));
    std::advance(locationr, -8);

    for (int n = degree - 2; n > 0; --n, std::advance(locationr, -8))
    {
        fert::simd::simd_vec4d ymm_BK = ymm_twox.fmadd(ymm_BKp1, fert::simd::simd_vec4d::load_unaligned(locationr) - ymm_BKp2);
        ymm_BKp2 = std::move(ymm_BKp1);
        ymm_BKp1 = std::move(ymm_BK);
    }

    return ymm_x.fmadd(ymm_BKp1, fert::simd::simd_vec4d::load_unaligned(locationr) - ymm_BKp2);
}

std::tuple<fert::simd::simd_vec4d, fert::simd::simd_vec4d> fert::Cspk3::interpolateRV(const double tau, const double *data, const int degree, const double trad) const
{
    const double *locationr = std::next(data, 8 * degree);
    const double *locationv = std::next(data, 8 * degree + 4);
    const fert::simd::simd_vec4d ymm_x = fert::simd::broadcast(tau);
    const fert::simd::simd_vec4d ymm_twos = fert::simd::broadcast(2);
    const fert::simd::simd_vec4d ymm_twox = fert::simd::broadcast(2 * tau);

    fert::simd::simd_vec4d ymm_BKp2 = fert::simd::simd_vec4d::load_unaligned(locationr);
    std::advance(locationr, -8);

    fert::simd::simd_vec4d ymm_BKp1 = ymm_twox.fmadd(ymm_BKp2, fert::simd::simd_vec4d::load_unaligned(locationr));
    std::advance(locationr, -8);

    fert::simd::simd_vec4d ymm_DKp2 = fert::simd::simd_vec4d::load_unaligned(locationv);
    std::advance(locationv, -8);

    fert::simd::simd_vec4d ymm_DKp1 = ymm_twox.fmadd(ymm_DKp2, fert::simd::simd_vec4d::load_unaligned(locationv));
    std::advance(locationv, -8);

    for (int n = degree - 2; n > 0; --n, std::advance(locationr, -8), std::advance(locationv, -8))
    {
        fert::simd::simd_vec4d ymm_BK = ymm_twox.fmadd(ymm_BKp1, fert::simd::simd_vec4d::load_unaligned(locationr) - ymm_BKp2);
        fert::simd::simd_vec4d ymm_DK = ymm_twox.fmadd(ymm_DKp1, fert::simd::simd_vec4d::load_unaligned(locationv) - ymm_DKp2);
        ymm_BKp2 = std::move(ymm_BKp1);
        ymm_BKp1 = std::move(ymm_BK);
        ymm_DKp2 = std::move(ymm_DKp1);
        ymm_DKp1 = std::move(ymm_DK);
    }

    fert::simd::simd_vec4d rmm = ymm_x.fmadd(ymm_BKp1, fert::simd::simd_vec4d::load_unaligned(locationr) - ymm_BKp2);
    fert::simd::simd_vec4d vmm = ymm_x.fmadd(ymm_DKp1, fert::simd::simd_vec4d::load_unaligned(locationv) - ymm_DKp2);

    return std::make_tuple(rmm, vmm);
}

std::tuple<fert::simd::simd_vec4d, fert::simd::simd_vec4d, fert::simd::simd_vec4d> fert::Cspk3::interpolateRVA(const double tau, const double *data, const int degree, const double trad) const
{
    const double *locationr = std::next(data, 8 * degree);
    const double *locationv = std::next(data, 8 * degree + 4);
    const fert::simd::simd_vec4d ymm_x = fert::simd::broadcast(tau);
    const fert::simd::simd_vec4d ymm_twos = fert::simd::broadcast(2);
    const fert::simd::simd_vec4d ymm_twox = fert::simd::broadcast(2 * tau);
    const fert::simd::simd_vec4d ymm_trad = fert::simd::broadcast(1 / trad);

    fert::simd::simd_vec4d ymm_BKp2 = fert::simd::simd_vec4d::load_unaligned(locationr);
    std::advance(locationr, -8);

    fert::simd::simd_vec4d ymm_BKp1 = ymm_twox.fmadd(ymm_BKp2, fert::simd::simd_vec4d::load_unaligned(locationr));
    std::advance(locationr, -8);

    fert::simd::simd_vec4d ymm_DKp2 = fert::simd::simd_vec4d::load_unaligned(locationv);
    std::advance(locationv, -8);

    fert::simd::simd_vec4d ymm_DKp1 = ymm_twox.fmadd(ymm_DKp2, fert::simd::simd_vec4d::load_unaligned(locationv));
    std::advance(locationv, -8);

    fert::simd::simd_vec4d ymm_SKp2 = fert::simd::zero();
    fert::simd::simd_vec4d ymm_SKp1 = ymm_twos * ymm_DKp1;

    for (int n = degree - 2; n > 0; --n, std::advance(locationr, -8), std::advance(locationv, -8))
    {
        fert::simd::simd_vec4d ymm_BK = ymm_twox.fmadd(ymm_BKp1, fert::simd::simd_vec4d::load_unaligned(locationr) - ymm_BKp2);
        fert::simd::simd_vec4d ymm_DK = ymm_twox.fmadd(ymm_DKp1, fert::simd::simd_vec4d::load_unaligned(locationv) - ymm_DKp2);
        fert::simd::simd_vec4d ymm_SK = ymm_twox.fmadd(ymm_SKp1, (ymm_twos * ymm_DKp1) - ymm_SKp2);
        ymm_BKp2 = std::move(ymm_BKp1);
        ymm_BKp1 = std::move(ymm_BK);
        ymm_DKp2 = std::move(ymm_DKp1);
        ymm_DKp1 = std::move(ymm_DK);
        ymm_SKp2 = std::move(ymm_SKp1);
        ymm_SKp1 = std::move(ymm_SK);
    }

    fert::simd::simd_vec4d rmm = ymm_x.fmadd(ymm_BKp1, fert::simd::simd_vec4d::load_unaligned(locationr) - ymm_BKp2);
    fert::simd::simd_vec4d vmm = ymm_x.fmadd(ymm_DKp1, fert::simd::simd_vec4d::load_unaligned(locationv) - ymm_DKp2);
    fert::simd::simd_vec4d amm = (ymm_x.fmadd(ymm_SKp1, ymm_DKp1 - ymm_SKp2)) * ymm_trad;

    return std::make_tuple(rmm, vmm, amm);
}