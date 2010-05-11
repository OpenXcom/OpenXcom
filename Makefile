CC = g++
CFLAGS = -Wall -O2
COMPILE = $(CC) $(CFLAGS) -c
OBJFILES := $(patsubst %.cpp,%.o,$(wildcard *.cpp))
LIBS := `sdl-config --cflags --libs` -lSDL_gfx -lSDL_mixer

all: openxcom

openxcom: $(OBJFILES)
	$(CC) -o OpenXcom $(OBJFILES) $(LIBS)

%.o: %.cpp
	$(COMPILE) -o $@ $< $(LIBS)

.PHONY: clean
 
clean:
	rm -f OpenXcom *.o
