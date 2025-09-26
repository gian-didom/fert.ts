#pragma once

#include <cstddef>
#include <tuple>
#include "SIMD.hpp"

namespace fert
{
    class Cspk;
    class CspkCheb;
}

class fert::Cspk
{
public:
    void getState(double et, double *r) const;
    void getState(double et, double *r, double *v) const;
    void getState(double et, double *r, double *v, double *a) const;

    virtual fert::simd::simd_vec4d mmgetStateR(double et) const = 0;
    virtual std::tuple<fert::simd::simd_vec4d, fert::simd::simd_vec4d> mmgetStateRV(double et) const = 0;
    virtual std::tuple<fert::simd::simd_vec4d, fert::simd::simd_vec4d, fert::simd::simd_vec4d> mmgetStateRVA(double et) const = 0;

    virtual ~Cspk() = default;

protected:
    alignas(32) double *ephData;
};

class fert::CspkCheb : public Cspk
{
public:
    CspkCheb(char *buffer, int dataBeg, int dataEnd, int nStates);

    virtual fert::simd::simd_vec4d mmgetStateR(double et) const;
    virtual std::tuple<fert::simd::simd_vec4d, fert::simd::simd_vec4d> mmgetStateRV(double et) const;
    virtual std::tuple<fert::simd::simd_vec4d, fert::simd::simd_vec4d, fert::simd::simd_vec4d> mmgetStateRVA(double et) const;

    virtual ~CspkCheb();

protected:
    double init;
    double intlen;
    int rsize;
    int n;
    int degree;
    int ephdim;
    int nStatesMod;

    virtual fert::simd::simd_vec4d interpolateR(const double tau, const double *data, const int degree, const double trad) const = 0;
    virtual std::tuple<fert::simd::simd_vec4d, fert::simd::simd_vec4d> interpolateRV(const double tau, const double *data, const int degree, const double trad) const = 0;
    virtual std::tuple<fert::simd::simd_vec4d, fert::simd::simd_vec4d, fert::simd::simd_vec4d> interpolateRVA(const double tau, const double *data, const int degree, const double trad) const = 0;
};
