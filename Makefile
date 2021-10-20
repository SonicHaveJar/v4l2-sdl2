CC = gcc
include = ./include
src = ./src

bin = ./bin
build = ./build

cheese: $(bin) $(build)/webcam.o $(src)/cheese.c
	$(CC) -I $(include) $(build)/webcam.o $(src)/cheese.c -o $(bin)/cheese

sdlcam: $(bin) $(build)/webcam.o $(src)/sdlcam.c
	$(CC) -I $(include) $(build)/webcam.o $(src)/sdlcam.c -lSDL2 -lSDL2_image -o $(bin)/sdlcam

 # gcc sdltest.c -lSDL2 -lSDL2_image
$(build)/webcam.o: $(build) $(include)/webcam.h $(src)/webcam.c
	$(CC) -I $(include) $(src)/webcam.c -c -o $(build)/webcam.o

$(build):
	mkdir $(build)

$(bin):
	mkdir $(bin)

clean:
	rm $(build)/*
