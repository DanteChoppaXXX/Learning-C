#include <stdio.h>
#include <stdlib.h>

// WRITING STRUCTS TO A BINARY FILE

struct Student
{
    int id;
    char name[50];
    float grade;
};

int main()
{

    FILE *file = fopen("students.bin", "wb"); // Open binary file for writing
    if (file == NULL)
    {
        printf("Error opening file!\n");
        return 1;
    }

    // Create some student records
    struct Student students[] = {
        {1, "John", 94.5},
        {2, "Jack", 89.4},
        {3, "Jane", 90.2}};

    size_t count = sizeof(students) / sizeof(students[0]); // Number of students

    // Write the array of structs to the binary file.
    size_t written = fwrite(students, sizeof(struct Student), count, file);

    if (written == count)
    {
        printf("Successfully wrote %zu student records to the file!\n", written);
    }
    else
    {
        printf("Error writing student records to file!\n");
    }

    fclose(file); // Close the file
    printf("File closed successfully!\n");

    return 0;
}