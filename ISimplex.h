#pragma once
#include "Point.h"

namespace nmsolver
{
  class ISimplex
  {
  public:

    virtual double get_deviation() const = 0;
    virtual IVariableSetUPtr reflection() = 0;
    virtual IVariableSetUPtr expansion() = 0;
    virtual IVariableSetUPtr contraction() = 0;
    virtual void shrink() = 0;
    virtual double value_in_point(const IVariableSetUPtr& a) const = 0;
    virtual const double best_value() const = 0;
    virtual const double second_worst_value() const = 0;
    virtual const double worst_value() const = 0;
    virtual void replace_maximum(const IVariableSetUPtr& a_new_point) = 0;
    virtual IVariableSetUPtr get_gravity_centre() const = 0;

    virtual ~ISimplex() {}

    static constexpr double alfa() { return 1.0; }
    static constexpr double gamma() { return 2.0; }
    static constexpr double rho() { return -0.5; }
    static constexpr double sigma() { return 0.5; }
  };

}