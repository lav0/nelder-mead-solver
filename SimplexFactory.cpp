#include "stdafx.h"
#include "SimplexFactory.h"

#include <array>

using namespace nmsolver;

std::unique_ptr<SimplexTriple> SimplexFactory::simplexTriple(
  std::function<double(const IVariableSet*)> obj_function,
  const Bounds& bounds,
  eArea a_area)
{
  Point p1(0, 0), p2(0, 0), p3(0, 0);
  bounds.build_triangle_on_area(a_area, p1, p2, p3);
  auto simplex = std::make_unique<SimplexTriple>(
    obj_function,
    p1, p2, p3
  );
  simplex->setBounds(bounds);
  return simplex;
}

std::unique_ptr<SimplexGenericDouble> SimplexFactory::simplexGenericDouble(
  std::function<double(const IVariableSet*)> obj_function,
  size_t simplex_size,
  VariableSetGeneric<ValueWrapperDouble>& pattern_set
)
{
  using VariableSetDouble = VariableSetGeneric<ValueWrapperDouble>;

  auto simplex = std::make_unique<SimplexGenericDouble>(obj_function, simplex_size);
  auto var_size = simplex_size - 1;

  std::vector<std::array<double, 2>> points;
  points.reserve(var_size);
  for (size_t i = 0; i < var_size; ++i)
  {
    double x0, x1;
    double min, max, value;
    pattern_set.get_limits(i, min, max);
    value = pattern_set.get_var(i);
    if (max != DBL_MAX)
    {
      x0 = 0.5 * (min + value);
      x1 = 0.5 * (max + value);
    }
    else
    {
      x0 = value - 1000.0;
      x1 = value + 1000.0;
    }
    points.push_back({ x0, x1 });
  }

  for (size_t i = 0; i < simplex_size; ++i)
  {
    auto var = std::make_shared<VariableSetDouble>(var_size);

    for (size_t j = 0; j < var_size; ++j)
    {
      auto ind1 = (i > (var_size - 1 -j)) ? 1 : 0;
      auto value = points[j][ind1];
      
      var->set_var(j, value);
    }

    simplex->addVariableSet((IVariableSetSPtr)var);
  }

  return simplex;
}
