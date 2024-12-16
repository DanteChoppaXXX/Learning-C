#include <stdio.h>
#include <stdlib.h>

/*
Write another program to:

Read the integers from numbers.bin into an array.
Print the integers to the console.
*/

int main()
{
    FILE *file = fopen("numbers.bin", "rb"); // Open binary file for reading
    if (file == NULL)
    {
        printf("Error opening file!\n");
        return 1;
    }

    int buffer[8]; // Buffer to store the integers

    // Calculate the number of integers in the array
    size_t count = sizeof(buffer) / sizeof(buffer[0]); // Number of elements in the array

    // Read data from the binary file.
    size_t read = fread(buffer, sizeof(int), count, file);

    if (read > 0)
    {
        printf("Successfully read %zu integers from file:\n", read);
        for (size_t i = 0; i < read; i++)
        {
            printf("%d\n", buffer[i]);
        }
    }
    else
    {
        printf("No data read from the file!\n");
    }

    fclose(file); // Close the file.
    printf("File closed successfully.\n");

    return 0;
}