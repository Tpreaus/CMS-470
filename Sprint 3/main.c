/**
* Theodore Preaus CMS-470
*
* Practice creating multi-threaded programs in C then use it to sort an array 
*/

#include <pthread.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <semaphore.h>

// Declaring the Needed Structs
struct sortingParams {
  double* location;
  int size;
};

struct mergingParams { 
  double* first;
  int firstsize;
  double *second;
  int secondsize;
  double *dest;
};

void * mergeThreads(void *arg) {
  // Load struct
  struct mergingParams *x = arg;
  // Get length
  int length = x->firstsize + x->secondsize;
  // Run for length of C
  // Check if length has been reached
  int firstCheck = 0;
  int secondCheck = 0;
  // Iterating through the diffrent array 
  int arr1 = 0;
  int arr2 = 0;
  for (int i = 0; i < length; i++) {
    // check if whole array has been used
    if (arr1 == x->firstsize) {
      firstCheck = 1;
    }
    if (arr2 == x->secondsize) {
      secondCheck = 1;
    } 
    // If each array still has arguments run comparison
    if ((firstCheck == 0) && (secondCheck == 0)) {
      if (x->first[arr1] < x->second[arr2]) {
        x->dest[i] = x->first[arr1];
        arr1 += 1;
      } else {
          x->dest[i] = x->second[arr2];
          arr2 += 1;
      }
    }
    // If fisrt array empty fill remaning with second
    if (firstCheck == 1) {
      x->dest[i] = x->second[arr2];
      arr2 += 1;
    }
    // If second array empty fill remaning with first
    if (secondCheck == 1) {
      x->dest[i] = x->first[arr1];
      arr1 += 1;
    }
  }
  return 0;
}

//Function to gen ran double in range 1.0 - 1000.0
double ranDouble() {
  double min = 1.0;
  double max = 1000.0;
  // generate random int and convert it to double 
  double random1 = ((double) rand() / RAND_MAX);
  double range = random1 * (max - min);
  double num = min + range;
  return num;
}

// Swap two items in a array 
void swapARG(double* x, double* y) {
  double temp = *x;
  *x = *y;
  *y = temp;
}
// Sort using an array
void* sort(void *data) {
  struct sortingParams *x = data;
  // for loop to increase the minimum index with every step 
  for(int i = 0; i < x->size - 1; i++) {
    // double arr[] = x.location;
    int min_index = i;
    for(int k = i+1; k < x->size; k++) {
      if(x->location[k] < x->location[min_index]) {
       min_index = k;
      }
    }
    if(min_index != i) {
      swapARG(&x->location[min_index], &x->location[i]);
    } 
  }
  return 0;
}

void copyArr(double* x, double* y, int n) {
  // N = length of arrays
  for (int i = 0; i < n; i++) {
    y[i] = x[i];
  }
}

int main(int argc, char *argv[]) {
  //Declare seed using the current time for the RAND function
  srand(time(NULL));
  // Int n to hold size of arrray
  int n;
  // Declare threads
  pthread_t threadB, thread1, thread2, threadM;
  // Create timespec
  struct timespec ts_begin, ts_end;
	double elapsed; 
  
  // Check for command line arguments
  if( argc == 2 ) {
    // Scan the char* into an int
    sscanf(argv[1], "%d", &n);
  }else if( argc > 2 ) {
      printf("Too many arguments supplied.\n");
      return 1;
  }else {
      printf("One argument expected.\n");
      return 1;
  }
  if (n < 1) {
    printf("Enter a positive integer.\n");
    return 0;
  }
  
  // Declare the arrays of size n
  double A[n];
  // Fill Array A with random Doubles
  for (int i = 0; i < n; i++) {
    A[i] = ranDouble();
  }
  // Declare other double*
  double B[n];
  double firstHalf[(n/2)];
  double secondHalf[(n - (n/2))];
  double C[n];
  
  // ONE THREAD CASE /////////////////////////////
  // Copy A ==> B
  copyArr(A, B, n);
  
  // Get Start time
  clock_gettime(CLOCK_MONOTONIC, &ts_begin);
  
  // Create Struct to sort 
  struct sortingParams firstArray;
  firstArray.location = B;
  firstArray.size = n;
  
  // Create Thread
  pthread_create(&threadB, NULL, sort, &firstArray);

  // Join Thread
  pthread_join(threadB, NULL);

  clock_gettime(CLOCK_MONOTONIC, &ts_end);  
	elapsed = ts_end.tv_sec - ts_begin.tv_sec; 
	elapsed += (ts_end.tv_nsec - ts_begin.tv_nsec) /       1000000000.0;

  // Print Elapsed time
  printf("\nSorting by ONE thread is done in: %f ms", elapsed*1000);
  
  // TWO THREAD CASE ////////////////////////////
  // Split A in two
  for (int i = 0; i < n; i++) {
    if (i < n/2) {
      firstHalf[i] = A[i];
    } else {
        secondHalf[i - n/2] = A[i];
    }
  }
  
  // Start clock 
  clock_gettime(CLOCK_MONOTONIC, &ts_begin);

  // Create Struct
  struct sortingParams oneSide;
  oneSide.location = firstHalf;
  oneSide.size = n/2;

  struct sortingParams twoSide;
  twoSide.location = secondHalf;
  twoSide.size = n - n/2;

  // Create threads 1 + 2
  pthread_create(&thread1, NULL, sort, &oneSide);
  pthread_create(&thread2, NULL, sort, &twoSide);

  // Join thread 1 + 2
  pthread_join(thread1, NULL);
  pthread_join(thread2, NULL);
  
  // Create Merging Struct
  struct mergingParams merg;
  merg.first = firstHalf;
  merg.firstsize = n/2;
  merg.second = secondHalf;
  merg.secondsize = n - n/2;
  merg.dest = C;

  // Create thread to run merge
  pthread_create(&threadM, NULL, mergeThreads, &merg);

  //Join Thread
  pthread_join(threadM, NULL);

  // End clock
  clock_gettime(CLOCK_MONOTONIC, &ts_end); 
  elapsed = ts_end.tv_sec - ts_begin.tv_sec; 
	elapsed += (ts_end.tv_nsec - ts_begin.tv_nsec) /       1000000000.0;
  
  // Print Elapsed time
  printf("\nSorting by Two threads is done in: %f ms", elapsed*1000);

  return 0;
}
