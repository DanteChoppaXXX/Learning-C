#include <stdio.h>
#include <stdlib.h>

// WRITING AND READING DATA IN CHUNKS.

/*
Write a file in chunks:

Create an array of 2000 floats (e.g., 1.0, 2.0, ...) and write it to a binary file in chunks of 250 floats each.
Read a file in chunks:

Read the binary file created in step 1 and calculate the sum of all floats in the file.
Challenge:

Modify the chunk size dynamically to process smaller chunks if memory usage exceeds a limit (e.g., 100 KB).

*/

void writeData()
{

    FILE *file = fopen("floats.bin", "wb"); // Open binary file for writing in chunks
    if (file == NULL)
    {
        printf("Error opening file for writing!\n");
        exit(1);
    }

    float numbers[2000]; // Array of floats
    // Populate the array with floats.
    for (int i = 0; i < 2000; i++)
    {
        numbers[i] = i + 1.0;
    }

    size_t chunk_size = 250;
    size_t total_numbers = sizeof(numbers) / sizeof(numbers[0]);
    size_t written = 0;

    // Write the floats to the file in chunks.
    while (written < total_numbers)
    {
        size_t remaining = total_numbers - written;
        size_t current_chunk = remaining < chunk_size ? remaining : chunk_size;

        fwrite(numbers + written, sizeof(float), current_chunk, file);
        written += current_chunk;

        printf(" Written %zu floats so far...\n", written);
    }

    fclose(file); // Close the file
    printf("File written successfully in chunks!\n");
    printf("File closed successfully!\n");
}

void readData()
{
    FILE *file = fopen("floats.bin", "rb"); // Open binary file for reading in chunks
    if (file == NULL)
    {
        printf("Error opening file for reading!\n");
        exit(1);
    }

    float buffer[250]; // read 250 floats at a time
    size_t chunk_size = sizeof(buffer) / sizeof(buffer[0]);
    size_t read = 0;
    size_t count = 0;
    float sum = 0;

    while ((read = fread(buffer, sizeof(float), chunk_size, file)) > 0)
    {
        for (size_t i = 0; i < read; i++)
        {
            printf("| %.1f |", buffer[i]);
            count++;
            if (count == 20)
            {
                printf("\n");
                count = 0;
            }
        }
        printf("--- End of Chunk ---\n\n");
        for (size_t i = 0; i < read; i++)
        {
            sum += buffer[i];
        }
    }
    printf("Sum = %.1f\n", sum);

    fclose(file); // Close the file
    printf("File read successfully in chunks!\n");
    printf("File closed successfully!\n");
}

int main()
{
    writeData();
    printf("\nCOUNTING TO 2000 IN CHUNKS OF 250\n");
    printf("==================================\n");
    readData();

    return 0;
}