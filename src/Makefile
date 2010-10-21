TARGET =

SRCDIR = ../src/
OBJDIR = ../obj/
BINDIR = ../bin/
SRCS = $(wildcard $(SRCDIR)*.cpp $(SRCDIR)*/*.cpp)
OBJS = $(SRCS:.cpp=.o)
OBJS = $(addprefix $(OBJDIR),$(OBJS))

ifeq ($(TARGET),DINGOO)
CXX = mipsel-linux-g++
SDL-CONFIG = /opt/mipsel-linux-uclibc/usr/bin/sdl-config
BIN = openxcom.dge
else
SDL-CONFIG = sdl-config
BIN = openxcom
endif

CXXFLAGS = -Wall -O2 `$(SDL-CONFIG) --cflags` -d$(TARGET)
LDFLAGS = `$(SDL-CONFIG) --libs` -lSDL_gfx -lSDL_mixer

$(BINDIR)$(BIN): $(OBJS)
	$(CXX) $(OBJS) $(LDFLAGS) -o $(BINDIR)$(BIN)

$(OBJDIR)%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $(input) -o $(output)

clean:
	rm -f $(BINDIR)$(BIN) $(OBJDIR)*.o

.PHONY: clean