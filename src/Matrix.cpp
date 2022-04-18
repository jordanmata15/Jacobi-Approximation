#include "Matrix.hpp"

Matrix::Matrix(int m, int n) : rows(m), cols(n)
{
  matrix = new double[m * n];
  this->reinitialize();
}

Matrix::~Matrix()
{
  delete[] matrix;
}

void Matrix::randomize(int limit)
{
  srand(time(NULL));
  for (int i = 0; i < rows * cols; ++i)
  {
    matrix[i] = (double)i;
    // matrix[i] = limit * ((double)rand() / (double)limit);
  }
}

void Matrix::reinitialize()
{
  this->randomize(1);
}

void Matrix::printMatrix()
{
  for (int i = 0; i < this->getNumRows(); ++i)
  {
    for (int j = 0; j < this->getNumCols(); ++j)
    {
      std::cout << this->getXY(i, j) << DELIMETER;
    }
    std::cout << NEWLINE;
  }
}

double Matrix::getXY(int x, int y)
{
  return matrix[x * this->getNumRows() + y];
}

double *Matrix::getMatrix()
{
  return this->matrix;
}

int Matrix::getNumRows()
{
  return this->rows;
}

int Matrix::getNumCols()
{
  return this->cols;
}
