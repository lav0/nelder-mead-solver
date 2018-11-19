#pragma once
#include "IVariableSet.h"
#include "ValueWrapperDouble.h"

namespace nmsolver
{
  template<typename TYPE>
  class VariableSetGeneric : public IVariableSet
  {
  public:

    typedef typename TYPE _type;

    VariableSetGeneric(size_t size) : vars_(size, TYPE(0)), limits_(size, limits()) {}
    ~VariableSetGeneric() {}

    size_t size() const override
    {
      return vars_.size();
    }

    double get_var(size_t index) const override
    {
      assert(index < size());

      return vars_[index].get();
    }

    std::unique_ptr<IVariableSet> subtract(const IVariableSet* other) const override
    {
      assert(size() == other->size());

      auto copy = deep_copy();
      auto ret = dynamic_cast<VariableSetGeneric<TYPE>*>(copy.get());

      for (size_t i = 0; i < size(); ++i)
      {
        double d = get_var(i) - other->get_var(i);
        ret->set_var(i, d);
      }

      return copy;
    }
    std::unique_ptr<IVariableSet> add(const IVariableSet* other) const override
    {
      assert(size() == other->size());

      auto copy = deep_copy();
      auto ret = dynamic_cast<VariableSetGeneric<TYPE>*>(copy.get());

      for (size_t i = 0; i < size(); ++i)
      {
        double d = get_var(i) + other->get_var(i);
        ret->set_var(i, d);
      }

      return copy;
    }
    std::unique_ptr<IVariableSet> multiply(double c) const override
    {
      auto copy = deep_copy();
      auto ret = dynamic_cast<VariableSetGeneric<TYPE>*>(copy.get());

      for (size_t i = 0; i < size(); ++i)
      {
        double d = get_var(i) * c;
        ret->set_var(i, d);
      }

      return copy;
    }

    void set_var(size_t index, double value)
    {
      if (value < limits_[index].min)
        value = limits_[index].min;
      else if (value > limits_[index].max)
        value = limits_[index].max;

      vars_[index].set(value);
    }
    void set_limits(size_t index, double min, double max)
    {
      limits_[index].min = min;
      limits_[index].max = max;
    }
    void get_limits(size_t index, double& min, double& max)
    {
      min = limits_[index].min;
      max = limits_[index].max;
    }

    void set_gut(size_t index, const TYPE& gut)
    {
      vars_[index] = gut;
    }

    TYPE& get_gut(size_t index)
    {
      return vars_[index];
    }

    std::unique_ptr<IVariableSet> deep_copy() const
    {
      auto r = std::make_unique<VariableSetGeneric<TYPE>>(vars_.size());
      r->vars_ = this->vars_;
      r->limits_ = this->limits_;
      return r;
    }
    
  private:

    typedef struct
    {
      double min = -DBL_MAX;
      double max = DBL_MAX;
    } limits;

    std::vector<TYPE> vars_;
    std::vector<limits> limits_;
  };

}
