#include <stdio.h>
#include <stdlib.h>

// Datatypes Structure.
typedef struct
{
  int digit;
  char letter;
  float decimal;

} Datatypes;

int main()
{
  // Declare a struct object and assign values to the struct members.
  Datatypes datatypes = {419, 'X', 99.99};

  // Display the values to the terminal.
  printf("Integer Value: %d\nChar Value: %c\nFloat Value: %.2f\n", datatypes.digit, datatypes.letter, datatypes.decimal);

  return EXIT_SUCCESS;
}
