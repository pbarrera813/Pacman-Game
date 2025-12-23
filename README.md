Recreation of the original Pac-Man game from the clasic arcade era (1980) written in C++ using SDL2 library
## Controls

| Key | Action                     |
|-------|--------------------------|
| ↑ / W | Move up                  |
| ↓ / S | Move down                |
| ← / A | Move left                |
| → / D | Move right               |
| ESC   | Pause Game               |
| ENTER | Restart (on Game Over)   |

## Sounds used

| File              | Event when it plays                            |
|-------------------|------------------------------------------------|
| `startup.ogg`     | Start of the game                              |
| `eating.wav`      | Pac-man eats a normal pellet                   |
| `powerup.wav`     | Pac-man eats a super pellet                    |
| `ghost_eaten.wav` | A ghost is eaten                               |
| `b2b.wav`         | While ghost is going back to base              |
| `death.wav`       | Pac-man dies                                   |
| `fruit.wav`       | A fruit is eaten                               |
| `siren.ogg`       | Background siren                               |
| `siren2.ogg`      | Sped up siren (certain amount of pellets left) |
| `high-score.ogg`  | Al superar el high score                       |

## Sprites used

## Pac-Man

| File                                | Use                            |
|-------------------------------------|--------------------------------|
| `pac_man_0.png` - `pac_man_4.png`   | Animation waka-waka (5 frames) |
| `pacdeath_0.png` - `pacdeath_2.png` | Death animation     (3 frames) |
| `lifecounter_0.png`                 | Life icon at the bottom        |

## Ghosts

| File                   | Use                            |
|------------------------|--------------------------------|
| `ghost_red_0/1.png`    | Blinky (directly follows)      |
| `ghost_pink_0/1.png`   | Pinky (ambush in the front)    |
| `ghost_blue_0/1.png`   | Inky (complex ambush)          |
| `ghost_orange_0/1.png` | Clyde (erratic)                |
| `afraid_0/1.png`       | Frightened ghost (vulnerable)  |
| `eyes_*.png`           | Eyes going back to base        |

## Items and UI

| File               | Use                       |
|--------------------|---------------------------|
| `pill_0.png`       | Normal pellet             |
| `pill_1.png`       | Power pellet (big)        |
| `spr_cherry_0.png` | Cherry (bonus)            |
| `ready.png`        | Text "READY!"             |
| `game_over.png`    | Text "GAME OVER"          |
| `clear.png`        | Text "Clear"              |
| `arcade_n.ttf`     | Text Font                 |

## Ghost AI:

## Blinky (Red) - "Shadow"

- **Behavior**: Directly follows Pac-man.
- **Strategy**: The most aggresive, always follow.

## Pinky (Pink) - "Speedy"

- **Behavior**: Points 4 tiles in front of Pac-Man.
- **Strategy**: Tries to ambush from the front.

## Inky (Cyan) - "Bashful"

- **Behavior**: Emboscada compleja usando la posición de Blinky
- **Strategy**: Vector desde Blinky → 2 tiles delante de Pac-Man, duplicado

## Clyde (Orange) - "Pokey"

- **Behavior**:
    - IF more than 8 tiles, follows Pac-man like Blinky.
    - If 8 tiles or less from Pac-Man, runs back to it's corner.
- **Strategy**: The most unpredictable.

## Point system

| Item         | Puntos |
|--------------|--------|
| Normal pellet| 10     |
| Power Pellet | 50     |
| Cherry       | 100    |
| 1st Ghost    | 200    |
| 2nd Ghost    | 400    |
| 3rd Ghost    | 800    |
| 4th Ghost    | 1600   |

## Proyect Structure

```
├── Main.cpp              # Punto de entrada
├── Game.cpp/h            # Lógica principal del juego
├── Pacman.cpp/h          # Clase de Pac-Man
├── Ghost.cpp/h           # Clase de los fantasmas
├── GhostAI.cpp/h         # IA de los fantasmas
├── Map.cpp/h             # Laberinto
├── Renderer.cpp/h        # Sistema de renderizado
├── TextureManager.cpp/h  # Gestor de texturas
├── AudioManager.cpp/h    # Gestor de audio
├── Entity.h              # Clase base de entidades
├── Constants.h           # Constantes del juego
├── Direction.h           # Enum de direcciones
├── Math.h                # Estructuras matemáticas
├── Makefile              # Compilación
└── assets/
    ├── fonts/
    │   └── arcade_n.ttf
    ├── gfx/
    │   ├── pacman_counter_death/
    │   ├── ghost/
    │   ├── pill/
    │   ├── cherry/
    │   ├── pacman_text/
    │   └── pacman_tiles/
    └── sounds/
        └── *.wav, *.ogg
```

## Current features

- Fluid movement with pac-man eating animations.
- 4 ghosts with their respective AIs.
- Scatter and chase mods added to ghosts (like the original).
- Ghosts enter "Frightened" mode when a super pellet is eaten.
- Ghosts blink between blue/white when leaving "frightened" mode.
- After being eaten, ghost turn to eyes and go back to base.
- Lateral tunnels.
- Score and high-score system.
- Game over and lives function.
- Cherry appears like in the original.
- Most of the original arcade sound effects play on their respective moment.
- Level transition.
  This project is an educative recreation of the Original Pac-Man game from the arcade version made by NAMCO (1980).
  PAC-MAN™ & © BANDAI NAMCO Entertainment Inc.

## Compilation

### CMake

#### Windows
Up until this point, the compilation of the project as been made with the next compilers:
- MinGW 11.0 w64

To compile using vcpkg
you need to use the command below so it takes care of the needed dependencies:

```bash
$ cmake -B build -S . -DVCPKG_TARGET_TRIPLET=x64-mingw-dynamic -DCMAKE_TOOLCHAIN_FILE=${VCPKG_HOME}\vcpkg\scripts\buildsystems\vcpkg.cmake 
$ cmake --build build -j
```

> [!IMPORTANT]
> It's important to use `-DVCPKG_TARGET_TRIPLET=x64-mingw-dynamic` or  `-DVCPKG_TARGET_TRIPLET=x64-mingw-static` so that
> vcpkg compiles with the SDL2 version that corresponds to MinGW.



ESPAÑOL    

Recreación fiel del juego arcade clásico Pac-Man (1980) usando C++ y SDL2.

## Controles

| Tecla | Acción                   |
|-------|--------------------------|
| ↑ / W | Mover arriba             |
| ↓ / S | Mover abajo              |
| ← / A | Mover izquierda          |
| → / D | Mover derecha            |
| ESC   | Salir del juego          |
| ENTER | Reiniciar (en Game Over) |

## Sonidos Utilizados

| Archivo           | Cuándo se reproduce                      |
|-------------------|------------------------------------------|
| `startup.ogg`     | Al iniciar el juego (melodía de inicio)  |
| `eating.wav`      | Cada vez que Pac-Man come un dot         |
| `powerup.wav`     | Al comer una power pellet                |
| `ghost_eaten.wav` | Al comer un fantasma                     |
| `death.wav`       | Cuando Pac-Man muere                     |
| `fruit.wav`       | Al comer la cereza                       |
| `siren.ogg`       | Sirena de fondo durante el juego         |
| `siren2.ogg`      | Sirena rápida (cuando quedan pocos dots) |
| `high-score.ogg`  | Al superar el high score                 |

## Gráficos Utilizados

## Pac-Man

| Archivo                             | Uso                            |
|-------------------------------------|--------------------------------|
| `pac_man_0.png` - `pac_man_4.png`   | Animación waka-waka (5 frames) |
| `pacdeath_0.png` - `pacdeath_2.png` | Animación de muerte (3 frames) |
| `lifecounter_0.png`                 | Icono de vida en el HUD        |

## Fantasmas

| Archivo                | Uso                            |
|------------------------|--------------------------------|
| `ghost_red_0/1.png`    | Blinky (persigue directamente) |
| `ghost_pink_0/1.png`   | Pinky (emboscada adelante)     |
| `ghost_blue_0/1.png`   | Inky (emboscada compleja)      |
| `ghost_orange_0/1.png` | Clyde (errático)               |
| `afraid_0/1.png`       | Fantasma asustado (vulnerable) |
| `eyes_*.png`           | Ojos volviendo a casa          |

## Items y UI

| Archivo            | Uso                          |
|--------------------|------------------------------|
| `pill_0.png`       | pellet pequeño               |
| `pill_1.png`       | Power pellet (grande)        |
| `spr_cherry_0.png` | Cereza (bonus)               |
| `ready.png`        | Texto "READY!"               |
| `game_over.png`    | Texto "GAME OVER"            |
| `clear.png`        | Texto cuando completas nivel |
| `arcade_n.ttf`     | Fuente para puntuación       |

## IA de los Fantasmas

## Blinky (Rojo) - "Shadow"

- **Comportamiento**: Persigue directamente a Pac-Man
- **Estrategia**: El más agresivo, siempre va hacia ti

## Pinky (Rosa) - "Speedy"

- **Comportamiento**: Apunta 4 tiles delante de Pac-Man
- **Estrategia**: Intenta emboscarte por delante

## Inky (Cyan) - "Bashful"

- **Comportamiento**: Emboscada compleja usando la posición de Blinky
- **Estrategia**: Vector desde Blinky → 2 tiles delante de Pac-Man, duplicado

## Clyde (Naranja) - "Pokey"

- **Comportamiento**:
    - Si está a más de 8 tiles: persigue como Blinky
    - Si está a 8 tiles o menos: huye a su esquina
- **Estrategia**: El más impredecible

## Puntuación

| Item         | Puntos |
|--------------|--------|
| Dot          | 10     |
| Power Pellet | 50     |
| Cereza       | 100    |
| 1er Fantasma | 200    |
| 2do Fantasma | 400    |
| 3er Fantasma | 800    |
| 4to Fantasma | 1600   |

## Estructura del Proyecto

```
├── Main.cpp              # Punto de entrada
├── Game.cpp/h            # Lógica principal del juego
├── Pacman.cpp/h          # Clase de Pac-Man
├── Ghost.cpp/h           # Clase de los fantasmas
├── GhostAI.cpp/h         # IA de los fantasmas
├── Map.cpp/h             # Laberinto
├── Renderer.cpp/h        # Sistema de renderizado
├── TextureManager.cpp/h  # Gestor de texturas
├── AudioManager.cpp/h    # Gestor de audio
├── Entity.h              # Clase base de entidades
├── Constants.h           # Constantes del juego
├── Direction.h           # Enum de direcciones
├── Math.h                # Estructuras matemáticas
├── Makefile              # Compilación
└── assets/
    ├── fonts/
    │   └── arcade_n.ttf
    ├── gfx/
    │   ├── pacman_counter_death/
    │   ├── ghost/
    │   ├── pill/
    │   ├── cherry/
    │   ├── pacman_text/
    │   └── pacman_tiles/
    └── sounds/
        └── *.wav, *.ogg
```

## Características Implementadas

- Movimiento fluido de Pac-Man con animación waka-waka.
- 4 fantasmas con IA única para cada uno.
- Modos Scatter/Chase alternantes (como el original).
- Modo Frightened cuando comes power pellet.
- Parpadeo de fantasmas cuando el efecto termina.
- Fantasmas vuelven a casa como ojos cuando son comidos.
- Túneles laterales con velocidad reducida para fantasmas.
- Sistema de puntuación completo.
- Vidas y Game Over.
- Cereza bonus.
- Todos los sonidos del arcade.
- Transición de niveles.
  Este proyecto es una recreación educativa del juego Pac-Man original de Namco (1980)
  PAC-MAN™ & © BANDAI NAMCO Entertainment Inc.

## Compilacion

### CMake

#### Windows

Hasta ahora se ha probado con los siguientes compiladores:

- MinGW 11.0 w64

Para compilar usando vcpkg para que se encargue de las dependencias:

```bash
$ cmake -B build -S . -DVCPKG_TARGET_TRIPLET=x64-mingw-dynamic -DCMAKE_TOOLCHAIN_FILE=${VCPKG_HOME}\vcpkg\scripts\buildsystems\vcpkg.cmake 
$ cmake --build build -j
```

> [!IMPORTANT]
> Es importante usar `-DVCPKG_TARGET_TRIPLET=x64-mingw-dynamic` o  `-DVCPKG_TARGET_TRIPLET=x64-mingw-static` para que
> vcpkg compile la version de SDL2 correspondiente a MinGW.
