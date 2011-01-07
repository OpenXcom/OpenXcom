# Target system.
# Leave blank for standard Linux builds.
# Put DINGOO for Dingoo A320 builds.
TARGET =

# Directories and files
OBJDIR = ../obj/
BINDIR = ../bin/
SRCS = $(wildcard *.cpp */*.cpp)
OBJS = $(patsubst %.cpp, $(OBJDIR)%.o, $(notdir $(SRCS)))

# Target-specific settings
ifeq ($(TARGET),DINGOO)
CXX = mipsel-linux-g++
SDL-CONFIG = /opt/mipsel-linux-uclibc/usr/bin/sdl-config
BIN = openxcom.dge
else
SDL-CONFIG = sdl-config
BIN = openxcom
endif

# Compiler settings
CXXFLAGS = -Wall -O2 `$(SDL-CONFIG) --cflags` -I/usr/local/include/yaml-cpp $(addprefix -D,$(TARGET))
LDFLAGS = `$(SDL-CONFIG) --libs` -lSDL_gfx -lSDL_mixer -lyaml-cpp

# Rules
all: $(BINDIR)$(BIN)

$(BINDIR)$(BIN): $(OBJS)
	$(CXX) $(OBJS) $(LDFLAGS) -o $(BINDIR)$(BIN)

$(OBJDIR)%.o:: %.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

$(OBJDIR)%.o:: Basescape/%.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<
	
$(OBJDIR)%.o:: Battlescape/%.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

$(OBJDIR)%.o:: Engine/%.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

$(OBJDIR)%.o:: Geoscape/%.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

$(OBJDIR)%.o:: Interface/%.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

$(OBJDIR)%.o:: Menu/%.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<
	
$(OBJDIR)%.o:: Resource/%.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<
	
$(OBJDIR)%.o:: Ruleset/%.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<
	
$(OBJDIR)%.o:: Savegame/%.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

clean:
	rm -f $(BINDIR)$(BIN) $(OBJDIR)*.o

.PHONY: all clean
