# Jacobi-Approximation

Algorithm pseudocode:
For each process with rank in {0,1,2,3}
    create xlocal (12x12 matrix)

    // initialize initial matrix (see examples below)
    set top and bottom boundary of xlocal = -1
    set right boundary of xlocal = -1 for ranks 0,2
    set left boundary of xlocal = -1 for ranks 1,3

    // Exchange vertically
    if (rank >= 2)  // bottom 2 blocks
        send first row of xlocal up
    if (rank < 2)  // top 2 blocks
        receive row from bottom, set it as the bottom most row of xlocal
    if (rank < 2)  // top 2 blocks
        send last row of xlocal down
    if (rank >= 2)  // bottom 2 blocks
        receive row from top, set it as the top most row of xlocal

    // Exchange horizontally
    if (rank % 2 == 0)  // left 2 blocks
        send last column to the right
    if (rank % 2 == 1)  // right 2 blocks
        receive column from left, set it as the first column of xlocal
    if (rank % 2 == 1)  // right 2 blocks
        send first column to the left
    if (rank % 2 == 0)  // left 2 blocks
        receive column from right, set it as the last column of xlocal

    compute xnew as usual using the inner points (xlocal[i][j] for i,j in {1,2,3,4,5})
    compute diffnorm as usual using the inner points (xlocal[i][j] for i,j in {1,2,3,4,5})
    transfer xnew to xlocal using the inner points (xlocal[i][j] for i,j in {1,2,3,4,5})
    map_allreduce to combine the result diffnorm into variable gdiffnorm
    calculate the sqrt of the gdiffnorm

    repeat the exchanges and diffs until convergence
            


RANK=0
-1  -1  -1  -1  -1  -1  -1
 0   0   0   0   0   0  -1
 0   0   0   0   0   0  -1
 0   0   0   0   0   0  -1
 0   0   0   0   0   0  -1
 0   0   0   0   0   0  -1
-1  -1  -1  -1  -1  -1  -1

RANK=1
-1  -1  -1  -1  -1  -1  -1
-1   1   1   1   1   1   1
-1   1   1   1   1   1   1
-1   1   1   1   1   1   1
-1   1   1   1   1   1   1
-1   1   1   1   1   1   1
-1  -1  -1  -1  -1  -1  -1

RANK=2
-1  -1  -1  -1  -1  -1  -1
 2   2   2   2   2   2  -1
 2   2   2   2   2   2  -1
 2   2   2   2   2   2  -1
 2   2   2   2   2   2  -1
 2   2   2   2   2   2  -1
-1  -1  -1  -1  -1  -1  -1

RANK=3
-1  -1  -1  -1  -1  -1  -1
-1   3   3   3   3   3   3
-1   3   3   3   3   3   3
-1   3   3   3   3   3   3
-1   3   3   3   3   3   3
-1   3   3   3   3   3   3
-1  -1  -1  -1  -1  -1  -1