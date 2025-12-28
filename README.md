# Pac-Man (C++ / SDL2)
Faithful recreation of the classic arcade game Pac-Man (1980) using C++ and the SDL2 library.  
Version 1.2

## Controls

| Key           | Action                         |
|---------------|--------------------------------|
| ↑ / W         | Move up                        |
| ↓ / S         | Move down                      |
| ← / A         | Move left                      |
| → / D         | Move right                     |
| ESC           | Pause the game                 |
| ENTER         | Restart (on Game Over)         |
| Volume Icon   | *Click* 100/50/25/mute         | <- NEW !

## Sounds Used

| File              | When it plays                               |
|-------------------|---------------------------------------------|
| `startup.ogg`     | When the game starts (intro melody)        |
| `eating.wav`      | Every time Pac-Man eats a dot              |
| `powerup.wav`     | When eating a power pellet                 |
| `ghost_eaten.wav` | When eating a ghost                        |
| `b2b.wav`         | While the ghost returns to the house       |
| `death.wav`       | When Pac-Man dies                          |
| `fruit.wav`       | When eating a fruit                        |
| `siren.ogg`       | Background siren during the game           |
| `siren2.ogg`      | Fast siren (when few dots remain)          |
| `high-score.ogg`  | When beating the high score                |
| `pause.wav`       | When pressing Esc the first time           | <- NEW !
| `unpause.wav`     | When pressing Esc the second time          | <- NEW !
| `success.wav`     | High-score reset / volume change           | <- NEW !

## Graphics Used

## Pac-Man
| File                                   | Usage                                  |
|----------------------------------------|----------------------------------------|
| `pac_man_0.png` - `pac_man_4.png`      | Waka-waka animation (5 frames)         |
| `pacdeath_0.png` - `pacdeath_11.png`   | Death animation (12 frames)            | <- NEW !
| `lifecounter_0.png`                    | Life icon in the HUD                  |
| `spr_x_0.png`                          | Fruit display on the right HUD side   | <- NEW !

## Ghosts
## Ghosts now have (in total) 32 new animation frames, 8 each <- NEW !
| File                         | Usage                         |
|------------------------------|-------------------------------|
| `ghost_red_up_0/1.png`       | Blinky (looking up)           |
| `ghost_red_down_0/1.png`     | Blinky (looking down)         |
| `ghost_red_left_0/1.png`     | Blinky (looking left)         |
| `ghost_red_right_0/1.png`    | Blinky (looking right)        |
| `ghost_pink_up_0/1.png`      | Pinky (looking up)            |
| `ghost_pink_down_0/1.png`    | Pinky (looking down)          |
| `ghost_pink_left_0/1.png`    | Pinky (looking left)          |
| `ghost_pink_right_0/1.png`   | Pinky (looking right)         |
| `ghost_blue_up_0/1.png`      | Inky (looking up)             |
| `ghost_blue_down_0/1.png`    | Inky (looking down)           |
| `ghost_blue_left_0/1.png`    | Inky (looking left)           |
| `ghost_blue_right_0/1.png`   | Inky (looking right)          |
| `ghost_orange_up_0/1.png`    | Clyde (looking up)            |
| `ghost_orange_down_0/1.png`  | Clyde (looking down)          |
| `ghost_orange_left_0/1.png`  | Clyde (looking left)          |
| `ghost_orange_right_0/1.png` | Clyde (looking right)         |

## "Frightened" State
Ghosts now have 4 animation frames in this state for better fidelity. <- NEW !
| File           | Color | Usage                    |
|----------------|-------|--------------------------|
| `afraid_0.png` | Blue  | Normal animation         |
| `afraid_1.png` | Blue  | Normal animation         |
| `afraid_2.png` | White | Warning blink            |
| `afraid_3.png` | White | Warning blink            |

## Eye System
| Eye file       | Usage                                               |
|----------------|-----------------------------------------------------|
| `eyes_left.png`  | Eyes returning home (looking left)                |
| `eyes_right.png` | Eyes returning home (looking right)               |
| `eyes_up.png`    | Eyes returning home (looking up)                  |
| `eyes_down.png`  | Eyes returning home (looking down)                |

## Items and UI
| File              | Usage                  |
|-------------------|------------------------|
| `pill_0.png`      | Small pellet           |
| `pill_1.png`      | Power pellet           |
| `spr_cherry_0.png`| Fruit (bonus)          |
| `spr_strawberry_0.png` | Fruit (bonus)   |
| `spr_orange_0.png`| Fruit (bonus)          |
| `spr_apple_0.png` | Fruit (bonus)          |
| `spr_melon_0.png` | Fruit (bonus)          | <- NEW !
| `spr_ship_0.png`  | Fruit (bonus)          | <- NEW !
| `spr_bell_0.png`  | Fruit (bonus)          | <- NEW !
| `spr_key_0.png`   | Fruit (bonus)          | <- NEW !
| `ready.png`       | "READY!" text          |
| `game_over.png`   | "GAME OVER" text       |
| `clear.png`       | Level complete text    |
| `arcade_n.ttf`    | Score font             |
| `sound_100.png`   | Sound icon 100%        | <- NEW !
| `sound_50.png`    | Sound icon 50%         | <- NEW !
| `sound_25.png`    | Sound icon 25%         | <- NEW !
| `no_sound.png`    | Mute icon              | <- NEW !

## Ghost AI
Ghost AI Bug (v1.1)  
Problem: Blinky and Pinky would get stuck looping in corners during Chase mode.  
Cause: When switching between Scatter and Chase, ghosts did not reverse direction (required behavior in the original arcade).  
Solution: Added `reverseDirection()` in `Ghost::setMode()` when transitioning between Scatter ↔ Chase.

## Scoring

| Item          | Points |
|---------------|--------|
| Dot           | 10     |
| Power Pellet  | 50     |
| 1st Ghost    | 200    |
| 2nd Ghost    | 400    |
| 3rd Ghost    | 800    |
| 4th Ghost    | 1600   |

## Fruit Scoring
| Level | Fruit      | Points |
|-------|------------|--------|
| 1     | Cherry     | 100    |
| 2     | Strawberry | 300    |
| 3-4   | Orange     | 500    |
| 5-6   | Apple      | 700    |
| 7-8   | Melon      | 1000   | <- NEW !
| 9-10  | Ship       | 2000   | <- NEW !
| 11-12 | Bell       | 3000   | <- NEW !
| 13+   | Key        | 5000   | <- NEW !

## Proyect Structure
```
assets/
├── fonts/
│   └── arcade_n.ttf
├── gfx/
│   ├── fruits/           (8 frutas)
│   ├── ghost/
│   │   ├── red_ghost/    (8 sprites direccionales)
│   │   ├── pink_ghost/   (8 sprites direccionales)
│   │   ├── blue_ghost/   (8 sprites direccionales)
│   │   ├── orange_ghost/ (8 sprites direccionales)
│   │   ├── ghost_afraid/ (4 frames)
│   │   └── eyes/         (4 direcciones)
│   ├── pacman/           (5 animación + 3 direccionales)
│   ├── pacman_counter/   (vida)
│   ├── pacman_death/     (12 frames)
│   ├── pacman_text/      (ready, pause, game_over, clear, enter)
│   ├── pill/             (normal y power)
│   ├── points/
│   │   ├── fruits/       (8 sprites de puntaje)
│   │   └── ghosts/       (4 sprites de puntaje)
│   └── volume/           (4 iconos) ← NUEVO
└── sounds/               (13 archivos de audio)
```

# Pac-Man (C++ / SDL2)

Recreación fiel del juego arcade clásico Pac-Man (1980) usando C++ y Libreria SDL2.  
Version 1.2

## Controles

|    Tecla      |         Acción           |
|---------------|--------------------------|
|    ↑ / W      | Mover arriba             |
|    ↓ / S      | Mover abajo              |
|    ← / A      | Mover izquierda          |
|    → / D      | Mover derecha            |
|     ESC       | Pausar el juego          |
|     ENTER     | Reiniciar (en Game Over) |
| Volumen Icono | *Clic* 100/50/25/mute    | <- NUEVO !

## Sonidos Utilizados

|      Archivo       |     Cuándo se reproduce                 |
|--------------------|-----------------------------------------|
| `startup.ogg`      | Al iniciar el juego (melodía de inicio) |
| `eating.wav`       | Cada vez que Pac-Man come un dot        |
| `powerup.wav`      | Al comer una power pellet               |
| `ghost_eaten.wav`  | Al comer un fantasma                    |
| `b2b.wav`          | Mientras el fantasma vuelve a la casa   |
| `death.wav`        | Cuando Pac-Man muere                    |
| `fruit.wav`        | Al comer una fruta                      |
| `siren.ogg`        | Sirena de fondo durante el juego        |
| `siren2.ogg`       | Sirena rápida (cuando quedan pocos dots)|
| `high-score.ogg`   | Al superar el high score                |
| `pause.wav`        | Al presionar Esc por primera vez        | <- NUEVO !
| `unpause.wav`      | Al presionar Esc por segunda vez        | <- NUEVO !
| `success.wav`      | Reinicio de high-score/cambio volumen   | <- NUEVO !


## Gráficos Utilizados

## Pac-Man
|              Archivo                 |                 Uso                     |
|--------------------------------------|-----------------------------------------|
| `pac_man_0.png` - `pac_man_4.png`    | Animación waka-waka (5 frames)          |
| `pacdeath_0.png` - `pacdeath_11.png` | Animación de muerte (12 frames)         | <- NUEVO !
| `lifecounter_0.png`                  | Icono de vida en el HUD                 |
| `spr_x_0.png`                        | Se muestra fruta en lado derecho del HUD| <- NUEVO !

## Fantasmas 
## Ahora los fantasmas tienen (en  total) 32 frames nuevos de animacion, 8 cada uno <- NUEVO !
|            Archivo           |           Uso                  |
|------------------------------|--------------------------------|
| `ghost_red_up_0/1.png`       | Blinky (mira hacia arriba)     |
| `ghost_red_down_0/1.png`     | Blinky (mira hacia abajo)      |
| `ghost_red_left_0/1.png`     | Blinky (mira hacia izquierda)  |
| `ghost_red_right_0/1.png`    | Blinky (mira hacia derecha)    |
| `ghost_pink_up_0/1.png`      | Pinky (mira hacia arriba)      |
| `ghost_pink_down_0/1.png`    | Pinky (mira hacia abajo)       |
| `ghost_pink_left_0/1.png`    | Pinky (mira hacia izquierda)   |
| `ghost_pink_right_0/1.png`   | Pinky (mira hacia derecha)     |
| `ghost_blue_up_0/1.png`      | Inky (mira hacia arriba)       |
| `ghost_blue_down_0/1.png`    | Inky (mira hacia abajo)        |
| `ghost_blue_left_0/1.png`    | Inky (mira hacia izquierda)    |
| `ghost_blue_right_0/1.png`   | Inky (mira hacia derecha)      |
| `ghost_orange_up_0/1.png`    | Clyde (mira hacia arriba)      |
| `ghost_orange_down_0/1.png`  | Clyde (mira hacia abajo)       |
| `ghost_orange_left_0/1.png`  | Clyde (mira hacia izquierda)   |
| `ghost_orange_right_0/1.png` | Clyde (mira hacia derecha)     |


## Estado "Frigthened"
Ahora cuentan con 4 frames de animacion en este estado, para una mejor fidelidad. <- NUEVO !
|    Archivo     | Color  |        Uso              |
| `afraid_0.png` | Azul   | Animación normal        |
| `afraid_1.png` | Azul   | Animación normal        |
| `afraid_2.png` | Blanco | Parpadeo de advertencia |
| `afraid_3.png` | Blanco | Parpadeo de advertencia |

## Sistema de ojos 
|----------------------- Archivo de ojos ------------------------------------------|
| `eyes_left.png`              | Ojos volviendo a casa (mirando hacia la izquierda)|
| `eyes_right.png`             | Ojos volviendo a casa (mirando hacia la derecha)  |
| `eyes_up.png`                | Ojos volviendo a casa (mirando hacia arriba)      |
| `eyes_down.png`              | Ojos volviendo a casa (mirando hacia abajo)       |

## Items y UI
|         Archivo         |              Uso                 |
|-------------------------|----------------------------------|
| `pill_0.png`            | pellet pequeño                   |
| `pill_1.png`            | Power pellet (grande)            |
| `spr_cherry_0.png`      | Fruta (bonus)                    |
| `spr_strawberry_0.png`  | Fruta (bonus)                    |
| `spr_orange_0.png`      | Fruta (bonus)                    |
| `spr_apple_0.png`       | Fruta (bonus)                    |
| `spr_melon_0.png`       | Fruta (bonus)                    | <- NUEVO !
| `spr_ship_0.png`        | Fruta (bonus)                    | <- NUEVO !
| `spr_bell_0.png`        | Fruta (bonus)                    | <- NUEVO !
| `spr_key_0.png`         | Fruta (bonus)                    | <- NUEVO !
| `ready.png`             | Texto "READY!"                   |
| `game_over.png`         | Texto "GAME OVER"                |
| `clear.png`             | Texto cuando completas nivel     |
| `arcade_n.ttf`          | Fuente para puntuación           |
| `sound_100.png`         | Icono sonido, 100%               | <- NUEVO !
| `sound_50.png`          | Icono sonido, 50%                | <- NUEVO !
| `sound_25.png`          | Icono sonido, 25%                | <- NUEVO !
| `no_sound.png`          | Icono sonido, sin sonido         | <- NUEVO !

## IA de los Fantasmas
Bug de IA de Fantasmas (v1.1)
Problema: Blinky y Pinky se quedaban atrapados dando vueltas en las esquinas durante modo Chase.
## Causa: Al cambiar entre Scatter y Chase, los fantasmas no invertían su dirección (comportamiento requerido del arcade original).
Solución: Agregado reverseDirection() en Ghost::setMode() al transicionar entre Scatter ↔ Chase.

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

|    Item      |Puntos|
|--------------|------|
|     Dot      | 10   |
| Power Pellet | 50   |
| 1er Fantasma | 200  |
| 2do Fantasma | 400  |
| 3er Fantasma | 800  |
| 4to Fantasma | 1600 |

## Puntuacion Frutas
|  Nivel |    Fruta       |  Puntos  |
|    1   |    Cherry      |   100    |
|    2   |   Strawberry   |   300    |
|   3-4  |    Orange      |   500    |
|   5-6  |    Apple       |   700    |
|   7-8  |    Melon       |  1000    | <- NUEVO !
|  9-10  |    Ship        |  2000    | <- NUEVO !
| 11-12  |    Bell        |  3000    | <- NUEVO !
|   13+  |    Key         |  5000    | <- NUEVO !


## Estructura del Proyecto

```
assets/
├── fonts/
│   └── arcade_n.ttf
├── gfx/
│   ├── fruits/           (8 frutas)
│   ├── ghost/
│   │   ├── red_ghost/    (8 sprites direccionales)
│   │   ├── pink_ghost/   (8 sprites direccionales)
│   │   ├── blue_ghost/   (8 sprites direccionales)
│   │   ├── orange_ghost/ (8 sprites direccionales)
│   │   ├── ghost_afraid/ (4 frames)
│   │   └── eyes/         (4 direcciones)
│   ├── pacman/           (5 animación + 3 direccionales)
│   ├── pacman_counter/   (vida)
│   ├── pacman_death/     (12 frames)
│   ├── pacman_text/      (ready, pause, game_over, clear, enter)
│   ├── pill/             (normal y power)
│   ├── points/
│   │   ├── fruits/       (8 sprites de puntaje)
│   │   └── ghosts/       (4 sprites de puntaje)
│   └── volume/           (4 iconos) ← NUEVO
└── sounds/               (13 archivos de audio)
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
- Frutas (todas!)
- Todos los sonidos del arcade (y algunos personalizados!).
- Transición de niveles (mapa parpadea azul y blanco) <- NUEVO !
- Icono personalizado.

Este proyecto es una recreación educativa del juego Pac-Man original de Namco (1980).
PAC-MAN™ & © BANDAI NAMCO Entertainment Inc.