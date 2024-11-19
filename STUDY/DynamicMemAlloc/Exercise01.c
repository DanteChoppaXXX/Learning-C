#include <stdio.h>
#include <stdlib.h>

/*
Exercise 1: Allocating Memory for One Integer
Write a program that:

1. Allocates memory for one integer using malloc.
2. Stores a number (inputted by the user) in the allocated memory.
3. Prints the stored value.
4. Frees the allocated memory.

*/

int main()
{
    // Allocate memory for one integer using malloc().
    int *number = (int *)malloc(sizeof(int));

    // Check for successful allocation.
    if (number == NULL)
    {
        printf("Memory Allocation Failed!");
        return 1; // Exit if malloc fails.
    }

    // Prompt user to enter a number.
    printf("Enter Any Number:\n");
    printf("=================\n");
    scanf("%d", number);

    // Display the stored value.
    printf("The number entered is: %d\n", *number);
    printf("============================\n");

    // Free the allocated memory.
    free(number); // Always free the allocated memory after use.

    return 0;
}