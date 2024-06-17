#include "device/device.h"
#include <device/serial.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <unistd.h>

DeviceInterface serial_func = {
    .read = serial_read,
    .write = serial_write,
    .update = NULL,
    .check_intr = NULL,
};

static void ResetKeyboardInput() {
    struct termios term;
    tcgetattr(0, &term);
    term.c_lflag |= ICANON | ECHO;
    tcsetattr(0, TCSANOW, &term);
}

// 捕获键盘输入
static void CaptureKeyboardInput() {
    // 当退出时，取消捕获键盘输入
    atexit(ResetKeyboardInput);

    struct termios term;
    tcgetattr(0, &term);
    term.c_lflag &= ~(ICANON | ECHO); // Disable echo as well
    tcsetattr(0, TCSANOW, &term);
}

static int is_eofd;

static int ReadKBByte() {
    if (is_eofd)
        return 0xffffffff;
    char rxchar = 0;
    int rread = read(fileno(stdin), (char *)&rxchar, 1);

    if (rread > 0) // Tricky: getchar can't be used with arrow keys.
        return rxchar;
    else
        return -1;
}

static int IsKBHit() {
    if (is_eofd)
        return -1;
    int byteswaiting;
    ioctl(0, FIONREAD, &byteswaiting);
    if (!byteswaiting && write(fileno(stdin), 0, 0) != 0) {
        is_eofd = 1;
        return -1;
    } // Is end-of-file for
    return !!byteswaiting;
}

Serial *serial_init() {
    CaptureKeyboardInput();
    return NULL;
}

DeviceAccessStatus serial_read(void *device, uint64_t addr, uint8_t size,
                               uint64_t *data) {
    if (addr == 0x5) {
        *data = 0x60 | IsKBHit();
    } else if (addr == 0x0 && IsKBHit()) {
        *data = ReadKBByte();
    } else {
        *data = 0x0;
    }
    return DEVICE_ACCESS_OK;
}

DeviceAccessStatus serial_write(void *device, uint64_t addr, uint8_t size,
                                uint64_t data) {
    if (addr == 0) {
        printf("%c", (uint8_t)data);
        fflush(stdout);
    } else {
        return DEVICE_ACCESS_ERROR;
    }
    return DEVICE_ACCESS_OK;
}
