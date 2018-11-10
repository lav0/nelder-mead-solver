#pragma once

#include "SimplexTriple.h"
#include "SimplexGeneric.h"

namespace nmsolver
{
  class SimplexFactory
  {
  public:

    static std::unique_ptr<SimplexTriple> simplexTriple(
      std::function<double(const IVariableSet*)> obj_function,
      const Bounds& bounds,
      eArea a_area);

    static std::unique_ptr<SimplexGenericDouble> simplexGenericDouble(
      std::function<double(const IVariableSet*)> obj_function,
      size_t simplex_size,
      VariableSetGeneric<ValueWrapperDouble>& pattern_set
    );
  };
}