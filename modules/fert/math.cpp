// C++ Standard Library
#include <cmath>
// SIMD abstraction layer
#include "math.hpp"

void fert::cart2sph(double *r, double *s)
{
    s[0] = ymmnormd(fert::simd::simd_vec4d::load_unaligned(r));
    s[1] = acos(r[2] / s[0]);
    s[2] = atan2(r[1], r[2]);
};