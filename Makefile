# Makefile para Pac-Man en C++
# Compatible con MSYS2/MinGW64 (Windows) y Linux

# Detectar sistema operativo
ifeq ($(OS),Windows_NT)
    # Windows con MSYS2/MinGW64
    EXE = pacman.exe
    RM = del /Q
    MKDIR = if not exist "bin" mkdir bin
    SDL_CFLAGS = $(shell sdl2-config --cflags)
    SDL_LIBS = $(shell sdl2-config --libs) -lSDL2_image -lSDL2_mixer -lSDL2_ttf
else
    # Linux/macOS
    EXE = pacman
    RM = rm -f
    MKDIR = mkdir -p bin
    SDL_CFLAGS = $(shell sdl2-config --cflags)
    SDL_LIBS = $(shell sdl2-config --libs) -lSDL2_image -lSDL2_mixer -lSDL2_ttf
endif

# Compilador y flags
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2 $(SDL_CFLAGS)
LDFLAGS = $(SDL_LIBS)

# Archivos fuente
SOURCES = Main.cpp \
          Game.cpp \
          Pacman.cpp \
          Ghost.cpp \
          GhostAI.cpp \
          Map.cpp \
          Renderer.cpp \
          TextureManager.cpp \
          AudioManager.cpp

# Archivos objeto
OBJECTS = $(SOURCES:.cpp=.o)

# Regla principal
all: $(EXE)

$(EXE): $(OBJECTS)
	$(CXX) $(OBJECTS) -o $(EXE) $(LDFLAGS)
	@echo "Build complete: $(EXE)"

# Compilar archivos .cpp a .o
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Limpiar
clean:
	$(RM) $(OBJECTS) $(EXE) *.o

# Ejecutar
run: $(EXE)
ifeq ($(OS),Windows_NT)
	./$(EXE)
else
	./$(EXE)
endif

# Información de debug
info:
	@echo "Compiler: $(CXX)"
	@echo "CXXFLAGS: $(CXXFLAGS)"
	@echo "LDFLAGS: $(LDFLAGS)"
	@echo "Sources: $(SOURCES)"
	@echo "Objects: $(OBJECTS)"
	@echo "Executable: $(EXE)"

# Dependencias
Main.o: Main.cpp Game.h
Game.o: Game.cpp Game.h Pacman.h Ghost.h GhostAI.h Map.h Renderer.h TextureManager.h AudioManager.h Constants.h
Pacman.o: Pacman.cpp Pacman.h Entity.h Map.h AudioManager.h Constants.h
Ghost.o: Ghost.cpp Ghost.h Entity.h Map.h TextureManager.h Constants.h
GhostAI.o: GhostAI.cpp GhostAI.h Ghost.h Pacman.h Constants.h
Map.o: Map.cpp Map.h Constants.h
Renderer.o: Renderer.cpp Renderer.h TextureManager.h Map.h Constants.h
TextureManager.o: TextureManager.cpp TextureManager.h
AudioManager.o: AudioManager.cpp AudioManager.h

.PHONY: all clean run info