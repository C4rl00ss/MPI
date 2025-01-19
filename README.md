# MPI
## IMPLICIT AND EXPLICIT PARALLELISM
### Repository description and file description
In this repository there are two pbs files ("runMatrix.pbs" and "runBuffer.pbs") and two c codes ("Matrix_Parallel_MPI.c" and "Buffer_Parallel_MPI"). il file pbs "runMatriz.pbs" compila ed esegue il codice "Matrix_Parallel_MPI.c", mentre il file "runBuffer.pbs" compila ed esegue il codice "Buffer_Parallel_MPI". Nel successivo paragrafo viene descritto come eseguire i file.
The difference between the two codes lies solely in the method used to perform the matrix transposition. As stated in the report, in the file Matrix_Parallel_MPI.c, the transposition was carried out by converting the original matrix into a buffer, which was then divided among the various processes. Each process reconstructed a matrix from the received buffer, computed its transpose, and then reconverted the transposed matrix into a buffer, sending it back to the parent process. The parent process then reconstructed the complete transposed matrix.

In contrast, the second code directly performs the transposition using the received buffer, avoiding matrix reconversions. Essentially, the two codes are equivalent and differ only in the MatTranspose function, which handles transposition using different approaches (the execution times differ very slightly, so running one code over the other does not make much of a difference).


### How to execute the codes with pbs files
- __It is recommended to first read the flag session regarding the -DAutomatic flag for setting the matrix size.__   
- Each PBS file contains all the commands required to compile and execute one of the two C files  
- All the codes are written in c and compiled using GCC. The version of the compiler is gcc-9.1.0. For the correct use of MPI, the MPICH library was introduced in its version mpich-3.2.1 compiled by GCC in the gcc-9.1.0 version (mpich-3.2.1--gcc-9.1.0) .
- To simplify the explanation we will only talk about the pbs file "runMatrix.pbs" and therefore its related code "Matrix_Parallel_MPI.c" because if you want to run the other code the procedure is the same.


To run the PBS file, you need to:  
1. Copy the "runMatrix.pbs" file and the "Matrix_Parallel_MPI.c" file to the cluster.  
2. Open the PBS file and modify the line indicating the location of the C file to be compiled (comments in the PBS file guide this operation).  
3. Save the PBS file.
5. Finally, submit the PBS file using the directive `qsub runMatrix.pbs` in the cluster's bash terminal, and the file will be processed.

### How to execute the main code in a interactive session
- __It is recommended to first read the flag session regarding the -DAutomatic flag for setting the matrix size__
-  To simplify the explanation we will only talk about the pbs file "runMatrix.pbs" and therefore its related code "Matrix_Parallel_MPI.c" because if you want to run the other code the procedure is the same.    
1. Copy the "Matrix_Parallel_MPI.c" file to the cluster.
2. Open an interactive session with 1 node, 96 cpus ,96 mpi processes, 3 gb of RAM
3. load the module gcc91 for the c compiler GCC 9.1.0
4. load the module  mpich-3.2.1--gcc-9.1.0 in order to compile MPI
5. Use this to compile the code: "mpicc Matrix_Parallel_MPI.c -o matrix_paralel_comp -DAutomatic" ( for the other file use "mpicc Buffer_Parallel_MPI.c -o buffer_parallel_comp -DAutomatic")
7. Run the output file "matrix_paralel_comp" using the following command "mpirun -np NUM_PROC ./matrix_paralel_comp", change "NUM_PROC" to the number of processes you want to be created (only power of two). (for the other file use "mpirun -np NUM_PROC ./buffer_parallel_comp") 
8. If you want to manually enter the size of the matrix, remove the flag "-DAutomatic", compile and then execute the output file. The program will ask you the matrix sixe. The only accepted values are numbers ​​of the power of two.

### What resources does the pbs file require:
The pbs file requests a node with 2GB of RAM and 96 available CPUs from the cluster for 5 minutes. 

### Output files 
The PBS file "runMatrix.pbs" will generate three outputs:  
- The compiled code, named "matrix_paralel_comp",  
- The error file, named "pbs_matrix_error.e",  
- And the output file, named "pbs_matrix_output.o".

- The PBS file "runBuffer.pbs" will generate three outputs:  
- The compiled code, named "buffer_parallel_comp",  
- The error file, named "pbs_buffer_error.e",  
- And the output file, named "pbs_buffer_output.o".  

### What's in the output file "pbs_matrix_output.o" and "pbs_buffer_output.o" and how to read it
- Nei file di output all'inizio vengono descritti tutti i nodi usati per eseguire il codice che, di default, è uno solo perche per questo progetto si è preferito rimanere su un solo nodo anche se ci sarebbe stata la possibilita di espandersi.

- The codes execute the matrix transposition using, by default, a square matrix of size **N x N**, where **N = 8192**, __thanks to the `-DAutomatic` flag__ (see the "Flags" section for a detailed explanation of the flags used). 

- L'output visualizzato consisterà in 7 esecuzioni del codice. Ogni esecuzione utilizzerà un numero diverso di processi per calcolare la trasposta della matrice. In particolare, il numero di processi sarà una potenza di 2, partendo da 1 fino a raggiungere 64. Per una migliore comprensione dell'output, verranno indicati sia il numero dell'esecuzione sia il corrispondente numero di processi utilizzati. In poche parole viene eseguito il weak scaling usando una matrice di **N = 8192**.

 


### Flags 
The only two required flags are -fopenmp and -DAutomatic:
- ##### fopenm
This flag enables the OpenMP library, which is used in the project for three key purposes:
1. Measuring execution time with the omp_get_wtime function.
2. Using directives like #pragma omp simd to optimize the code through vectorization.
3. Enabling parallel programming with directives such as #pragma omp parallel.

- ##### DAutomatic:
This flag eliminates the need to manually specify the matrix size at runtime. When the flag is used, the program automatically sets the matrix size to 8192. This is defined in the header of each C file by the line *#define AUTOMATIC_MATRIX_SIZE 8192*, which assigns the value 8192 to the matrix size **N**. This flag can be removed during compilation, but __NOT IN THE PBS FILE__, as the code will then prompt the user for the matrix size, which cannot be provided in a PBS batch script, leading to execution failure.   
__IF YOU WANT TO REMOVE THE -DAutomatic FLAG IN ORDER TO SET THE MATRIX SIZE  (ONLY VALUES THAT ARE POWERS OF 2), YOU MUST OPEN AN INTERACTIVE SESSION.__   

- Alternatively, to change the matrix size, you can edit the C code "Best_implicit_and_explicit_combinations.c" and replace the line *#define AUTOMATIC_MATRIX_SIZE 8192* with a desired value, while keeping the -DAutomatic flag so the pbs file with a different matrix size can be used.



### OTHER FILES


### General structure of the codes:
1. ##### Function declaration
   The codes are structured in a very similar way to each other: at the beginning of the file there is the function that perform the transpose of the matrix __matTranspose__.  Immediately after the transpose functions comes the implementation of the __checkSym__ functions for checking the symmetry of the matrix, __initializeMatrix__ for the initialization with random float values ​​from 0 to 10.0 of the matrix and finally __printMatrix__ which prints the matrix that is passed to it as input.

2. ##### Main
   The main starts with a piece of conditional code that checks whether the **Automatic** flag is present (which must be added at compile time via the **-DAutomatic** flag). If the flag is not present the code asks the user to enter the size of the Matrix which must be a number of the order of the power of 2 and if this condition is not satisfied the code will exit the execution. If the flag is present, the code skips the manual input process by the user and assigns to the variable **N** the value defined in AUTOMATIC_MATRIX_SIZE which by default corresponds to 2^13= 8192 on all codes.
Immediately after comes the allocation of the matrix memory in the heap, its random initialization, the definition of the variables that will be used to calculate the time and finally the verification of the symmetry of the matrix.
This is followed by calls to all the implemented functions, for each one the execution time is measured and this value is printed on the screen.
Finally, the last part of the code deallocates the matrix memory.











