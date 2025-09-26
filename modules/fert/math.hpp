/// Copyright (C) 2021 DART Lab - Politecnico di Milano.
/// All right reserved.

#pragma once

// C++ Standard Library
#include <cmath>
// SIMD abstraction layer
#include "SIMD.hpp"

namespace fert
{
    void cart2sph(double *r, double *s);
}