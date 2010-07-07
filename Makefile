PROG = openxcom
SRCS = $(wildcard *.cpp)
OBJS = $(SRCS:.cpp=.o)

CXXFLAGS = -Wall -O2 `sdl-config --cflags`
LDFLAGS = `sdl-config --libs` -lSDL_gfx -lSDL_mixer

$(PROG): $(OBJS)
	$(CXX) $(OBJS) $(LDFLAGS) -o $(PROG)

clean:
	rm -f $(PROG) *.o

.PHONY: clean