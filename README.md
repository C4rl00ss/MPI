# MPI
### Repository description and file description
This repository contains two PBS files (runMatrix.pbs and runBuffer.pbs) and three C codes (Matrix_Parallel_MPI.c, Buffer_Parallel_MPI.c and Sequential_MPI_Transposition.c).

   - The PBS file runMatrix.pbs compiles and executes the Matrix_Parallel_MPI.c code and Sequential_MPI_Transposition.c.
   - The PBS file runBuffer.pbs compiles and executes the Buffer_Parallel_MPI.c code and Sequential_MPI_Transposition.c.

In the following section, the execution of these files is explained.
The difference between the two codes (Matrix_Parallel_MPI.c and Buffer_Parallel_MPI.c) lies in the method used to perform the matrix transposition:
   1. ##### Matrix_Parallel_MPI.c
In this code, the transposition is performed by converting the original matrix into a buffer, which is then split and distributed among the processes using MPI_Scatter. Each process reconstructs a matrix from the received buffer, computes its transpose, reconverts the transposed matrix into a buffer, and sends it back to the parent process using MPI_Gather. The parent process then reconstructs the complete transposed matrix from the received buffers.

  2. ##### Buffer_Parallel_MPI.c
This code performs the transposition directly on the received buffer, avoiding the need to convert between matrices and buffers during the process. The buffer is directly manipulated for transposition, simplifying the implementation.

In essence, the two codes are equivalent and differ only in the implementation of the MatTranspose function, which handles the transposition using different approaches. The execution times of the two methods are nearly identical, so running one code over the other does not lead to significant differences in performance.

The **Sequential_MPI_Transposition.c** code performs the matrix transposition sequentially and serves as a baseline for comparing execution times. **For this reason, it is executed by the PBS files**.
Therefore, to successfully run either of the PBS files, you must ensure that the repository contains both Matrix_Parallel_MPI.c (or Buffer_Parallel_MPI.c) and the sequential code. If the sequential file is missing, the PBS script will result in an error.



### How to execute the codes with pbs files
- __It is recommended to first read the flag section regarding the -DAutomatic flag for setting the matrix size.__   
- Each PBS file contains all the commands required to compile and execute the Sequential_MPI_Transposition.c file and one of the two C files Matrix_Parallel_MPI.c or Buffer_Parallel_MPI.c
- All the codes are written in c and compiled using GCC. The version of the compiler is gcc-9.1.0. For the correct use of MPI, the MPICH library was introduced in its version mpich-3.2.1 compiled by GCC in the gcc-9.1.0 version (mpich-3.2.1--gcc-9.1.0) .
- To simplify the explanation we will only talk about the pbs file "runMatrix.pbs" and therefore its related code "Matrix_Parallel_MPI.c" because if you want to run the other code the procedure is the same.


To run the PBS file, you need to:  
1. Copy runMatrix.pbs, Matrix_Parallel_MPI.c, and Sequential_MPI_Transposition.c to the cluster.  
2. Open the PBS file and modify the line indicating the location of the C files to be compiled (comments in the PBS file guide this operation).  
3. Save the PBS file.
5. Finally, submit the PBS file using the directive `qsub runMatrix.pbs` in the cluster's bash terminal, and the file will be processed.

### How to execute the MPI codes in a interactive session
- __It is recommended to first read the flag section regarding the -DAutomatic flag for setting the matrix size__
-  To simplify the explanation we will only talk about the Matrix_Parallel_MPI.c file because if you want to run the other code Buffer_Parallel_MPI.c the procedure is the same.    
1. Copy the Matrix_Parallel_MPI.c file to the cluster.
2. Open an interactive session with 1 node, 96 cpus ,96 mpi processes, 4 gb of RAM 
3. load the module gcc91 for the c compiler GCC 9.1.0
4. load the module  mpich-3.2.1--gcc-9.1.0 in order to compile MPI
5. Use this to compile the code: "mpicc Matrix_Parallel_MPI.c -o matrix_parallel_comp -DAutomatic" ( for the other file use "mpicc Buffer_Parallel_MPI.c -o buffer_parallel_comp -DAutomatic")
7. Run the output file "matrix_paralel_comp" using the following command "mpirun -np NUM_PROC ./matrix_parallel_comp", change "NUM_PROC" to the number of processes you want to be created (only power of two). (for the other file use "mpirun -np NUM_PROC ./buffer_parallel_comp") 
8. If you want to manually enter the size of the matrix, remove the flag "-DAutomatic", compile and then execute the output file. the program will ask you for the matrix size. The only accepted values are numbers ​​of the power of two.

### What resources does the pbs file require:
The pbs file requests a node with 4GB of RAM and 96 available CPUs from the cluster for 5 minutes. 

### Output files 
The PBS file "runMatrix.pbs" will generate four outputs:  
- Two compiled code, named "matrix_paralel_comp" and "sequential_comp",  
- The error file, named "pbs_matrix_error.e",  
- And the output file, named "pbs_matrix_output.o".

The PBS file "runBuffer.pbs" will generate four outputs:  
- Two compiled code, named "buffer_parallel_comp" and "sequential_comp",  
- The error file, named "pbs_buffer_error.e",  
- And the output file, named "pbs_buffer_output.o".  

### What's in the output file "pbs_matrix_output.o" and "pbs_buffer_output.o" and how to read it
- At the beginning of the output files, all the nodes used to execute the code are listed. By default, only one node is used, as this project was designed to remain within a single-node setup, even though it could have been expanded to multiple nodes.
  
- Immediately after, the **sequential** execution time is displayed, which is obtained from the Sequential_MPI_Transposition.c file.
  
- After that, a list of 7 execution times done by the MPI code is displayed, indicating the time taken to compute the transpose of a matrix of size of **N x N**, where **N = 8192**.  Each execution uses a different number of processes. The execution number and the corresponding number of processes used are included in the output for better clarity. The number of processes used is a power of 2, starting from 1 and going up to 64.


### Flags 
The only required flag is -DAutomatic:
- ##### DAutomatic:
This flag eliminates the need to manually specify the matrix size at runtime. When the flag is used, the program automatically sets the matrix size to 8192. This is defined in the header of each C file by the line *#define AUTOMATIC_MATRIX_SIZE 8192*, which assigns the value 8192 to the matrix size **N**. This flag can be removed during compilation, but __NOT IN THE PBS FILE__, as the code will then prompt the user for the matrix size, which cannot be provided in a PBS batch script, leading to execution failure.   
__IF YOU WANT TO REMOVE THE -DAutomatic FLAG IN ORDER TO SET THE MATRIX SIZE  (ONLY VALUES THAT ARE POWERS OF 2), YOU MUST OPEN AN INTERACTIVE SESSION.__   

- Alternatively, to change the matrix size, you can edit the C code "Matrix_Parallel_MPI.c" (or in "Buffer_Parallel_MPI.c") and replace the line *#define AUTOMATIC_MATRIX_SIZE 8192* with a desired value, while keeping the -DAutomatic flag so the pbs file with a different matrix size can be used.




### General structure of the codes:
1. ##### Function declaration
   The codes are structured in a very similar way to each other: at the beginning of the file there is the function that perform the transpose of the matrix __matTranspose__.  Immediately after the transpose functions comes the implementation of the __checkSym__ functions for checking the symmetry of the matrix, __initializeMatrix__ for the initialization with random float values ​​from 0 to 10.0 of the matrix and finally __printMatrix__ which prints the matrix that is passed to it as input.

2. ##### Main
   The main begins by initializing the MPI environment using the MPI_Init function. This function is essential for setting up the environment needed for communication between parallel processes. Once the initialization is complete, each process runs the code in parallel. Then, there is a conditional block that checks whether the **Automatic** flag is defined (which must be added at compile time using the **-DAutomatic** flag). If the flag is not present, the code prompts the user to manually input the matrix size, which must be a power of 2. If this condition is not met, the program terminates execution. If the flag is present, the code bypasses the manual input step and automatically assigns the value defined in AUTOMATIC_MATRIX_SIZE to the variable **N**. By default, AUTOMATIC_MATRIX_SIZE is set to  2^13= 8192 for both the code.
After determining the matrix size, the program proceeds to allocate memory for the matrix on the heap, perform its random initialization, define variables used for timing, and finally verify the matrix's symmetry.
Following the preparation of the original matrix and necessary memory spaces, the matrix is split into chunks using __MPI_Scatter__ and distributed to all processes. Each process computes the transpose of its chunk and sends it back to the root process using __MPI_Gather__.
The root process, in the meantime, calculates the total time required for all these operations.
The program concludes by deallocating the allocated memory and releasing the resources used by MPI with a call to MPI_Finalize.











