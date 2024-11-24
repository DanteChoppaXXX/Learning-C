#include <stdio.h>
#include <stdlib.h>

/*
Exercise 1: Array Allocation with calloc
Prompt the user for the number of integers they want to store.
Use calloc to allocate memory for the integers.
Print the values (verify they are all zero-initialized).
Free the memory.
*/

int main()
{
    // Prompt the user for the number of integers they want to store.
    int numOfIntegers;
    printf("How many integers do you want to store?\n");
    printf("=======================================\n");
    scanf("%d", &numOfIntegers);

    // Use calloc to allocate memory for the integers.
    int *integers = (int *)calloc(numOfIntegers, sizeof(int));

    // Check for successful allocation.
    if (integers == NULL)
    {
        printf("Memory Allocation Failed!");
        return 1; // Exit if malloc fails.
    }

    // Print the values (verify they are all zero-initialized).
    for (int i = 0; i < numOfIntegers; i++)
    {
        printf("%d\n", integers[i]);
    }

    // Free the allocated memory.
    free(integers); // Always free the allocated memory.

    printf("=======================================\n");

    // ARRAY OF FLOATS.

    int n = 5;
    // Use calloc to allocate memory for an array of 5 floats.
    float *prices = (float *)calloc(n, sizeof(float));

    // Check for successful allocation.
    if (prices == NULL)
    {
        printf("Memory Allocation Failed!");
        return 1; // Exit if malloc fails.
    }

    // Print the values and verify they are initialized to zero.
    for (int i = 0; i < n; i++)
    {
        printf("%.2f\n", prices[i]);
    }

    // Assign values to the array.
    for (int i = 0; i < n; i++)
    {
        prices[i] = i * 5;
    }

    printf("=======================================\n");

    // Print the updated values.
    for (int i = 0; i < n; i++)
    {
        printf("$%.2f\n", prices[i]);
    }

    // Free the allocated memory.
    free(prices); // Always free the allocated memory.

    // DYNAMIC MEMORY FOR STRINGS.

    int stringLength;
    // Prompt the user to enter the number of characters for a string.
    printf("Enter the length of any word:\n");
    printf("==============================\n");
    scanf("%d", &stringLength);

    // Use calloc to allocate memory for the string (including space for the null terminator).
    char *string = (char *)calloc(stringLength + 1, sizeof(char));

    // Check for successful allocation.
    if (string == NULL)
    {
        printf("Memory Allocation Failed!");
        return 1; // Exit if malloc fails.
    }

    for (int i = 0; i < stringLength; i++)
    {
        /* code */
        printf("%d\n", string[i]);
    }

    // Read a string from the user, then print it.
    printf("Enter the word:\n");
    printf("===============\n");
    scanf("%s", string);

    printf("The word is: %s\n", string);

    // Free the allocated memory.
    free(string); // Always free the allocated memory.

    return 0;
}