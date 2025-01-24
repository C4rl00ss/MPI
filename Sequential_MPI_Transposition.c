#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#define AUTOMATIC_MATRIX_SIZE 8192




//TRANSPOSE FUNCTIONS
/////////////////////////////////////////////////////////////////////////////////////////

// SEQUENTIAL
void matTranspose(float** matrix, float** transpose, int N) {
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            transpose[j][i] = matrix[i][j];
        }
    }
}

///////////////////////////////////////////////////////////////////////////////////////







//OTHER FUNCTIONS
////////////////////////////////////////////////////////////////////////////////////////

// Symmetry control function
void checkSym(float** matrix, int N){
    for(int i =0; i<N; i++){
        for (int j=0; j<N; j++){
            if (matrix[i][j] != matrix[j][i]){
                printf("The matrix is not symmetric.\n");
                return;
            }
        }
    }
    printf("The matrix is symmetric.\n");
}

// matrix initialization function with random float values
void initializeMatrix(float** matrix, int N) {
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            matrix[i][j] = (float)rand() / RAND_MAX * 10.0; //from 0 to 10.0
        }
    }
}

// print matrix function
void printMatrix(float** matrix, int N) {
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            printf("%6.2f ", matrix[i][j]);
        }
        printf("\n");
    }
}

////////////////////////////////////////////////////////////////////////////////////////








int main() {
    

//conditional code for manual insertion of the size of the matrices or automatic insertion
//-----------------------------------------------------------------------------------------
#ifdef Automatic
    int N = AUTOMATIC_MATRIX_SIZE; //automatic insertion
#else
    int N;
    bool inputVerification = true;
    // input verification
    while (inputVerification) {
        printf("Enter the size of the matrix (N x N): ");
        scanf("%d", &N);
        //&& ((N & (N - 1)) == 0)
        if ((N > 0) ) { 
            inputVerification = false;
        } else {
            printf("The number entered is not a power of 2.\n");
        }
    }
#endif
//-----------------------------------------------------------------------------------------
    




    // Dynamic matrix allocation
    float** matrix = (float**)malloc(N * sizeof(float*));
    float** transpose = (float**)malloc(N * sizeof(float*));
    for (int i = 0; i < N; i++) {
        matrix[i] = (float*)malloc(N * sizeof(float));
        transpose[i] = (float*)malloc(N * sizeof(float));
    }
    
    //time variables initialization
    clock_t start, end;
    double cpu_time_used;

    // Initialize the array with random values
    srand(time(NULL));
    initializeMatrix(matrix, N);
    
    
    //symmetry controll call function
    checkSym(matrix, N);
    
  
  
  
  
    
//EXECUTION OF FUNCTIONS
///////////////////////////////////////////////////////////////////////////////////////////

    
    printf("//////////////////////////////////////////////////////////\n");
    
    
    //SEQUENTIAL

    start = clock();
    matTranspose(matrix, transpose, N);
    end = clock();

    cpu_time_used = ((double)(end - start)) / CLOCKS_PER_SEC;

    printf("Tempo di esecuzione: %f seconds\n", cpu_time_used);


    printf("//////////////////////////////////////////////////////////\n");

    

//////////////////////////////////////////////////////////////////////////////////////////
   
    
    
    //IF YOU WANT TO PRINT THE TRANSPOSED MATRIX, REMOVE THE COMMENTS
    /*
    printf("Original Matrix:\n");
    printMatrix(matrix, N);
    printf("\n");
    printf("\nMatrice trasposta:\n");
    printMatrix(Transposed matrix, N);
    */


    
    // Deallocazione della memoria
    for (int i = 0; i < N; i++) {
        free(matrix[i]);
        free(transpose[i]);
    }
    free(matrix);
    free(transpose);

    return 0;
}












