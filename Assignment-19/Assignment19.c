#include <stdio.h>
#include <stdlib.h>
#include <strings.h>

int main(){

    char *ptr;
    
    ptr = malloc(10 * sizeof(char));
    if (ptr == NULL)
    {
        printf("Memory Allocation Failed!");
    }

    printf("Memory Allocation Successful!");

    free(ptr);
    


    return 0;
}