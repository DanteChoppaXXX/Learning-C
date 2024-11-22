#include <stdio.h>
#include <stdlib.h>

/*
Exercises for realloc
Exercise 1: Expand an Array
Allocate memory for an array of 3 integers.
Initialize the array with values 1, 2, 3.
Reallocate the array to store 6 integers.
Initialize the new elements with values 4, 5, 6.
Print the entire array and free the memory.
*/

int main()
{

    int size = 3;
    // Allocate memory for an array of 3 integers.
    int *integers = (int *)malloc(size * sizeof(int));
    // Check for successful allocation.
    if (integers == NULL)
    {
        printf("Memory Allocation Failed!");
        return 1; // Exit if malloc fails.
    }

    // Initialize the array with values 1, 2, 3.
    for (int i = 0; i < size; i++)
    {
        integers[i] = i + 1;
    }
    // Display the values of the array.
    for (int i = 0; i < size; i++)
    {
        printf("%d\n", integers[i]);
    }

    // Reallocate the array to store 6 integers.
    size = 6;
    integers = (int *)realloc(integers, size * sizeof(int));
    // Check for successful reallocation.
    if (integers == NULL)
    {
        printf("Memory Reallocation Failed!");
        return 1; // Exit if malloc fails.
    }

    // Initialize the new elements with values 4, 5, 6.
    for (int i = 3; i < size; i++)
    {
        integers[i] = i + 1;
    }

    // Display the values of the entire array.
    for (int i = 3; i < size; i++)
    {
        printf("%d\n", integers[i]);
    }

    // Reallocate the array to store only the first 2 integers.
    size = 2;
    integers = (int *)realloc(integers, size * sizeof(int));
    // Check for successful reallocation.
    if (integers == NULL)
    {
        printf("Memory Reallocation Failed!");
        return 1; // Exit if malloc fails.
    }

    // Display the values of the entire array.
    for (int i = 0; i < size; i++)
    {
        printf("%d\n", integers[i]);
    }

    // Free the memory.
    free(integers); // Always the free the memory after use.
    return 0;
}