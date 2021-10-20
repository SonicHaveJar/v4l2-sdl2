// gcc -I . webcam.c cheese.c -o cheese

#include "webcam.h"
#include <time.h>

int main() {
  int fd = new_descriptor("/dev/video0");

  check_capabilities(fd);

  // v4l2-ctl -d /dev/video0 --list-formats-ext
  struct v4l2_format format = set_format(fd, V4L2_PIX_FMT_MJPEG, 640, 480); // 300 KiB

  // Inform the device about our future buffer
  request_buffer(fd);
  
  void *buffer_start = allocate_buffer(fd);
  
  struct v4l2_buffer bufferinfo = prepare_buffer(fd);

  int type = streamon(fd, bufferinfo);

  shot(fd, &bufferinfo);
  
  streamoff(fd, type);

  char name[64];
  sprintf(name, "%ld.jpg", time(NULL));
  jpeg(name, buffer_start, bufferinfo.length);

  close(fd);
  return EXIT_SUCCESS;
}
