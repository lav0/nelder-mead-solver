#include "stdafx.h"
#include "SimplexGeneric.h"

#include <set>
using namespace nmsolver;

SimplexGenericDouble::SimplexGeneric(
  std::function<double(const IVariableSet*)> obj_function,
  size_t simplex_size
)
  : objective_(obj_function)
  , expected_size_(simplex_size)
{
  variables_.reserve(simplex_size);
}

double SimplexGenericDouble::get_deviation() const
{
  assert(simplex_size() == expected_size_);

  using VariableSetDouble = VariableSetGeneric<T>;

  auto grav_centre = get_gravity_centre();

  auto max_len = 0.0;
  for (auto& var : variables_)
  {
    auto len = length(var->subtract(grav_centre.get()).get());
    if (max_len < len)
      max_len = len;
  }

  return max_len;
}

IVariableSetUPtr SimplexGenericDouble::general_move(double coef) const
{
  auto* last = variables_.back().get();
  auto div = centroid_->subtract(last)->multiply(coef);;

  return centroid_->add(div.get());
}

IVariableSetUPtr SimplexGenericDouble::reflection()
{
  assert(simplex_size() == expected_size_);

  centroid_ = get_centroid();

  return general_move(alfa());
}

IVariableSetUPtr SimplexGenericDouble::expansion()
{
  assert(simplex_size() == expected_size_);

  centroid_ = get_centroid();

  return general_move(gamma());
}

IVariableSetUPtr SimplexGenericDouble::contraction()
{
  assert(simplex_size() == expected_size_);

  centroid_ = get_centroid();

  return general_move(rho());
}

void SimplexGenericDouble::shrink()
{
  assert(simplex_size() == expected_size_);

  sort_according_to_objective();

  auto* first = variables_.front().get();
  auto smx_size = simplex_size();

  // Xi = X1 + sigma * (Xi - X1)
  for (size_t i = 1; i < smx_size; ++i)
  {
    auto div = variables_[i]->subtract(first)->multiply(sigma());
    variables_[i] = first->add(div.get());
  }
}

double SimplexGenericDouble::value_in_point(const IVariableSetUPtr & a) const
{
  return objective_(a.get());
}

const double SimplexGenericDouble::best_value() const
{
  assert(simplex_size() == expected_size_);

  auto cmp = [this](const IVariableSetSPtr& a, const IVariableSetSPtr& b)
  {
    return objective_(a.get()) < objective_(b.get());
  };
  auto a = std::min_element(variables_.begin(), variables_.end(), cmp);
  return objective_(a->get());
}

const double SimplexGenericDouble::second_worst_value() const
{
  assert(simplex_size() == expected_size_);

  auto max = -DBL_MAX;
  auto sec = max;
  for (auto& var : variables_)
  {
    auto value = objective_(var.get());
    if (sec < value && sec <= max)
    {
      sec = value;
    }
    if (max < value)
    {
      sec = max;
      max = value;
    }
  }
  return sec;
}

const double SimplexGenericDouble::worst_value() const
{
  assert(simplex_size() == expected_size_);

  auto cmp = [this](const IVariableSetSPtr& a, const IVariableSetSPtr& b)
  {
    return objective_(a.get()) < objective_(b.get());
  };
  auto a = std::max_element(variables_.begin(), variables_.end(), cmp);
  return objective_(a->get());
}

void SimplexGenericDouble::replace_maximum(const IVariableSetUPtr & a_new_point)
{
  assert(simplex_size() == expected_size_);

  using VariableSetDouble = VariableSetGeneric<T>;

  sort_according_to_objective();

  auto bare_pointer = dynamic_cast<VariableSetDouble*>(a_new_point.get());
  if (nullptr == bare_pointer)
    return;
  
  variables_.back() = std::move(bare_pointer->deep_copy());
}

template<>
IVariableSetUPtr SimplexGenericDouble::get_gravity_centre() const
{
  assert(simplex_size() == expected_size_);

  using VariableSetDouble = VariableSetGeneric<T>;
  using Wrapper = VariableSetDouble::_type;

  auto size = variable_size();
  auto res = std::make_unique<VariableSetDouble>(size);
  
  for (size_t i = 0; i < size; ++i)
  {
    auto sum = Wrapper(0);
    for (auto& varset : variables_)
    {
      auto d = (sum.get() + varset->get_var(i));
      sum.set(d);
    }
    sum.set(sum.get() / simplex_size());
    res->set_var(i, sum.get());
  }
  return res;
}

template<>
IVariableSetUPtr SimplexGenericDouble::get_centroid()
{
  sort_according_to_objective();

  auto var_size = variable_size();
  auto res = std::make_unique<VariableSetGeneric<T>>(var_size);

  // looping through components of a simplex' variable
  for (size_t i = 0; i < var_size; ++i)
  {
    auto sum = VariableSetGeneric<T>::_type(0);
    auto smx_size = simplex_size();

    // looping through the vars themselves
    for (size_t j = 0; j < smx_size - 1; ++j)
    {
      auto& varset = variables_[j];
      auto d = (sum.get() + varset->get_var(i));
      sum.set(d);
    }
    sum.set(sum.get() / (smx_size - 1));
    res->set_var(i, sum.get());
  }
  return res;
}
template<>
void SimplexGenericDouble::sort_according_to_objective()
{
  auto cmp = [this](const IVariableSetSPtr& a, const IVariableSetSPtr& b)
  {
    return objective_(a.get()) < objective_(b.get());
  };

  std::sort(variables_.begin(), variables_.end(), cmp);
}
bool SimplexGenericDouble::addVariableSet(IVariableSetSPtr& varset)
{
  if (variables_.size() < expected_size_)
  {
    variables_.push_back(varset);
    return true;
  }

  return false;
}

template<>
size_t SimplexGenericDouble::simplex_size() const
{
  return variables_.size();
}

template<>
size_t SimplexGenericDouble::variable_size() const
{
  if (!variables_.empty())
    return variables_.front()->size();   

  return size_t(0);
}

IVariableSetSPtr SimplexGenericDouble::get_variable(size_t index) const
{
  assert(simplex_size() == expected_size_);
  assert(index < variables_.size());

  return variables_[index];
}

//void MakeTheTemplatesLink()
//{
//  SimplexGeneric<ValueWrapperDouble> o(0);
//  o.get_gravity_centre();
//}