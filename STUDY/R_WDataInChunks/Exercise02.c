#include <stdio.h>
#include <stdlib.h>

/*
1. Create a file called data.bin containing at least 10 integers (you can use any external tool or program to generate it).

2. Write a program that reads integers from data.bin in chunks of 4 and prints them to the console.

*/

int main()
{
    FILE *file;
    char filename[256] = "data.bin";

    file = fopen(filename, "wb"); // Open the file for reading.
    if (file == NULL)
    {
        printf("The file '%s' does not exist!\n", filename);
        // return 1;
    }
    printf("File opened successfully!\n");

    // Data to write
    int numbers[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20};
    size_t count = sizeof(numbers) / sizeof(numbers[0]);

    // Write the array to the file
    fwrite(numbers, sizeof(int), count, file);

    int buffer[4]; //  Buffer to store chunks of 4 integers.
    size_t elements_read;

    printf("Reading data in chunks:\n");

    // Read data in chunks.
    while ((elements_read = fread(buffer, sizeof(int), 4, file)) > 0)
    {
        for (size_t i = 0; i < elements_read; i++)
        {
            printf("%d ", buffer[i]); // Print each integer.
        }
        printf("\n");
    }

    fclose(file); // Close the file.

    return 0;
}