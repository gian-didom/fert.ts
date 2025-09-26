#pragma once

#include <cstddef>
#include <tuple>
#include "spk.hpp"

namespace fert
{
    class Cspk3;
}

class fert::Cspk3 : public CspkCheb
{
public:
    Cspk3(char *buffer, int dataBeg, int dataEnd) : CspkCheb(buffer, dataBeg, dataEnd, 6){};

private:
    virtual fert::simd::simd_vec4d interpolateR(const double tau, const double *data, const int degree, const double trad) const;
    virtual std::tuple<fert::simd::simd_vec4d, fert::simd::simd_vec4d> interpolateRV(const double tau, const double *data, const int degree, const double trad) const;
    virtual std::tuple<fert::simd::simd_vec4d, fert::simd::simd_vec4d, fert::simd::simd_vec4d> interpolateRVA(const double tau, const double *data, const int degree, const double trad) const;
};