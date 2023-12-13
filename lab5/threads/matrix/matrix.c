#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

int MAX;

int **matA; 
int **matB; 

int **matSumResult;
int **matDiffResult; 
int **matProductResult;

typedef struct {
    int start_row;
    int end_row;
} thread_args;

void fillMatrix(int **matrix) {
    for(int i = 0; i<MAX; i++) {
        for(int j = 0; j<MAX; j++) {
            matrix[i][j] = rand()%10+1;
        }
    }
}

void printMatrix(int **matrix){
    for(int i = 0; i<MAX; i++) {
        for(int j = 0; j<MAX; j++) {
            printf("%5d", matrix[i][j]);
        }
        printf("\n");
    }
    printf("\n");
}

// // Fetches the appropriate coordinates from the argument, and sets
// // the cell of matSumResult at the coordinates to the sum of the
// // values at the coordinates of matA and matB.
// void* computeSum(void* args) { // pass in the number of the ith thread
//     thread_args *t_args = (thread_args *) args;

//     int i = t_args -> start_row;
//     for (int j = 0; j < MAX; j++) {
//         matSumResult[i][j] = matA[i][j] + matB[i][j];
//     }
// 	pthread_exit(NULL);
// }

// // Fetches the appropriate coordinates from the argument, and sets
// // the cell of matSumResult at the coordinates to the difference of the
// // values at the coordinates of matA and matB.
// void* computeDiff(void* args) { // pass in the number of the ith thread
//     thread_args *t_args = (thread_args *) args;

//     int i = t_args -> start_row;
//     for (int j = 0; j < MAX; j++) {
//         matSumResult[i][j] = matA[i][j] - matB[i][j];
//     }
// 	pthread_exit(NULL);
// }

// // Fetches the appropriate coordinates from the argument, and sets
// // the cell of matSumResult at the coordinates to the inner product
// // of matA and matB.
// void* computeProduct(void* args) { // pass in the number of the ith thread
//     thread_args *t_args = (thread_args *) args;

//     int i = t_args -> start_row;
//     for (int j = 0; j < MAX; j++) {
//         matProductResult[i][j] = 0;
//         for (int k = 0; k < MAX; k++) {
//             matSumResult[i][j] += matA[i][k] * matB[k][j];
//         }
//     }
//     pthread_exit(NULL);
// }

void* computeOperations(void* args) {
    thread_args *t_args = (thread_args *) args;

    int i = t_args -> start_row;
    for (int j = 0; j < MAX; j++) {
        //Addition
        matSumResult[i][j] = matA[i][j] + matB[i][j];

        //Subtraction
        matDiffResult[i][j] = matA[i][j] - matB[i][j];

        //Multiplication
        matProductResult[i][j] = 0;
        for (int k = 0; k < MAX; k++) {
            matProductResult[i][j] += matA[i][k] * matB[k][j];
        }
    }
    pthread_exit(NULL);
}
// Spawn a thread to fill each cell in each result matrix.
// How many threads will you spawn?
int main() {
    srand(time(0));  // Do Not Remove. Just ignore and continue below.
    
    // 0. Get the matrix size from the command line and assign it to MAX
    printf("How many rows does the matrix have? (It will be square so the amount of rows will be eqaul to the amount of colums): ");
    scanf("%d", &MAX);
    int row, col = MAX;

    //0.5 Allocate memory to the matrices
    matA = (int **)malloc(MAX * sizeof(int *));
    matB = (int **)malloc(MAX * sizeof(int *));
    matSumResult = (int **)malloc(MAX * sizeof(int *));
    matDiffResult = (int **)malloc(MAX * sizeof(int *));
    matProductResult = (int **)malloc(MAX * sizeof(int *));

    for (int i = 0; i < MAX; i++) {
        matA[i] = (int *)malloc(MAX * sizeof(int *));
        matB[i] = (int *)malloc(MAX * sizeof(int *));
        matSumResult[i] = (int *)malloc(MAX * sizeof(int *));
        matDiffResult[i] = (int *)malloc(MAX * sizeof(int *));
        matProductResult[i] = (int *)malloc(MAX * sizeof(int *));
    }
    
    // 1. Fill the matrices (matA and matB) with random values.
    fillMatrix(matA);
    fillMatrix(matB);

    
    // 2. Print the initial matrices.
    printf("Matrix A:\n");
    printMatrix(matA);
    printf("Matrix B:\n");
    printMatrix(matB);
    
    // 3. Create pthread_t objects for our threads.
    pthread_t threads[MAX];

    thread_args thr_args[MAX];
    
    // 4. Create a thread for each cell of each matrix operation.
    // You'll need to pass in the coordinates of the cell you want the thread
    // to compute.
    // 
    // One way to do this is to malloc memory for the thread number i, populate the coordinates
    // into that space, and pass that address to the thread. The thread will use that number to calcuate 
    // its portion of the matrix. The thread will then have to free that space when it's done with what's in that memory.
    for (int i = 0; i < MAX; i++) {
        thr_args[i].start_row = i;
        pthread_create(&threads[i], NULL, computeOperations, &thr_args[i]);
    }
    
    // 5. Wait for all threads to finish.

    for (int i = 0; i < MAX; i++) {
        pthread_join(threads[i], NULL);
    }
    
    // 6. Print the results.
    printf("Results:\n");
    printf("Sum:\n");
    printMatrix(matSumResult);
    printf("Difference:\n");
    printMatrix(matDiffResult);
    printf("Product:\n");
    printMatrix(matProductResult);
    return 0;

    // 7. Free allocated memory
    for (int i = 0; i < MAX; i++) {
        free(matA[i]);
        free(matB[i]);
        free(matSumResult[i]);
        free(matDiffResult[i]);
        free(matProductResult[i]);
    }

    free(matA);
    free(matB);
    free(matSumResult);
    free(matDiffResult);
    free(matProductResult);

    return 0;
}