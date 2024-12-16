#include <stdio.h>
#include <stdlib.h>

/*
Write a program to:
Create a binary file called numbers.bin.
Write an array of 8 integers to the file.
*/

int main()
{

    FILE *file = fopen("numbers.bin", "wb"); // Open binary file for writing
    if (file == NULL)
    {
        printf("Error opening file!\n");
        return 1;
    }

    int numbers[] = {1, 2, 3, 4, 5, 6, 7, 8}; // An array of 8 integers

    size_t count = sizeof(numbers) / sizeof(numbers[0]); // Number of elements in the array

    // Write integers to the binary file.
    size_t written = fwrite(numbers, sizeof(int), count, file);

    if (written == count)
    {
        printf("Successfully wrote %zu integers to file!\n", written);
    }
    else
    {
        printf("Error writing data to file!\n");
    }

    fclose(file); // Close the file
    return 0;
}