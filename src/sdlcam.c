#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "webcam.h"

int main() { 
  int fd = new_descriptor("/dev/video0");

  check_capabilities(fd);

  struct v4l2_format format = set_format(fd, V4L2_PIX_FMT_MJPEG, 640, 480);

  request_buffer(fd);
  
  void *buffer_start = allocate_buffer(fd);
  
  struct v4l2_buffer bufferinfo = prepare_buffer(fd);

  int type = streamon(fd, bufferinfo);

  int width = format.fmt.pix.width;
  int height = format.fmt.pix.height;

  SDL_Init(SDL_INIT_VIDEO);
  IMG_Init(IMG_INIT_JPG);
  SDL_Window *window = SDL_CreateWindow("Webcam",
                                        SDL_WINDOWPOS_UNDEFINED,
                                        SDL_WINDOWPOS_UNDEFINED,
                                        width, height,
                                        SDL_WINDOW_OPENGL);

  SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, 
                                              SDL_RENDERER_ACCELERATED | 
                                              SDL_RENDERER_PRESENTVSYNC);
  SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
  
  SDL_RWops *buffer_stream;
  SDL_Surface *frame;
  SDL_Texture *frame_tex;
  SDL_Rect position = {.x = 0, 
                       .y = 0,
                       .w = width,
                       .h = height};

  bool running = true;
  SDL_Event event;

  while (running) {
    while(SDL_PollEvent(&event)) {
      if (event.type == SDL_QUIT) running = false;
    }
    SDL_RenderClear(renderer);
    // --
    shot(fd, &bufferinfo);

    buffer_stream = SDL_RWFromMem(buffer_start, bufferinfo.length);
    frame = IMG_Load_RW(buffer_stream, 0);
    frame_tex = SDL_CreateTextureFromSurface(renderer, frame);

    SDL_RenderCopy(renderer, frame_tex, NULL, &position);
    // --
    SDL_RenderPresent(renderer);
  }
  
  streamoff(fd, type);
  close(fd);

  IMG_Quit();
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();

}
