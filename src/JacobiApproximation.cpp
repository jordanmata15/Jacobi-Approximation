#include "JacobiApproximation.hpp"

JacobiApproximation::JacobiApproximation()
{
}

int JacobiApproximation::Approximate()
{
    int rank, size, i, j, itcnt;
    int first_x, last_x, first_y, last_y;
    MPI_Status status;
    double diffnorm, gdiffnorm;
    double xlocal[((12 / 4) + 2) * 12] = {0};
    double xnew[((12 / 3) + 2) * 12] = {0};

    MPI_Init(nullptr, nullptr);

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (size != 4)
        MPI_Abort(MPI_COMM_WORLD, 1);

    MPI_Datatype half_row_type;
    MPI_Type_vector(1, maxn / 2, 1, MPI_DOUBLE, &half_row_type);
    MPI_Type_commit(&half_row_type);

    MPI_Datatype half_column_type;
    MPI_Type_vector(maxn / 2, 1, maxn, MPI_DOUBLE, &half_column_type);
    MPI_Type_commit(&half_column_type);

    first_x = 1;
    last_x = maxn / 2 - 1;
    first_y = 1;
    last_y = maxn / 2 - 1;

    /* Fill the rank */
    for (i = 0; i <= last_y; ++i)
        for (j = 0; j <= last_x + 1; ++j)
            xlocal[i * maxn + j] = rank;

    /* Fill in the border -1 values */
    for (j = 0; j < maxn / 2 + 1; j++)
    {
        xlocal[(first_y - 1) * maxn + j] = -1;
        xlocal[(last_y + 1) * maxn + j] = -1;
        // fill the spots where we will receive a row or column
        // not needed, but helpful for debugging
        if (rank % 2 == 1)
            xlocal[j * maxn + (first_x - 1)] = -1;
        else
            xlocal[j * maxn + (last_x + 1)] = -1;
    }

    itcnt = 0;
    do
    {

        // Exchange vertically
        if (rank >= 2) // bottom 2 blocks send their first row up
            MPI_Send(&xlocal[(first_y * maxn) + first_x], 1, half_row_type, rank - 2, 0, MPI_COMM_WORLD);
        if (rank < 2) // top 2 blocks receive a row from bottom at the last_y+1 row
            MPI_Recv(&xlocal[(last_y + 1) * maxn + first_x], 1, half_row_type, rank + 2, 0, MPI_COMM_WORLD, &status);
        if (rank < 2) // top 2 blocks send their last row down
            MPI_Send(&xlocal[(last_y * maxn) + first_x], 1, half_row_type, rank + 2, 0, MPI_COMM_WORLD);
        if (rank >= 2) // bottom 2 blocks receive a row from top at the first_y-1 row
            MPI_Recv(&xlocal[(first_y - 1) * maxn + first_x], 1, half_row_type, rank - 2, 0, MPI_COMM_WORLD, &status);

        // Exchange horizontally
        if (rank % 2 == 0) // left 2 blocks send their last column to the right (ranks 1,3)
            MPI_Send(&xlocal[(first_y * maxn) + last_x], 1, half_column_type, rank + 1, 0, MPI_COMM_WORLD);
        if (rank % 2 == 1) // right 2 blocks receive a column from left (ranks 0,2)
            MPI_Recv(&xlocal[(first_y * maxn)], 1, half_column_type, rank - 1, 0, MPI_COMM_WORLD, &status);
        if (rank % 2 == 1) // right 2 blocks send their first column to the left (ranks 1,3)
            MPI_Send(&xlocal[(first_y * maxn) + first_x], 1, half_column_type, rank - 1, 0, MPI_COMM_WORLD);
        if (rank % 2 == 0) // left 2 blocks receive a column from right (ranks 0,2)
            MPI_Recv(&xlocal[(first_y * maxn) + last_x + 1], 1, half_column_type, rank + 1, 0, MPI_COMM_WORLD, &status);

        itcnt++;
        diffnorm = 0.0;
        for (i = first_y; i <= last_y; i++)
            for (j = first_x; j <= last_x; j++)
            {
                xnew[(i * maxn) + j] = (xlocal[(i * maxn) + (j + 1)] + xlocal[(i * maxn) + (j - 1)] +
                                        xlocal[(i + 1) * maxn + j] + xlocal[(i - 1) * maxn + j]) /
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
            printf("At iteration %d, diff is %e\n", itcnt,
                   gdiffnorm);
    } while (gdiffnorm > 1.0e-2 && itcnt < 100);

    MPI_Finalize();
    return 0;
}