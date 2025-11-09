# Conway's Game of Life

Modern C++ implementation of Conway's Game of Life with SFML.

## Features

- Modular architecture with clean separation of concerns
- Interactive mouse and keyboard controls
- Built-in pattern library (glider, oscillators, etc.)
- Responsive layout and adjustable simulation speed

## Building

**Requirements:** CMake 3.28+, C++17 compiler, Git

```bash
mkdir build && cd build
cmake .. && make
./bin/gol
```

## Controls

**Mouse:** Click cells to toggle state, click buttons for controls

**Keyboard:**
- `SPACE` - Pause/Resume
- `R` - Random pattern
- `G` - Glider pattern
- `C` - Clear grid
- `+/-` - Speed control

## Architecture

Modular design with five subsystems:
- `core/` - Game logic and state
- `graphics/` - Rendering and layout
- `input/` - Event handling
- `ui/` - Interface components
- `patterns/` - Pattern library

## License

Open source. Based on John Conway's Game of Life (1970).
