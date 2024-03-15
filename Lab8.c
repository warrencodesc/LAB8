#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// global variable to keep track of extra memory allocated
int extraMemoryAllocated;

// the memory allocation function
void *Alloc(size_t sz)
{
    // increment the extraMemoryAllocated counter
    extraMemoryAllocated += sz;
    // Allocate memory using malloc and store the size before the pointer
    size_t* ret = malloc(sizeof(size_t) + sz);
    *ret = sz;
    printf("Extra memory allocated, size: %ld\n", sz);
    // Return a pointer to the memory block after the size
    return &ret[1];
}

// the memory deallocation function
void DeAlloc(void* ptr)
{
    // retrieve the size of the allocated memory block
    size_t* pSz = (size_t*)ptr - 1;
    // Decrement the extraMemoryAllocated counter
    extraMemoryAllocated -= *pSz;
    printf("Extra memory deallocated, size: %ld\n", *pSz);
    // Free the memory block using free
    free((size_t*)ptr - 1);
}

// Function to get the size of an allocated memory block
size_t Size(void* ptr)
{
    // Extract the size stored before the pointer and return it
    return ((size_t*)ptr)[-1];
}

// merge function to merge two sorted subarrays
void merge(int pData[], int l, int m, int r)
{
    int i, j, k;
    int n1 = m - l + 1;
    int n2 = r - m;

    // allocate memory for temporary arrays L[] and R[]
    int *L = (int *)Alloc(n1 * sizeof(int));
    int *R = (int *)Alloc(n2 * sizeof(int));

    // Copy data to temporary arrays L[] and R[]
    for (i = 0; i < n1; i++)
        L[i] = pData[l + i];
    for (j = 0; j < n2; j++)
        R[j] = pData[m + 1 + j];

    // merge the temporary arrays back into pData[l..r]
    i = 0; // Initial index of first subarray
    j = 0; // Initial index of second subarray
    k = l; // Initial index of merged subarray
    while (i < n1 && j < n2) {
        if (L[i] <= R[j]) {
            pData[k] = L[i];
            i++;
        } else {
            pData[k] = R[j];
            j++;
        }
        k++;
    }

    // Copy the remaining elements of L[], if any
    while (i < n1) {
        pData[k] = L[i];
        i++;
        k++;
    }

    // Copy the remaining elements of R[], if any
    while (j < n2) {
        pData[k] = R[j];
        j++;
        k++;
    }

    // dealocate memory for temporary arrays
    DeAlloc(L);
    DeAlloc(R);
}

// the main function to perform merge sort
void mergeSort(int pData[], int l, int r)
{
    if (l < r) {
        // same as (l+r)/2, but avoids overflow for large l and r
        int m = l + (r - l) / 2;

        // sort first and second halves
        mergeSort(pData, l, m);
        mergeSort(pData, m + 1, r);

        // mrge the sorted halves
        merge(pData, l, m, r);
    }
}

// parses input file to an integer array
int parseData(char *inputFileName, int **ppData)
{
    FILE* inFile = fopen(inputFileName,"r");
    int dataSz = 0;
    int i, n, *data;
    *ppData = NULL;

    if (inFile)
    {
        // Read the size of the array
        fscanf(inFile,"%d\n",&dataSz);
        // allocate memory for the array
        *ppData = (int *)Alloc(sizeof(int) * dataSz);
        // implement parse data block
        if (*ppData == NULL)
        {
            printf("Cannot allocate memory\n");
            exit(-1);
        }
        // read data from file and store in the array
        for (i=0;i<dataSz;++i)
        {
            fscanf(inFile, "%d ",&n);
            data = *ppData + i;
            *data = n;
        }

        fclose(inFile);
    }

    return dataSz;
}

// prints first and last 100 items in the data array
void printArray(int pData[], int dataSz)
{
    int i, sz = dataSz - 100;
    printf("\tData:\n\t");
    // Print first 100 elements
    for (i=0;i<100;++i)
    {
        printf("%d ",pData[i]);
    }
    printf("\n\t");

    // Print last 100 elements
    for (i=sz;i<dataSz;++i)
    {
        printf("%d ",pData[i]);
    }
    printf("\n\n");
}

int main(void)
{
    clock_t start, end;
    int i;
    double cpu_time_used;
    // array of input file names
    char* fileNames[] = { "input1.txt", "input2.txt", "input3.txt", "input4.txt" };

    for (i=0;i<4;++i)
    {
        int *pDataSrc, *pDataCopy;
        // parse data from file
        int dataSz = parseData(fileNames[i], &pDataSrc);

        if (dataSz <= 0)
            continue;

        // Allocate memory for copy of data
        pDataCopy = (int *)Alloc(sizeof(int)*dataSz);

        printf("---------------------------\n");
        printf("Dataset Size : %d\n",dataSz);
        printf("---------------------------\n");

        printf("Merge Sort:\n");
        // copy data for sorting
        memcpy(pDataCopy, pDataSrc, dataSz*sizeof(int));
        // Reset memory allocation counter
        extraMemoryAllocated = 0;
        // Start timing
        start = clock();
        // perfrm merge sort
        mergeSort(pDataCopy, 0, dataSz - 1);
        // Stop timing
        end = clock();
        // calculate runtime
        cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
        // print runtime and memory allocation details
        printf("\truntime\t\t\t: %.1lf\n",cpu_time_used);
        printf("\textra memory allocated\t: %d\n",extraMemoryAllocated);
        // Print sorted array
        printArray(pDataCopy, dataSz);

        // deallocate memory
        DeAlloc(pDataCopy);
        DeAlloc(pDataSrc);
    }

    return 0;
}