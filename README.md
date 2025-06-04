# Conway's Game of Life - Modular Architecture

A modern C++ implementation of Conway's Game of Life featuring a clean, modular architecture built with SFML. This cellular automaton demonstrates how simple rules can create complex, emergent behaviors.

## Features

- **Modular Design**: Clean separation of concerns with dedicated subsystems
- **Interactive UI**: Mouse controls and keyboard shortcuts
- **Pattern Library**: Built-in classic patterns (glider, oscillators, etc.)
- **Responsive Layout**: Automatic scaling and centering
- **Real-time Simulation**: Adjustable speed controls
- **Visual Polish**: Subtle grid patterns and smooth animations

## Architecture Overview

The codebase is organized into five main subsystems:

```
src/
├── core/           # Game logic and coordination
├── graphics/       # Rendering and visual presentation
├── input/          # Event handling and user interaction
├── ui/             # User interface components
└── patterns/       # Pattern management and library
```

### Core System (`src/core/`)

**GameEngine** - Central coordinator that manages all subsystems and the main game loop
- Orchestrates communication between subsystems
- Manages game state (pause/resume, speed control)
- Provides unified interface for game operations

**Grid** - Manages the cellular automaton state and evolution
- Stores cell states in a 2D boolean array
- Implements Conway's Game of Life rules
- Provides thread-safe cell manipulation methods

### Graphics System (`src/graphics/`)

**Renderer** - Handles all visual rendering and layout calculations
- Maintains responsive grid layout with constant margins
- Renders cells, grid background, and borders
- Calculates screen-to-grid coordinate transformations
- Ensures 1:1 aspect ratio for cells regardless of window size

### Input System (`src/input/`)

**InputHandler** - Processes user input and translates to game actions
- Handles SFML events (mouse, keyboard, window)
- Uses callback system for loose coupling with game logic
- Supports both mouse interaction and keyboard shortcuts
- Manages window resize events and coordinate updates

### UI System (`src/ui/`)

**UIManager** - Coordinates user interface elements and layout
- Manages button creation, positioning, and updates
- Handles UI event routing and state synchronization
- Automatically repositions elements on window resize

**Button** - Individual UI component with hover states and click handling
- Custom symbol rendering (no external font dependencies)
- Visual feedback with hover effects
- Callback-based click handling

### Pattern System (`src/patterns/`)

**PatternManager** - Manages pattern library and application
- Built-in collection of classic Game of Life patterns
- Pattern positioning and centering algorithms
- Random pattern generation with configurable density
- Extensible design for future pattern file I/O

## Building and Running

### Prerequisites
- CMake 3.28 or higher
- C++17 compatible compiler
- Git (for SFML dependency fetching)

### Build Instructions
```bash
mkdir build
cd build
cmake ..
make
./bin/gol
```

SFML will be automatically downloaded and built as part of the CMake configuration.

## Controls

### Mouse Controls
- **Left Click on Cell**: Toggle cell state (alive/dead)
- **Left Click on Buttons**: Use control panel at top of window

### Keyboard Shortcuts
- **SPACE**: Pause/Resume simulation
- **R**: Generate random pattern
- **G**: Create glider pattern
- **C**: Clear entire grid
- **T**: Create test pattern (for debugging)
- **+/=**: Increase simulation speed
- **-**: Decrease simulation speed

### UI Buttons
- **||/▶**: Pause/Resume simulation
- **▲**: Speed up simulation
- **▼**: Speed down simulation
- **●●●**: Generate random pattern
- **Clear**: Clear all cells

## Design Principles

### Separation of Concerns
Each subsystem has a single, well-defined responsibility:
- **Core**: Game logic and state management
- **Graphics**: Visual presentation and layout
- **Input**: User interaction and event handling
- **UI**: Interface components and user experience
- **Patterns**: Pattern library and manipulation

### Loose Coupling
Subsystems communicate through:
- Forward declarations to minimize header dependencies
- Callback functions for event handling
- Abstract interfaces where appropriate
- Smart pointers for automatic memory management

### Extensibility
The architecture supports easy extension:
- New patterns can be added to the PatternManager
- Additional UI components inherit from Button
- Rendering modes can be added to Renderer
- Input methods can extend InputHandler

## File Structure Detail

```
game-of-life/
├── src/
│   ├── main.cpp                    # Entry point and initialization
│   ├── core/
│   │   ├── GameEngine.hpp/.cpp     # Main game coordinator
│   │   └── Grid.hpp/.cpp           # Cellular automaton logic
│   ├── graphics/
│   │   └── Renderer.hpp/.cpp       # Rendering and layout
│   ├── input/
│   │   └── InputHandler.hpp/.cpp   # Event processing
│   ├── ui/
│   │   ├── UIManager.hpp/.cpp      # UI coordination
│   │   └── Button.hpp/.cpp         # Button component
│   └── patterns/
│       └── PatternManager.hpp/.cpp # Pattern library
├── CMakeLists.txt                  # Build configuration
└── README.md                       # This file
```

## Future Enhancements

### Planned Features
- **Pattern File I/O**: Load/save patterns from files
- **Rule Variations**: Support for different cellular automaton rules
- **History Navigation**: Undo/redo functionality
- **Performance Optimization**: Sparse grid representation for large grids
- **Additional Patterns**: Expanded pattern library with categories

### Technical Improvements
- **Configuration System**: External config files for settings
- **Logging Framework**: Structured logging for debugging
- **Unit Tests**: Comprehensive test suite for all subsystems
- **Documentation**: Doxygen-generated API documentation

## Contributing

The modular architecture makes contributions straightforward:

1. **Adding Patterns**: Extend PatternManager with new pattern definitions
2. **UI Enhancements**: Create new Button types or extend UIManager
3. **Rendering Features**: Add new visualization modes to Renderer
4. **Input Methods**: Extend InputHandler for new interaction paradigms

Each subsystem can be developed and tested independently, making the codebase maintainable and contributor-friendly.

## License

This project is open source. See the original Game of Life concept by John Conway (1970).