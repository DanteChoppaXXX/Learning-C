#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void print_prompt(const char* user_name) {
    printf("%s: ", user_name);  // Print the label "User1: "
    fflush(stdout);  // Make sure the prompt is displayed before waiting for input
}

int main() {
    char input[100];
    
    // Name of the user
    const char* user_name = "User1";
    
    while (1) {
        // Display prompt
        print_prompt(user_name);

        // Read user input
        if (fgets(input, sizeof(input), stdin) == NULL) {
            break;
        }
        
        // Remove trailing newline
        input[strcspn(input, "\n")] = 0;
        
        // Example of clearing the line where the user will input and writing a message
        if (strcmp(input, "clear") == 0) {
            printf("\033[1K");  // Clear the entire current line
            printf("\033[1A");  // Move the cursor up by one line (clear prompt line)
            print_prompt(user_name);  // Redraw the prompt
        } else {
            // Message overwrite example
            //printf("\033[1K");  // Clear the entire current line
            printf("\033[1A");  // Move the cursor up by one line (clear prompt line)
            print_prompt(user_name);  // Redraw the prompt
            printf("%s\n", input);
        }
    }
    
    return 0;
}
