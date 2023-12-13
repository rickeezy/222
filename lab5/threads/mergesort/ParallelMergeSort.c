#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#define SIZE 100

int array[SIZE];

void fillArrayWithRandomNumbers(int arr[SIZE]) {
    for(int i = 0; i<SIZE; i++) array[i] = rand()%100;
}

void printArray(int arr[SIZE]) {
    for(int i = 0; i<SIZE; i++) printf("%5d", arr[i]);
    printf("\n");
}

typedef struct StartEndIndexes {
    int start;
    int end;
} StartEndIndexes;

//merge the two sorted halves
void merge(int arr[SIZE], int start, int mid, int end) {
	int num1 = mid - start + 1;
	int num2 = end - mid;

	int left[num1], right[num2];

	//populate some temporary arrays with the two halves of the given array
	for (int i = 0; i < num1; i++) {
		left[i] = arr[start + i];
	}
	for (int i = 0; i < num2; i++) {
		right[i] = arr[mid + 1 + i];
	}

	int i = 0;
	int j = 0;
	int k = start;

	while (i < num1 && j < num2) {
		if (left[i] <= right[j]) {
			arr[k] = left[i];
			i++;
		}
		else {
			arr[k] = right[j];
			j++;
		}
		k++;
	}

	while (i < num1) {
		arr[k] = left[i];
		i++;
		k++;
	}
	while (j < num2) {
		arr[k] = right[j];
		j++;
		k++;
	}
}

// Runs mergesort on the array segment described in the
// argument. Spawns two threads to mergesort each half
// of the array segment, and then merges the results.
void* mergeSort(void* args) {
	StartEndIndexes* indexes = (StartEndIndexes*) args;
	int start = indexes -> start;
	int end = indexes -> end;

	if (start < end) {
		int mid = start + (end - start) / 2;

		pthread_t thread1, thread2;
		StartEndIndexes index1, index2;

		index1.start = start;
		index1.end = mid;

		index2.start = mid + 1;
		index2.end = end;

		pthread_create(&thread1, NULL, mergeSort, (void*)&index1);
		pthread_create(&thread2, NULL, mergeSort, (void*)&index2);

		pthread_join(thread1, NULL);
		pthread_join(thread2, NULL);

		merge(array, start, mid, end);
	}

	pthread_exit(NULL);
}

int main() {
    srand(time(0));
    StartEndIndexes sei[3];

		int third = SIZE / 3;
		int rem = SIZE % 3;
		for (int i = 0; i < 3; i++) {
			sei[i].start = i * third;
			sei[i].end = (i+1) * third - 1;

			if (i == 2 && rem > 0) {
				sei[i].end += rem;
			}
		}
    
    // 1. Fill array with random numbers.
    fillArrayWithRandomNumbers(array);
    
    // 2. Print the array.
    printf("Unsorted array: ");
    printArray(array);
    
    // 3. Create a up to 3 threads for merge sort.
		pthread_t main_threads[3];
		for (int i = 0; i < 3; i++) {
			pthread_create(&main_threads[i], NULL, mergeSort, (void*)&sei[i]);
		}

    
    // 4. Wait for mergesort to finish.
		for (int i = 0; i < 3; i++) {
			pthread_join(main_threads[i], NULL);
		}

		//4.5 Merge the three sorted segments back into a single sorted array		
		merge(array, sei[0].start, sei[0].end, sei[1].end); //segment 1 and 2
		merge(array, sei[0].start, sei[1].end, sei[2].end); //segment (1 and 2) and 3
    
    // 5. Print the sorted array.
    printf("Sorted array:   ");
    printArray(array);

		return 0;
}
