#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#define AUTOMATIC_MATRIX_SIZE 8192





//TRASPOSE FUNCTION
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void MatTranspose(float ** local_transposed, float  ** local_rows, int rows_per_proc, int N){
    for (int i = 0; i < rows_per_proc; i++) {
        for (int j = 0; j < N; j++) {
            local_transposed[j][i] = local_rows[i][j];    //takes a row of the matrix and turns it into a column  
        }
    }
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



//ALL THE FUNCTIONS
//////////////////////////////////////////////////////////////////////////////////////////////////


//FUNCTION TO ALLOCATE A DYNAMIC MATRIX IN THE HEAP
float** allocate_matrix(int rows, int cols) {
    float** matrix = (float**)malloc(rows * sizeof(float*));  // Allocate an array of pointers for the rows
    for (int i = 0; i < rows; i++) {
        matrix[i] = (float*)malloc(cols * sizeof(float)); // Allocate memory for the columns of each row
    }
    return matrix;
}


//FUNCTION TO INITIALIZE THE MATRIX 
void initializeMatrix(float** matrix, int N) {
    srand(time(NULL));
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            matrix[i][j] = (float)rand() / RAND_MAX * 10.0; // Values from 0 to 10.0
        }
    }
}


//FUNCTION TO DEALLOCATE MEMORY
void free_matrix(float **matrix, int rows) {
    for (int i = 0; i < rows; i++) {
        free(matrix[i]);
    }
    free(matrix);
}


//FUNCTION TO CHECK THE SYMMETRY
void checkSym(float** matrix, int N) {
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            if (matrix[i][j] != matrix[j][i]) {
                printf("\n The matrix is not symmetric\n");
                return;
            }
        }
    }
    printf("\n The matrix is symmetric\n");
}


// FUNCTION TO PRINT A MATRIX
void print_matrix(float **matrix, int rows, int cols) {
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            printf("%.2f ", matrix[i][j]);
        }
        printf("\n");
    }
}


//////////////////////////////////////////////////////////////////////////////////////////////////





int main(int argc, char *argv[]) {
    int rank, size;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);


//conditional code for manual insertion of the size of the matrices or automatic insertion
//-----------------------------------------------------------------------------------------
   
    int N;
    if (rank == 0) { // The master process read the input
        #ifdef Automatic
            N = AUTOMATIC_MATRIX_SIZE; // If the flag -DAutomatic is present the size is the one defined in the header
            if(N%size!=0){
                printf("the matrix size is not divisible by the number of processes entered, enter a number of processes that is a power of two\n");
                MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
            }
        #else
            bool inputVerification = true;
            // Input verification 
            while (inputVerification) {
                printf("Enter the size of the matrix (N x N): ");
                scanf("%d", &N); 
        
                if ((N > 0)&& ((N & (N - 1)) == 0 )&&(N % size == 0)) { 
                    inputVerification = false;
                } else {
                    printf("the size of the entered matrix or the nuber of processes is not a power of two \n");
                    MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
                }
            }
        #endif
    }

    // all the processes recieve N 
    MPI_Bcast(&N, 1, MPI_INT, 0, MPI_COMM_WORLD);

//-----------------------------------------------------------------------------------------







// Matrix initialization and printing 
//------------------------------------------------------------------------

    int rows_per_proc = N / size; // number of lines per process
    float **matrix = NULL;
    float **local_rows = allocate_matrix(rows_per_proc, N);
    float **transposed = NULL;
    double start_time, end_time;



    if (rank == 0) {
        matrix = allocate_matrix(N, N);
        initializeMatrix(matrix ,N);
        checkSym(matrix,N);
    }



    //IF YOU WANT PRINT THE ORIGINAL MATRIX REMOVE THE FOLLOWING COMMENTS
    /*
    if (rank == 0) {
        printf("Original matrix:\n");
        print_matrix(matrix, N, N);
    }
    */
    
//-------------------------------------------------------------------------


    // creation of the buffer to sent to the master process
    float *recv_buffer = NULL;
    if (rank == 0) {
        recv_buffer = malloc(N * N * sizeof(float));
    }
    


    // creation of the monodimensional buffer to send to  all the processes via scatter
    float *send_buffer = NULL;
    if (rank == 0) {
        send_buffer = malloc(N * N * sizeof(float));
        for (int i = 0; i < N; i++) {
            for (int j = 0; j < N; j++) {
                send_buffer[i * N + j] = matrix[i][j]; 
            }
        }
    }



    // creation of the local buffer present in each process that receives its own part of the matrix
    float *local_buffer = malloc(rows_per_proc * N * sizeof(float));
    
    if (rank==0){
        start_time = MPI_Wtime();
    }

    
    //receive buffer
    MPI_Scatter(send_buffer, rows_per_proc * N, MPI_FLOAT, local_buffer, rows_per_proc * N, MPI_FLOAT, 0, MPI_COMM_WORLD);

    


    // converting the local buffer (monodimensial array) to a matrix
    for (int i = 0; i < rows_per_proc; i++) {
        for (int j = 0; j < N; j++) {
            local_rows[i][j] = local_buffer[i * N + j];
        }
    }

    free(local_buffer); //deallocation of the received buffer
    if (rank == 0) {
        free(send_buffer);
    }





    // creation of the local matrix transopsed
    float **local_transposed = allocate_matrix(N, rows_per_proc);


    // compute transposed matrix 
    MatTranspose(local_transposed, local_rows, rows_per_proc, N);
    
 
    


    // converting the transposed matrix to monodimensional array (the buffer to send)
    float *local_transposed_buffer = malloc(rows_per_proc * N * sizeof(float));  
    for(int i =0; i<rows_per_proc; i++){
        for(int j=0; j<N; j++){
            local_transposed_buffer[j+i*N]= local_transposed[j][i]; //legge COLONNA PER COLONNA (per quello [j][i] della trasposta e la inserisce nel buffer da inviare
        }
    }
    
    

    
   
        
    //each process sends its buffer to the master process
    MPI_Gather(local_transposed_buffer, rows_per_proc * N, MPI_FLOAT, recv_buffer, rows_per_proc * N, MPI_FLOAT, 0, MPI_COMM_WORLD);
    
    
    if (rank==0){
        end_time = MPI_Wtime();
    }






//reconstruction by the master process of the transposed matrix and printing the results
//----------------------------------------------------------------------------------------------------------------

    if (rank == 0) {
        
        transposed = allocate_matrix(N, N);
        for(int i=0; i<N; i++){
            for(int j=0; j<N ;j++){
                transposed[j][i]=recv_buffer[i*N+j];  //reconstruction
            }
        }
        
        printf("//////////////////////////////////////////\n");
        printf("Tempo di esecuzione: %f secondi\n", end_time - start_time);
        printf("//////////////////////////////////////////\n");
        
        //IF YOU WANT PRINT THE TRASPOSED MATRIX REMOVE THE FOLLOWING COMMENTS
        /*
        printf("\nMatrice trasposta:\n");
        print_matrix(transposed, N, N);
        */

        //matrix transposed dellocation
        free_matrix(transposed, N);
        free(recv_buffer);
    }
//----------------------------------------------------------------------------------------------------------------





    //deallocation of everything
    free_matrix(local_rows, rows_per_proc);
    free_matrix(local_transposed, N);
    free(local_transposed_buffer);

    if (rank == 0) {
        free_matrix(matrix, N);
    }

    MPI_Finalize();
    return 0;

}
