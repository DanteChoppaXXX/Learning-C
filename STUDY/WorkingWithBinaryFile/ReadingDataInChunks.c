#include <stdio.h>
#include <stdlib.h>

// Read integers from a binary file in chunks.

int main()
{

    FILE *file = fopen("numbers_chunked.bin", "rb"); // Open binary file for reading in chunk
    if (file == NULL)
    {
        printf("Error opening file for reading!\n");
        return 1;
    }

    int buffer[100]; // read 100 integers at a time
    size_t chunk_size = sizeof(buffer) / sizeof(buffer[0]);
    size_t read = 0;
    // Display the integers read.
    printf("\nIntegers Read:\n");
    printf("==============\n");

    while ((read = fread(buffer, sizeof(int), chunk_size, file)) > 0)
    {

        for (size_t i = 0; i < read; i++)
        {
            printf("%d\n", buffer[i]);
        }

        printf("--- End of Chunk ---\n\n");
    }

    fclose(file); // Close the file
    printf("File read successfully in chunks!\n");
    printf("File closed successfully!\n");
    return 0;
}