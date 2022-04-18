#ifndef JACOBI_APPROXIMATION_HPP
#define JACOBI_APPROXIMATION_HPP

#include <math.h>

#include <fstream>
#include <iostream>

#include "Matrix.hpp"
#include "mpi.h"

class JacobiApproximation {
   private:
    int maxn = 12;
    Matrix *matrix;

    /**
     * @brief Takes in our 4 processes contribution and coordinates to combine
     * them into a single matrix.
     *
     * @param rank The rank of the process calling it.
     * @param matrix The matrix contribution of this process.
     */
    void consolidateMatrix(int rank, double *matrix);

    /**
     * @brief Prints out the matrix in an aligned format. Mixes spaces/tabs to
     * align so it's not ideal for parsing.
     *
     */
    void printMatrix();

    /**
     * @brief Writes the matrix out to a file in CSV format (after it's been
     * consolidated).
     *
     */
    void writeMatrix();

   public:
    /**
     * @brief Construct a new Jacobi Approximation object. Allocates the memory
     * for the resulting matrix.
     *
     */
    JacobiApproximation() { matrix = new Matrix(maxn, maxn); }

    /**
     * @brief Destroy the Jacobi Approximation object, deallocate the memory for
     * the resulting matrix.
     *
     */
    ~JacobiApproximation() { delete matrix; }

    /**
     * @brief Run the approximation algorithm. This calls consolidateMatrix and
     * writeMatrix.
     *
     * @return int 0 on success. Non zero on error.
     */
    int Approximate();
};

#endif  // JACOBI_APPROXIMATION_HPP