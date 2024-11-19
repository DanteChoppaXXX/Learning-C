#include <stdio.h>
#include <stdlib.h>

int main()
{

    // void* malloc(size_t, size);

    // Allocate Memory for an integer using malloc().
    int *number = (int *)malloc(sizeof(int));
    // Check for successful allocation.
    if (number == NULL)
    {
        printf("Memory Allocation Failed!");
        return 1; // Exit if malloc fails.
    }

    *number = 419; // Store an integer value at the allocated memory.

    printf("Address of the memory block: %p\n", number);
    printf("Value stored at allocated memory: %d\n", *number);

    free(number); // Always free the allocated memory after use.

    // Allocate Memory for an array using malloc().
    int numberOfElements = 5;
    int *array = (int *)malloc(numberOfElements * sizeof(int));
    // Check for successful allocation.
    if (array == NULL)
    {
        printf("Memory Allocation Failed!");
        return 1; // Exit if malloc fails.
    }

    // Assign values to the array.
    for (int i = 0; i < numberOfElements; i++)
    {
        array[i] = i * 5;
    }

    // Display the elements of the array.

    for (int i = 0; i < numberOfElements; i++)
    {
        printf("%d\n", array[i]);
    }

    free(array); // Always free the allocated memory after use.

    // Try Allocating a very large block.
    int *ptr = (int *)malloc(10000000000 * sizeof(int));
    if (ptr == NULL)
    {
        printf("Memory allocation failed! Not enough memory.\n");
    }
    else
    {
        printf("Memory allocation succeeded!\n");
        free(ptr); // Free memory only if allocation succeeds
    }

    return 0;
}