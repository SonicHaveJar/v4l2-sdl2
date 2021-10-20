#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <fcntl.h>
#include <unistd.h>

#include <sys/time.h>
#include <sys/mman.h>
#include <sys/ioctl.h>

#include <linux/videodev2.h>

int new_descriptor(const char* device);

void check_capabilities(const int fd);

struct v4l2_format set_format(const int fd, const int pixelformat, const int width, const int height);

void request_buffer(const int fd);

void *allocate_buffer(const int fd);

struct v4l2_buffer prepare_buffer(const int fd);

// Activate the device's streaming capaibility
int streamon(const int fd, struct v4l2_buffer bufferinfo);

void streamoff(const int fd, const int type);

void shot(const int fd, struct v4l2_buffer *bufferinfo);

void jpeg(const char *filename, const void *buffer_start, const int length);
