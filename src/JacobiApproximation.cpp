#include "JacobiApproximation.hpp"

/* Place all our data from different processes into the same matrix */
void JacobiApproximation::consolidateMatrix(int rank, double* matrix) {
    // send over quadrants of size (maxn/2 X maxn/2) instead of individual rows
    MPI_Datatype quarter_array_type;
    MPI_Type_vector(maxn / 2, maxn / 2, maxn, MPI_DOUBLE, &quarter_array_type);
    MPI_Type_commit(&quarter_array_type);

    MPI_Status status;
    double tempMatrix[maxn * maxn] = {0};

    int begin_x = 0;
    int begin_y = 0;

    if (rank % 2 == 1)  // right 2 quadrants have an empty column on the left
        begin_x = 1;
    if (rank >= 2)  // bottom 2 quadrants have an empty row on the top
        begin_y = 1;

    if (rank != 0)  // quadrants 1-3 will give their portion to rank 0
        MPI_Send(&matrix[(begin_y * maxn) + begin_x], 1, quarter_array_type, 0,
                 0, MPI_COMM_WORLD);

    else if (rank == 0)  // rank 0 will consilidate everything
    {
        MPI_Recv(&tempMatrix[maxn / 2], 1, quarter_array_type, 1, 0,
                 MPI_COMM_WORLD, &status);
        MPI_Recv(&tempMatrix[((maxn / 2) * maxn)], 1, quarter_array_type, 2, 0,
                 MPI_COMM_WORLD, &status);
        MPI_Recv(&tempMatrix[(maxn / 2) * maxn + (maxn / 2)], 1,
                 quarter_array_type, 3, 0, MPI_COMM_WORLD, &status);

        // copy first quadrant to temp manually
        for (int i = 0; i < maxn / 2; ++i)
            for (int j = 0; j < maxn / 2; ++j)
                tempMatrix[i * maxn + j] = matrix[i * maxn + j];

        // write it to our matrix field/variable in or object
        for (int i = 0; i < maxn; ++i)
            for (int j = 0; j < maxn; ++j)
                this->matrix->setXY(i, j, tempMatrix[i * maxn + j]);
    }
}

/* print out the matrix in aligned format. Not ideal for parsing (mixes
 * tabs/spaces) */
void JacobiApproximation::printMatrix() {
    std::cout << "\n";
    this->matrix->printMatrix('\t');
    std::cout << std::endl;
}

/* Write out our matrix in csv format */
void JacobiApproximation::writeMatrix() {
    char* filename = "../data/output.csv";
    std::ofstream outputFile;
    outputFile.open(filename, std::ios::out | std::ios::trunc);

    for (int i = 0; i < maxn; ++i) {
        for (int j = 0; j < maxn; ++j) {
            outputFile << this->matrix->getXY(i, j);
            if (j < maxn - 1) outputFile << ",";
        }
        outputFile << std::endl;
    }
    outputFile.close();
}

/* Run the approximation algorithm among 4 processes.
 * Will consolidate to a single matrix and print to file */
int JacobiApproximation::Approximate() {
    int rank, size, i, j, itcnt;
    int first_x, last_x, first_y, last_y;
    double diffnorm, gdiffnorm;
    double xlocal[12 * 12] = {0};
    double xnew[12 * 12] = {0};
    MPI_Status status;

    MPI_Init(nullptr, nullptr);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (size != 4) MPI_Abort(MPI_COMM_WORLD, 1);

    // for sending a single row of length (maxn/2)
    MPI_Datatype half_row_type;
    MPI_Type_vector(1, maxn / 2, 1, MPI_DOUBLE, &half_row_type);
    MPI_Type_commit(&half_row_type);

    // for sending a single col of length (maxn/2)
    MPI_Datatype half_column_type;
    MPI_Type_vector(maxn / 2, 1, maxn, MPI_DOUBLE, &half_column_type);
    MPI_Type_commit(&half_column_type);

    first_x = 1;
    last_x = maxn / 2 - 1;
    first_y = 1;
    last_y = maxn / 2 - 1;

    // Fill the rank
    for (i = 0; i <= last_y; ++i)
        for (j = 0; j <= last_x + 1; ++j) xlocal[i * maxn + j] = rank;

    // Fill in the border -1 values
    for (j = 0; j < maxn / 2 + 1; j++) {
        xlocal[(first_y - 1) * maxn + j] = -1;
        xlocal[(last_y + 1) * maxn + j] = -1;
        // fill the spots where we will receive a row or column
        // not needed, but helpful for printing/debugging
        if (rank % 2 == 1)
            xlocal[j * maxn + (first_x - 1)] = -1;
        else
            xlocal[j * maxn + (last_x + 1)] = -1;
    }

    itcnt = 0;
    do {
        // Exchange vertically
        if (rank >= 2)  // bottom 2 blocks send their first row up
            MPI_Send(&xlocal[(first_y * maxn) + first_x], 1, half_row_type,
                     rank - 2, 0, MPI_COMM_WORLD);
        if (rank <
            2)  // top 2 blocks receive a row from bottom at the last_y+1 row
            MPI_Recv(&xlocal[(last_y + 1) * maxn + first_x], 1, half_row_type,
                     rank + 2, 0, MPI_COMM_WORLD, &status);
        if (rank < 2)  // top 2 blocks send their last row down
            MPI_Send(&xlocal[(last_y * maxn) + first_x], 1, half_row_type,
                     rank + 2, 0, MPI_COMM_WORLD);
        if (rank >=
            2)  // bottom 2 blocks receive a row from top at the first_y-1 row
            MPI_Recv(&xlocal[(first_y - 1) * maxn + first_x], 1, half_row_type,
                     rank - 2, 0, MPI_COMM_WORLD, &status);

        // Exchange horizontally
        if (rank % 2 ==
            0)  // left 2 blocks send their last column to the right (ranks 1,3)
            MPI_Send(&xlocal[(first_y * maxn) + last_x], 1, half_column_type,
                     rank + 1, 0, MPI_COMM_WORLD);
        if (rank % 2 ==
            1)  // right 2 blocks receive a column from left (ranks 0,2)
            MPI_Recv(&xlocal[(first_y * maxn)], 1, half_column_type, rank - 1,
                     0, MPI_COMM_WORLD, &status);
        if (rank % 2 == 1)  // right 2 blocks send their first column to the
                            // left (ranks 1,3)
            MPI_Send(&xlocal[(first_y * maxn) + first_x], 1, half_column_type,
                     rank - 1, 0, MPI_COMM_WORLD);
        if (rank % 2 ==
            0)  // left 2 blocks receive a column from right (ranks 0,2)
            MPI_Recv(&xlocal[(first_y * maxn) + last_x + 1], 1,
                     half_column_type, rank + 1, 0, MPI_COMM_WORLD, &status);

        itcnt++;
        diffnorm = 0.0;
        for (i = first_y; i <= last_y; i++)
            for (j = first_x; j <= last_x; j++) {
                xnew[(i * maxn) + j] =
                    (xlocal[(i * maxn) + (j + 1)] +
                     xlocal[(i * maxn) + (j - 1)] + xlocal[(i + 1) * maxn + j] +
                     xlocal[(i - 1) * maxn + j]) /
                    4.0;
                diffnorm += (xnew[(i * maxn) + j] - xlocal[(i * maxn) + j]) *
                            (xnew[(i * maxn) + j] - xlocal[(i * maxn) + j]);
            }

        // Only transfer the interior points
        for (i = first_y; i <= last_y; i++)
            for (j = first_x; j <= last_x; j++)
                xlocal[(i * maxn) + j] = xnew[(i * maxn) + j];

        MPI_Allreduce(&diffnorm, &gdiffnorm, 1, MPI_DOUBLE, MPI_SUM,
                      MPI_COMM_WORLD);
        gdiffnorm = sqrt(gdiffnorm);

        if (rank == 0)
            printf("At iteration %d, diff is %e\n", itcnt, gdiffnorm);

    } while (gdiffnorm > 1.0e-2 && itcnt < 100);

    consolidateMatrix(rank, xlocal);
    if (rank == 0) {
        writeMatrix();
        // printMatrix(); // if we want to see the resulting matrix
    }

    MPI_Finalize();

    return 0;
}