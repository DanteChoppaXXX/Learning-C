#include <stdio.h>
#include <stdlib.h>

// READING STRUCTS TO A BINARY FILE

struct Student
{
    int id;
    char name[50];
    float grade;
};

int main()
{

    FILE *file = fopen("students.bin", "rb"); // Open binary file for reading
    if (file == NULL)
    {
        printf("Error opening file!\n");
        return 1;
    }

    struct Student buffer[3]; // Buffer to store the structs

    size_t count = sizeof(buffer) / sizeof(buffer[0]); // Number of students

    // Read the array of structs from the binary file.
    size_t read = fread(buffer, sizeof(struct Student), count, file);

    if (read > 0)
    {
        printf("Successfully read %zu student records from the file!\n", read);

        for (size_t i = 0; i < read; i++)
        {
            // printf("%d\n", buffer[i]);
            printf("%d, %s, %.1f\n", buffer[i].id, buffer[i].name, buffer[i].grade);
        }
    }
    else
    {
        printf("Error reading student records from file!\n");
    }

    fclose(file); // Close the file
    printf("File closed successfully!\n");

    return 0;
}