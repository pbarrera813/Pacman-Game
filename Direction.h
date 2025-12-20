// Direction.h
#pragma once

enum class Direction {
    None,
    Up,
    Down,
    Left,
    Right
};

// Función auxiliar para obtener dirección opuesta
inline Direction oppositeDirection(Direction d) {
    switch (d) {
        case Direction::Up:    return Direction::Down;
        case Direction::Down:  return Direction::Up;
        case Direction::Left:  return Direction::Right;
        case Direction::Right: return Direction::Left;
        default: return Direction::None;
    }
}
