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

    // Ask the user for numbers and keep adding them to the array, resizing it dynamically with realloc as needed.
    printf("Enter multiple numbers(enter a negative number to stop.):\n");
    printf("=========================================================\n");
    int i = 0;
    while (&numbers[i] > 0)
    {
        scanf("%d", &numbers[i]);
        i++;
    }

    return 0;
}