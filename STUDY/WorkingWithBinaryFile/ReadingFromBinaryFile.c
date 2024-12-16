#include <stdio.h>
#include <stdlib.h>

// READING INTEGERS FROM A BINARY FILE.
/*
Key Function: fread()
    size_t fread(const void* ptr, size_t size, size_t count, FILE* stream);
    Returns: Number of elements successfully read.
*/

int main()
{

    FILE *file = fopen("data.bin", "rb"); // Open binary file for reading
    if (file == NULL)
    {
        printf("Error opening file!\n");
        return 1;
    }

    int buffer[5];                                     // Buffer to store the integers
    size_t count = sizeof(buffer) / sizeof(buffer[0]); // Number of elements read

    // Read data from the binary file
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
        printf("No data read from file!\n");
    }

    fclose(file); // Close the file
    return 0;
}