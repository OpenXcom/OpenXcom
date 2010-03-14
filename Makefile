CC = g++
CFLAGS = -Wall -O2
COMPILE = $(CC) $(CFLAGS) -c
OBJFILES := $(patsubst %.cpp,%.o,$(wildcard *.cpp))

all: openxcom

openxcom: $(OBJFILES)
	$(CC) -o OpenXcom $(OBJFILES) `sdl-config --cflags --libs` -lSDL_gfx

%.o: %.cpp
	$(COMPILE) -o $@ $< `sdl-config --cflags --libs` -lSDL_gfx

.PHONY: clean
 
clean:
	rm -f OpenXcom *.o
