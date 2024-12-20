#include <stdio.h>
#include <stdlib.h>

// Write an array of integers to a file in smaller pieces.

int main()
{

    FILE *file = fopen("numbers_chunked.bin", "wb"); // Open binary file for writing in chunks
    if (file == NULL)
    {
        printf("Error opening file for writing!\n");
        return 1;
    }

    int numbers[1000]; // Array to store the integers

    // Populate the array with numbers 1 to 1000.
    for (size_t i = 0; i < 1000; i++)
    {
        numbers[i] = i + 1;
    }

    size_t chunk_size = 100;                                     // Write 100 integers at a time
    size_t total_numbers = sizeof(numbers) / sizeof(numbers[0]); // Get the number of integers in the array
    size_t written = 0;

    // Write the integers in the array to the file in chunks.
    while (written < total_numbers)
    {
        size_t remaining = total_numbers - written;
        size_t current_chunk = remaining < chunk_size ? remaining : chunk_size; // Conditional or Ternary operator(?:)

        fwrite(numbers + written, sizeof(int), current_chunk, file);
        written += current_chunk;

        printf(" Written %zu integers so far...\n", written);
    }

    fclose(file); // Close the file
    printf("File written successfully in chunks!\n");
    printf("File closed successfully!\n");

    return 0;
}