#include <stdio.h>
#include <stdlib.h>

// WRITING INTEGERS TO A BINARY FILE.
/*
Key Function: fwrite()
    size_t fwrite(const void* ptr, size_t size, size_t count, FILE* stream);
    Returns: Number of elements successfully written.
*/
int main()
{
    FILE *file = fopen("data.bin", "wb"); // Open a binary file for writing
    if (file == NULL)
    {
        printf("Error opening file!\n");
        return 1;
    }

    int numbers[] = {10, 20, 30, 40, 50}; // Integers to be written to file

    size_t count = sizeof(numbers) / sizeof(numbers[0]); // Number of elements

    // Write the array to the binary file
    size_t written = fwrite(numbers, sizeof(int), count, file);

    if (written == count)
    {
        printf("%zu Integers successfully written to file!\n", written);
    }
    else
    {
        printf("Error writing integers to file!\n");
    }

    fclose(file); // Close the file
    return 0;
}
