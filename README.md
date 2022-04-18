# Jacobi-Approximation

Uses Jacobi iteration to approximate a system of linear equations. In this case, a Laplace equation with finite differences. The [original source code](https://www.mcs.anl.gov/research/projects/mpi/tutorial/mpiexmpl/src/jacobi/C/main.html) uses exchanges of rows vertically only. Each process is assigned a contiguous set of rows. 

This implementation assigns each process a quadrant (noncontiguous). Therefore, both partial rows and partial columns must be exchanged. The end result is the same, just the means to parallelizing is different.

# Usage
The code can be compiled and run using the makefile. The resulting matrix can be plotted using the make command as well.
```
// while in the src directory

make        # will make the files in the /build director
make run    # will use mpirun to execute the executable (this writes out the resulting matrix to data/output.csv)
make plot   # plot the data/output.csv data to a 2d heatmap and 3d surface plot (results saved as pdf in data/2d_plot.pdf and data/3d_plot.pdf) 
```

# References
https://www.mcs.anl.gov/research/projects/mpi/tutorial/mpiexmpl/src/jacobi/C/main.html
