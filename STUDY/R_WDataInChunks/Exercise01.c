#include <stdio.h>
#include <stdlib.h>

/*
1. Write a program to open a file called data.txt for reading. If the file doesn’t exist, print an error message.

2. Open another file called output.txt for writing and close it immediately.
*/

int main()
{
    FILE *file, *file1;
    char filename[256] = "data.txt";

    file = fopen(filename, "r"); // Open the file for reading.
    if (file == NULL)
    {
        printf("The file '%s' does not exist!\n", filename);
        // return 1;
    }

    file1 = fopen("output.txt", "w"); // Open the file for writing.
    if (file1 == NULL)
    {
        printf("Error opening file!\n");
        return 1;
    }
    printf("File opened successfully!\n");

    fclose(file1); // Close the file.
    printf("File closed successfully!\n");

    return 0;
}
