#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

/*
Exercise 3: Summing Elements in an Allocated Array
Write a program that:

1. Prompts the user for the number of integers to store.
2. Dynamically allocates memory for that many integers.
3. Asks the user to input the integers one by one.
4. Calculates and prints the sum of the integers.
5. Frees the allocated memory.

*/

int main()
{
    // Prompt the user for the number of integers to store.
    int numberOfIntegers;
    printf("Enter the number of integers to be summed up:\n");
    printf("=============================================\n");
    scanf("%d", &numberOfIntegers);
    
    // if (isdigit(numberOfIntegers))
    // {
    //     printf("Invalid input! Enter a number!");
    // }
    

    // Dynamically allocate memory for that many integers.
    int *integers = (int *)malloc(numberOfIntegers * sizeof(int));
    
    // Check for successful allocation.
    if (integers == NULL)
    {
        printf("Memory Allocation Failed!");
        return 1; // Exit if malloc fails.
    }

    // Ask the user to input the integers one by one.
    printf("Enter the integers one by one:\n");
    printf("===============================\n");


    for (int i = 0; i < numberOfIntegers; i++)
    {
        scanf("%d", &integers[i]);
    }
    
    // Calculate and print the sum of the integers.
    int sum = 0;
    
    for (int i = 0; i < numberOfIntegers; i++)
    {
        sum += integers[i];
    }
    printf("===============================\n");
    printf("Total sum of the integers: %d\n", sum);
    printf("===============================\n");

    // Free the allocated memory.
    free(integers);    // Always free the allocated memory.

    return 0;
}