#include "webcam.h"

int new_descriptor(const char* device) {
  int fd = open(device, O_RDWR);
  if (fd < 0) {
    perror("open");
    exit(1);
  }

  return fd;
}

void check_capabilities(const int fd) {
  struct v4l2_capability cap;
  if (ioctl(fd, VIDIOC_QUERYCAP, &cap) < 0) { // Papulates cap with device info
    perror("VIDIOC_QUERYCAP");
    exit(1);
  }

  // We're interested in these two capabilities:
  if (!(cap.capabilities & V4L2_CAP_VIDEO_CAPTURE)) {
    fprintf(stderr, "The device does not support video capture.\n");
    exit(1);
  }
 
  if (!(cap.capabilities & V4L2_CAP_STREAMING)) {
    fprintf(stderr, "The device does not support video streaming.\n");
    exit(1);
  }
}

struct v4l2_format set_format(const int fd, const int pixelformat, const int width, const int height) {
  struct v4l2_format format;

  format.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  format.fmt.pix.pixelformat = pixelformat;
  format.fmt.pix.width = width;
  format.fmt.pix.height = height;
 
  if (ioctl(fd, VIDIOC_S_FMT, &format) < 0) {
    perror("VIDIOC_S_FMT");
    exit(1);
  }

  return format;
}

void request_buffer(const int fd) {
  struct v4l2_requestbuffers bufrequest;

  bufrequest.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  bufrequest.memory = V4L2_MEMORY_MMAP;
  bufrequest.count = 1;
 
  if (ioctl(fd, VIDIOC_REQBUFS, &bufrequest) < 0) {
    perror("VIDIOC_REQBUFS");
    exit(1);
  }
}

void *allocate_buffer(const int fd) {
  // The device is telling us how many bytes it needs for the 
  // format and frame dimensions and stores it on the struct.

  struct v4l2_buffer bufferinfo;

  memset(&bufferinfo, 0, sizeof(bufferinfo));
 
  bufferinfo.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  bufferinfo.memory = V4L2_MEMORY_MMAP;
  bufferinfo.index = 0;
 
  if (ioctl(fd, VIDIOC_QUERYBUF, &bufferinfo) < 0) {
    perror("VIDIOC_QUERYBUF");
    exit(1);
  }

  // Map our memory
  void* buffer_start = mmap(
    NULL,
    bufferinfo.length,
    PROT_READ | PROT_WRITE,
    MAP_SHARED,
    fd,
    bufferinfo.m.offset
  );
 
  if (buffer_start == MAP_FAILED) {
    perror("mmap");
    exit(1);
  }
 
  memset(buffer_start, 0, bufferinfo.length);

  return buffer_start;
}

struct v4l2_buffer prepare_buffer(const int fd) {
  struct v4l2_buffer bufferinfo;
  
  memset(&bufferinfo, 0, sizeof(bufferinfo));
 
  bufferinfo.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  bufferinfo.memory = V4L2_MEMORY_MMAP;
  bufferinfo.index = 0;

  return bufferinfo;
}

// Activate the device's streaming capaibility
int streamon(const int fd, struct v4l2_buffer bufferinfo) {
  int type = bufferinfo.type;
  if (ioctl(fd, VIDIOC_STREAMON, &type) < 0) {
    perror("VIDIOC_STREAMON");
    exit(1);
  }

  return type;
}

void streamoff(const int fd, const int type) {
  if (ioctl(fd, VIDIOC_STREAMOFF, &type) < 0) {
    perror("VIDIOC_STREAMOFF");
    exit(1);
  }
}

void shot(const int fd, struct v4l2_buffer *bufferinfo) {
  // Queue the buffer.
  if(ioctl(fd, VIDIOC_QBUF, bufferinfo) < 0){
    perror("VIDIOC_QBUF");
    exit(1);
  }
  
  // Dequeue the buffer
  if(ioctl(fd, VIDIOC_DQBUF, bufferinfo) < 0){
    perror("VIDIOC_QBUF");
    exit(1);
  }
}

void jpeg(const char *filename, const void *buffer_start, const int length) {
  int jpgfile;
  if((jpgfile = open(filename, O_WRONLY | O_CREAT, 0660)) < 0){
    perror("open");
    exit(1);
  }
 
  write(jpgfile, buffer_start, length);
  close(jpgfile);
}
