#include <stdio.h>
#include <stdlib.h>

//  READING AND WRITING STRUCTS TO BINARY FILE.

/*
Write a function to:

Create an array of 5 books.
Write the array of books to a binary file called books.bin.

Write another function to:

Read the book records from books.bin into a buffer.
Print the details of each book.
*/

struct Book
{
    int id;
    char title[50];
    float price;
};

char filename[] = "books.bin"; // Binary file to store books details

void writeStruct()
{

    FILE *file = fopen(filename, "wb"); // Open binary file for writing
    if (file == NULL)
    {
        printf("Error opening file!\n");
        exit(1);
    }

    struct Book books[] = {
        {1, "48 Laws Of Power", 55.00},
        {2, "33 Strategies Of War", 60.00},
        {3, "The Art Of Seduction", 50.55}};

    // Calculate the number of books
    size_t count = sizeof(books) / sizeof(books[0]); // Number of books

    // Write book details to the binary file.
    size_t written = fwrite(books, sizeof(struct Book), count, file);

    if (written == count)
    {
        printf("Successfully wrote details of %zu books to the file!\n", written);
    }
    else
    {
        printf("Error writing data to file!\n");
    }

    fclose(file); // Close the file
    printf("File closed successfully!\n");
}

void readStruct()
{
    FILE *file = fopen(filename, "rb"); // Open binary file for reading
    if (file == NULL)
    {
        printf("Error opening file!\n");
        exit(1);
    }

    struct Book buffer[3]; // Buffer to store the book details

    // Calculate the number of books
    size_t count = sizeof(buffer) / sizeof(buffer[0]); // Number of books

    // Read the books details from the buffer.
    size_t read = fread(buffer, sizeof(struct Book), count, file);

    if (read > 0)
    {
        printf("Successfully read details of %zu books to the file!\n", read);

        // Display books details.
        printf("\nBooks Details:\n");
        printf("==============\n");
        printf("%-5s %-30s %10s\n", "ID", "Title", "Price($)");
        printf("===============================================\n");
        for (size_t i = 0; i < read; i++)
        {
            printf("%-5d %-30s %9.2f\n", buffer[i].id, buffer[i].title, buffer[i].price);
        }
    }
    else
    {
        printf("No data read from the file!\n");
    }

    fclose(file); // Close the file
    printf("File closed successfully!\n");
}

int main()
{

    char message[] = "WELCOME TO THE BOOK STORE!\n";

    writeStruct();

    printf("\n%s", message);
    printf("===========================\n");

    readStruct();

    return 0;
}