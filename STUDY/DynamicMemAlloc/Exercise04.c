#include <stdio.h>
#include <stdlib.h>

/*
Exercise 4: Detecting Memory Allocation Failure
Write a program that:

1. Tries to allocate a very large block of memory using malloc (e.g., 1000000000 * sizeof(int)).
2. Checks if the allocation was successful.
3. If successful, prints a message and frees the memory.
4. If not, prints an error message.

*/

int main()
{
    // Tries to allocate a very large block of memory using malloc (e.g., 1000000000 * sizeof(int)).
    int *ptr = (int *)malloc(10000000000 * sizeof(int));

    // Check for successful allocation.
    if (ptr == NULL)
    {
        printf("Memory Allocation Failed! Not Enough Memory!\n");
        return 1; // Exit if malloc fails.
    }
    else{
        printf("Memory Allocation Successful!\n");
        free(ptr);  // Always free the allocated memory after use.
    }

    return 0;
}