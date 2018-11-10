
#pragma once

#include <thread>
#include <queue>

#include "stdafx.h"
#include "SimplexFactory.h"

namespace nmsolver
{
  class Solver
  {
  public:

    Solver();

    void addSimplex(ISimplex* simplex);

    bool solve(
      bool            a_multithreading,
      double          a_tolerance,
      IVariableSetUPtr& a_output,
      double&         a_value
    );

  private:

    bool internal_solve_cycle(double a_tolerance);

  private:

    std::queue<ISimplex*>     m_simplex_queue;
    bool                      m_found;
    IVariableSetUPtr            m_found_solution;
    double                    m_found_value;

    std::mutex m_mutex;

  };
}