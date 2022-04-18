#ifndef JACOBI_APPROXIMATION_HPP
#define JACOBI_APPROXIMATION_HPP

#include <math.h>
#include "mpi.h"

class JacobiApproximation
{
private:
  int maxn = 12;

public:
  JacobiApproximation();

  int Approximate();
};

#endif // JACOBI_APPROXIMATION_HPP