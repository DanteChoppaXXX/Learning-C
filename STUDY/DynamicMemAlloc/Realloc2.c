#include <stdio.h>
#include <stdlib.h>

/*
Exercise: Dynamically Adjust Array Size

Start with an empty array (NULL pointer).
Ask the user for numbers and keep adding them to the array, resizing it dynamically with realloc as needed.
Stop when the user enters a negative number.
Print the array contents and free the memory.

*/

int main()
{

    // Start with an empty array (NULL pointer).
    int *numbers = NULL;

    // Allocate memory to store the numbers in the array.
    int size = 1;
    numbers = (int *)calloc(size, sizeof(int));

    // Check for successful allocation.
    if (numbers == NULL)
    {
        printf("Memory Allocation Failed!");
        return 1; // Exit if malloc fails.
    }

    // Ask the user for numbers and keep adding them to the array, resizing it dynamically with realloc as needed.
    printf("Enter multiple numbers(enter a negative number to stop.):\n");
    printf("=========================================================\n");
    int i = 0;
    while (scanf("%d", &numbers[i]))
    {
        // Stop when the user enters a negative number.
        if (numbers[i] < 0)
        {
            break;
        }

        // Resize the array.
        size++;
        numbers = (int *)realloc(numbers, size * sizeof(int));

        // Check for successful reallocation.
        if (numbers == NULL)
        {
            printf("Memory Reallocation Failed!");
            return 1; // Exit if malloc fails.
        }
        i++;
    }

    // Print the array contents and free the memory.
    printf("\nArray Contents:\n");
    printf("===============\n");
    for (int i = 0; i < size - 1; i++)
    {
        printf("%d\n", numbers[i]);
    }

    // Free the memory.
    free(numbers); // Always the free the memory after use.

    return 0;
}