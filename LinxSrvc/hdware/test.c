#ifdef GPIO
#include "gpio.h"
#elif defined(ME9S)
#include "me909s.h"
#elif defined(PIPE_FIFO)
#include <stdlib.h>
#include "pipefifo.h"
#elif defined(VIDEO) || defined(SNAP)
#include "test.h"
#elif defined(DRIVER)
#include "stdio.h"
#include "fcntl.h"
#include "unistd.h"
#else
#error compile command unsupported
#endif

const int SizeOfBuf = 1024;

int main(int argc, char** argv)
{
#ifdef GPIO
    if (argc > 3) {
        set_gpio_by_direction(atoi(argv[1]), atoi(argv[2]), atoi(argv[3]));
    } else if (argc == 3) {
        set_gpio_export(atoi(argv[1]), atoi(argv[2]));
    } else if (argc == 2) {
        gpio_hint(get_gpio_value(atoi(argv[1])));
    } else {
        gpio_hint(-1);
    }
#elif defined(ME9S)
    meat_main();
#elif defined(PIPE_FIFO)
    if (argc >= 2) {
        pipe_fifo_test((long long)atoi(argv[1]), argv[2] == NULL ? -1 : atoi(argv[2]));
    } else {
        pipe_fifo_test(-1, 0);
    }
#elif defined(VIDEO)
    video_capture(argc, argv);
#elif defined(SNAP)
    snap_image_test("image", 640, 480);
#elif defined(DRIVER)
#define DEV_NODE "/dev/chars-node"
    int fd = open(DEV_NODE, O_RDWR, S_IRUSR | S_IWUSR);
    if (fd < 0) {
        perror("open ["DEV_NODE"] fail");
    } else {
        char msg[SizeOfBuf];
        ssize_t len = read(fd, msg, sizeof(msg));
        if (len > SizeOfBuf || len < 0) {
            perror("beyond read size");
            return -1;
        }
        msg[SizeOfBuf - 1] = '\0';
        printf("Default chars is [%s].\n", msg);
        printf("Please input a string written to chars device: ");
        scanf("%1023s", msg);
        write(fd, msg, sizeof(msg));
        len = read(fd, msg, sizeof(msg));
        if (len > SizeOfBuf || len < 0) {
            perror("beyond read size");
            return -1;
        }
        msg[SizeOfBuf - 1] = '\0';
        printf("Chars [%s] written to '%s'.\n", msg, DEV_NODE);
        close(fd);
    }
#endif
    return 0;
}
