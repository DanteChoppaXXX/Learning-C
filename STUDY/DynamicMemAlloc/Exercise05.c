#include <stdio.h>
#include <stdlib.h>

/*
Extra Challenge (Optional): Dynamic String Storage
Write a program that:

1. Prompts the user to input a string length.
2. Allocates memory for a string of that length using malloc.
3. Reads the string from the user and prints it.
4. Frees the allocated memory.

*/

int main()
{
    // Prompt the user to input a string length.
    int lengthOfString;
    printf("Enter the length of any word:\n");
    printf("==============================\n");
    scanf("%d", &lengthOfString);

    // Allocate memory for a string of that length using malloc.
    char *string = (char *)malloc(lengthOfString * sizeof(char));

    // Check for successful allocation.
    if (string == NULL)
    {
        printf("Memory Allocation Failed!");
        return 1; // Exit if malloc fails.
    }

    // Read the string from the user and print it.
    printf("Enter the word:\n");
    printf("===============\n");
    scanf("%s", string);

    printf("The word is: %s\n", string);

    // Free the allocated memory.
    free(string);    // Always free the allocated memory.

    return 0;
}