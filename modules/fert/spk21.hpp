#pragma once

#include <cstddef>
#include <tuple>
#include "spk.hpp"

namespace fert
{
    class Cspk21;
}

class fert::Cspk21 : public Cspk
{
public:
    Cspk21(char *buffer, int dataBeg, int dataEnd);
    ~Cspk21();

    virtual fert::simd::simd_vec4d mmgetStateR(double et) const;
    virtual std::tuple<fert::simd::simd_vec4d, fert::simd::simd_vec4d> mmgetStateRV(double et) const;
    virtual std::tuple<fert::simd::simd_vec4d, fert::simd::simd_vec4d, fert::simd::simd_vec4d> mmgetStateRVA(double et) const;

private:
    int MAXTRM = 50;
    int degSize;
    int rsize;
    int N;
    double *wini;
    double *times;
    double *dt;
    alignas(32) double *x0;
    alignas(32) double *coeffs;
    int *kqmax;

    virtual fert::simd::simd_vec4d interpolateR(const double delta, const double *dt, const double *x0, const double *v0, const double *coeffs, const int kqmax) const;
    virtual std::tuple<fert::simd::simd_vec4d, fert::simd::simd_vec4d> interpolateRV(const double delta, const double *dt, const double *x0, const double *v0, const double *coeffs, const int kqmax) const;
    virtual std::tuple<fert::simd::simd_vec4d, fert::simd::simd_vec4d, fert::simd::simd_vec4d> interpolateRVA(const double delta, const double *dt, const double *x0, const double *v0, const double *coeffs, const int kqmax) const;
};