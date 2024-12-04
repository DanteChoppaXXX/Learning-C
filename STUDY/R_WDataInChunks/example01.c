#include <stdio.h>
#include <stdlib.h>

int main()
{
    FILE *file = fopen("data.bin", "rb"); // Open a binary file for reading
    if (file == NULL)
    {
        printf("Error opening file!\n");
        return 1;
    }

    int buffer[3]; // Buffer to store chunks of 3 integers
    size_t elements_read;

    printf("Reading data in chunks:\n");

    // Read chunks of data
    while ((elements_read = fread(buffer, sizeof(int), 3, file)) > 0)
    {
        for (size_t i = 0; i < elements_read; i++)
        {
            printf("%d ", buffer[i]); // Print each integer
        }
        printf("\n");
    }

    fclose(file); // Close the file
    return 0;
}
