#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <linux/input.h>

int main() {
    // int fd;
    // struct input_event ie;

    // fd = open("/dev/input/event0", O_RDONLY);
    // if (fd == -1) {
    //     return 1;
    // }

    // while (1) {
    //     read(fd, &ie, sizeof(struct input_event));
    //     printf("time %ld.%06ld\ttype %d\tcode %d\tvalue %d\n", ie.time.tv_sec, ie.time.tv_usec, ie.type, ie.code, ie.value);
    // }

    return 0;
}