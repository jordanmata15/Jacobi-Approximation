#include "Matrix.hpp"

Matrix::Matrix(int m, int n) : rows(m), cols(n) {
    matrix = new double[m * n];
    this->reinitialize();
}

Matrix::~Matrix() { delete[] matrix; }

void Matrix::randomize(int limit) {
    srand(time(NULL));
    for (int i = 0; i < rows * cols; ++i) {
        matrix[i] = limit * ((double)rand() / (double)limit);
        if (limit == 0) matrix[i] = 0;
    }
}

void Matrix::reinitialize() { this->randomize(0); }

void Matrix::printMatrix() {
    for (int i = 0; i < this->getNumRows(); ++i) {
        for (int j = 0; j < this->getNumCols(); ++j) {
            std::cout << std::fixed << std::setprecision(5);
            if (this->getXY(i, j) >= 0)  // align negative values
                std::cout << " ";
            std::cout << this->getXY(i, j) << DELIMETER;
        }
        std::cout << NEWLINE;
    }
}

void Matrix::printMatrix(char charDelimeter) {
    for (int i = 0; i < this->getNumRows(); ++i) {
        for (int j = 0; j < this->getNumCols(); ++j) {
            std::cout << std::fixed << std::setprecision(5);
            if (this->getXY(i, j) >= 0)  // align negative values
                std::cout << " ";
            std::cout << this->getXY(i, j) << charDelimeter;
        }
        std::cout << NEWLINE;
    }
}

double Matrix::getXY(int x, int y) {
    return matrix[x * this->getNumRows() + y];
}

double Matrix::setXY(int x, int y, double value) {
    matrix[x * this->getNumRows() + y] = value;
}

double *Matrix::getMatrix() { return this->matrix; }

int Matrix::getNumRows() { return this->rows; }

int Matrix::getNumCols() { return this->cols; }
