#include <stdio.h>
#include <stdlib.h>

/*
Exercise 2: Allocating Memory for an Array
Write a program that:

1. Prompts the user to input the size of an array.
2. Allocates memory for the array using malloc.
3. Fills the array with numbers from 1 to size.
4. Prints the array elements.
5. Frees the allocated memory.

*/

int main()
{
    int sizeOfArray;
    // Prompt the user to input the size of an array.
    printf("Enter a size for an Array:\n");
    printf("==========================\n");
    scanf("%d", &sizeOfArray);

    // Allocate memory for the array using malloc.
    int *array = (int*)malloc(sizeOfArray * sizeof(int));
    
    // Check for successful allocation.
    if (array == NULL)
    {
        printf("Memory Allocation Failed!");
        return 1; // Exit if malloc fails.
    }

    // Fill the array with numbers from 1 to size.
    for (int i = 0; i < sizeOfArray; i++)
    {
        array[i] = i + 1;
    }
    
    // Display the elements stored in the array.
    printf("Elements stored in the Array:\n");
    printf("=============================\n");
    
    for (int i = 0; i < sizeOfArray; i++)
    {
        printf("%d\n", array[i]);
    }
    
    // Free the allocated memory.
    free(array);    // Always free the allocated memory.

    return 0;
}