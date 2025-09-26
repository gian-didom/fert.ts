#pragma once

#include <cstddef>
#include <tuple>
#include "spk.hpp"

namespace fert
{
    class Cspk9;
}

class fert::Cspk9 : public Cspk
{
public:
    Cspk9(char *buffer, int dataBeg, int dataEnd);
    ~Cspk9();

    virtual fert::simd::simd_vec4d mmgetStateR(double et) const;
    virtual std::tuple<fert::simd::simd_vec4d, fert::simd::simd_vec4d> mmgetStateRV(double et) const;
    virtual std::tuple<fert::simd::simd_vec4d, fert::simd::simd_vec4d, fert::simd::simd_vec4d> mmgetStateRVA(double et) const;

private:
    int degree;
    int N;
    int ephdim;
    double *times;

    virtual fert::simd::simd_vec4d interpolateR(const double tau, const double *data, const double *times) const;
    virtual std::tuple<fert::simd::simd_vec4d, fert::simd::simd_vec4d> interpolateRV(const double tau, const double *data, const double *times) const;
    virtual std::tuple<fert::simd::simd_vec4d, fert::simd::simd_vec4d, fert::simd::simd_vec4d> interpolateRVA(const double tau, const double *data, const double *times) const;
};