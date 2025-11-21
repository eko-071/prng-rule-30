# PRNG30 - Rule 30 Cellular Automaton PRNG

A high-quality pseudo-random number generator based on the Rule 30 cellular automaton. This library provides statistically sound random numbers suitable for simulations, games, procedural generation, and Monte Carlo methods.

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)
[![C Standard](https://img.shields.io/badge/C-C99-blue.svg)](https://en.wikipedia.org/wiki/C99)

## Features

- ✅ **Excellent Statistical Quality** - Passes chi-squared, runs, and autocorrelation tests
- ✅ **Deterministic** - Same seed produces identical sequences
- ✅ **Flexible** - Generate 1 to 64 bits per call
- ✅ **Configurable** - Adjustable automaton size (32-256+ cells)
- ✅ **Visualization** - Animated display of cellular automaton evolution
- ✅ **Cross-platform** - Works on Linux, macOS, and Windows
- ✅ **Well-tested** - Comprehensive test suite included

## Statistical Properties

| Test | Result | Quality |
|------|--------|---------|
| Chi-Squared (10 bins, 10k samples) | χ² ≈ 7.04 / 16.92 | ⭐⭐⭐⭐⭐ |
| Bit Distribution (64 bits) | 64/64 within 40-60% | ⭐⭐⭐⭐⭐ |
| Runs Test | Z-score ≈ 0.06 | ⭐⭐⭐⭐⭐ |
| Autocorrelation (lag-1) | r ≈ -0.009 | ⭐⭐⭐⭐⭐ |
| Birthday Spacing | 0 collisions in 500 samples | ⭐⭐⭐⭐⭐ |

**Note:** Not suitable for cryptographic purposes. Use a cryptographically secure PRNG for security applications.

## Quick Start

### Building from Source

```bash
git clone https://github.com/eko-071/prng-rule-30.git
cd prng-rule-30
mkdir build && cd build
cmake ..
make
```

### Installation

```bash
# System-wide installation (requires sudo)
sudo make install

# Or install to custom location
cmake -DCMAKE_INSTALL_PREFIX=$HOME/.local ..
make install
```

### Basic Usage

```c
#include <prng30.h>
#include <stdio.h>

int main() {
    // Initialize the PRNG
    prng30_state prng;
    prng30_init(&prng, 12345, 64);  // seed=12345, width=64
    
    // Generate random numbers
    uint64_t random32 = prng30_generate(&prng, 32);  // 32-bit number
    uint64_t random64 = prng30_generate(&prng, 64);  // 64-bit number
    
    printf("Random 32-bit: %lu\n", random32);
    printf("Random 64-bit: %lu\n", random64);
    
    // Clean up
    prng30_free(&prng);
    return 0;
}
```

### Compilation

```bash
# Method 1: Direct compilation
gcc myprogram.c -lprng30 -o myprogram

# Method 2: Using CMake
# In your CMakeLists.txt:
find_package(prng30 REQUIRED)
target_link_libraries(myprogram prng30::prng30)
```

## API Reference

### Initialization and Cleanup

#### `void prng30_init(prng30_state *st, uint64_t seed, int width)`
Initialize the cellular automaton PRNG.

**Parameters:**
- `st` - Pointer to the PRNG state structure
- `seed` - 64-bit seed value (use different seeds for different sequences)
- `width` - Size of the automaton grid (recommended: 64, 128, or 256)

**Example:**
```c
prng30_state prng;
prng30_init(&prng, 42, 64);
```

#### `void prng30_free(prng30_state *st)`
Free memory allocated for the PRNG state.

**Parameters:**
- `st` - Pointer to the PRNG state structure

**Example:**
```c
prng30_free(&prng);
```

### Random Number Generation

#### `uint64_t prng30_generate(prng30_state *st, int nbits)`
Generate random bits from the automaton.

**Parameters:**
- `st` - Pointer to the PRNG state structure
- `nbits` - Number of random bits to generate (1-64)

**Returns:**
- `uint64_t` - Random number with `nbits` bits of entropy

**Example:**
```c
uint64_t rand8  = prng30_generate(&prng, 8);   // 8-bit random
uint64_t rand16 = prng30_generate(&prng, 16);  // 16-bit random
uint64_t rand32 = prng30_generate(&prng, 32);  // 32-bit random
uint64_t rand64 = prng30_generate(&prng, 64);  // 64-bit random
```

### Visualization

#### `void prng30_visualize_animated(prng30_state *st)`
Display an animated visualization of the cellular automaton evolution.

**Parameters:**
- `st` - Pointer to the PRNG state structure

**Example:**
```c
prng30_state prng;
prng30_init(&prng, 12345, 40);  // Smaller size for better visualization
prng30_visualize_animated(&prng);
prng30_free(&prng);
```

### Utility Functions

#### `void prng30_step(prng30_state *st)`
Advance the automaton by one generation. This is called internally by `prng30_generate()`.

**Parameters:**
- `st` - Pointer to the PRNG state structure

## Examples

### Dice Rolling Simulator

```c
#include <prng30.h>
#include <stdio.h>

int main() {
    prng30_state prng;
    prng30_init(&prng, 777, 64);
    
    printf("Rolling 20 six-sided dice:\n");
    for (int i = 0; i < 20; i++) {
        uint64_t random = prng30_generate(&prng, 8);
        int dice = (random % 6) + 1;  // 1-6
        printf("%d ", dice);
    }
    printf("\n");
    
    prng30_free(&prng);
    return 0;
}
```

### Random Range Function

```c
#include <prng30.h>

// Generate random integer in range [min, max]
uint32_t random_range(prng30_state *prng, uint32_t min, uint32_t max) {
    uint32_t range = max - min + 1;
    uint64_t random = prng30_generate(prng, 32);
    return min + (random % range);
}

int main() {
    prng30_state prng;
    prng30_init(&prng, 12345, 64);
    
    // Generate numbers between 1 and 100
    for (int i = 0; i < 10; i++) {
        printf("%u ", random_range(&prng, 1, 100));
    }
    printf("\n");
    
    prng30_free(&prng);
    return 0;
}
```

### Time-Based Seed

```c
#include <prng30.h>
#include <time.h>

int main() {
    prng30_state prng;
    
    // Use current time as seed for non-deterministic results
    uint64_t seed = (uint64_t)time(NULL);
    prng30_init(&prng, seed, 64);
    
    uint64_t random = prng30_generate(&prng, 32);
    printf("Random number: %lu\n", random);
    
    prng30_free(&prng);
    return 0;
}
```

## Running Tests

The library includes a comprehensive test suite:

```bash
cd build
./tests
```

The test suite validates:
- Memory management
- Deterministic behavior
- Statistical quality (chi-squared, bit distribution, runs test)
- Autocorrelation
- Edge cases
- Multiple grid sizes

## Running Examples

```bash
cd build
./example
```

The example program demonstrates:
- Basic random number generation
- Different bit widths
- Time-based seeds
- Dice rolling simulation
- Statistical distribution checks
- Cellular automaton visualization

## How It Works

PRNG30 is based on Rule 30, a one-dimensional cellular automaton discovered by Stephen Wolfram. Despite its simple rules, it exhibits complex, chaotic behavior that makes it suitable for random number generation.

### Rule 30 Definition

For each cell, the next generation is computed from the current cell and its two neighbors:

```
Current:  111  110  101  100  011  010  001  000
Next:      0    0    0    1    1    1    1    0
```

In binary: `00011110` = 30 (hence "Rule 30")

### Algorithm

1. Initialize a row of cells with bits from the seed
2. Apply Rule 30 to generate the next row
3. Extract random bits from the center column
4. Repeat for as many bits as needed

The library enhances basic Rule 30 by:
- Using a 2D grid that wraps circularly
- Extracting bits from multiple columns for better mixing
- Running warmup iterations to ensure good entropy
- Supporting variable grid sizes for different applications

## Project Structure

```
prng-rule-30/
├── CMakeLists.txt          # Build configuration
├── LICENSE                 # MIT License
├── README.md              # This file
├── cmake/
│   └── prng30Config.cmake.in  # CMake package config
├── include/
│   └── prng30.h           # Public API header
├── src/
│   ├── prng.c             # Core PRNG implementation
│   └── visualizer.c       # Visualization functions
├── examples/
│   └── example.c          # Usage examples
└── tests/
    └── tests.c            # Comprehensive test suite
```

## CMake Options

| Option | Default | Description |
|--------|---------|-------------|
| `BUILD_SHARED_LIBS` | OFF | Build shared library instead of static |
| `INSTALL_EXAMPLES` | ON | Install example and test programs |
| `CMAKE_INSTALL_PREFIX` | /usr/local | Installation directory |

**Examples:**

```bash
# Build shared library
cmake -DBUILD_SHARED_LIBS=ON ..

# Install to custom location
cmake -DCMAKE_INSTALL_PREFIX=$HOME/.local ..

# Don't install examples
cmake -DINSTALL_EXAMPLES=OFF ..
```

## Performance Considerations

- **Grid Size**: Larger grids (128-256) provide better statistical quality but use more memory
- **Bit Width**: Generating more bits per call is more efficient due to warmup overhead
- **Memory**: Grid size of N requires N² bytes of memory
- **Speed**: Approximately 1-10 million random numbers per second (depends on grid size and CPU)

## Recommended Grid Sizes

| Grid Size | Memory Usage | Use Case |
|-----------|--------------|----------|
| 32 | 1 KB | Lightweight, embedded systems |
| 64 | 4 KB | General purpose (recommended) |
| 128 | 16 KB | High-quality randomness |
| 256 | 64 KB | Maximum quality, research |

## Limitations

- **Not cryptographically secure** - Use for simulations, not security
- **Fixed period** - Period depends on grid size (typically 2^size generations)
- **Memory usage** - Requires grid_size² bytes of memory
- **Deterministic** - Same seed always produces same sequence

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## References

- [Rule 30 on Wikipedia](https://en.wikipedia.org/wiki/Rule_30)