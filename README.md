# prng30

A pseudo-random number generator based on Wolfram's Rule 30 elementary
cellular automaton, implemented as a portable C99 library.

> **Not cryptographically secure.** Use a CSPRNG for anything security-sensitive.

---

## Building

**Requirements:** CMake >= 3.12, a C99 compiler, GNU make.

```bash
git clone https://github.com/eko-071/prng-rule-30.git
cd prng-rule-30
mkdir build && cd build
cmake ..
make
```

This produces three binaries inside `build/`:

| Binary | Description |
|---|---|
| `tests` | Test suite |
| `example` | Usage examples |
| `prng30_visualizer` | Animated terminal display of the CA |

And one library: `libprng30.a` (static) or `libprng30.so` (shared).

### CMake options

```bash
cmake .. -DBUILD_SHARED_LIBS=ON        # build shared library instead of static
cmake .. -DBUILD_EXAMPLES=OFF          # skip example binary
cmake .. -DBUILD_VISUALIZER=OFF        # skip visualizer binary
cmake .. -DENABLE_SANITIZERS=ON        # enable ASan + UBSan (use with Debug)
```

---

## Running the tests

```bash
cd build
./tests
```

Or through CTest:

```bash
ctest --output-on-failure
```

Each test prints a pass/fail result with its measured statistic. The suite
covers correctness (determinism, error codes, edge cases) and statistical
quality (monobit, chi-squared, runs, autocorrelation, birthday spacing).

---

## Running the visualizer

```bash
cd build
./prng30_visualizer              # random time-based seed, width=64
./prng30_visualizer 12345        # fixed seed, width=64
./prng30_visualizer 12345 40     # fixed seed, width=40
```

Width must be between 32 and 4096. The centre column (the extraction point)
is highlighted in red.

---

## Using the library

### In your own CMake project

Copy `include/prng30.h` and `src/prng.c` directly into your project, or
build and link against `libprng30`:

```cmake
target_link_libraries(your_target PRIVATE prng30)
```

### Compiling manually

```bash
# From the repo root:
gcc -Iinclude your_program.c src/prng.c -o your_program
```

### Basic usage

```c
#include "prng30.h"
#include <stdio.h>

int main(void) {
    prng30_state st;

    if (prng30_init(&st, 12345, 64) != PRNG30_OK)
        return 1;

    // 32-bit integer
    uint64_t n = prng30_generate(&st, 32);

    // uniform double in [0, 1) 
    double d = prng30_generate_double(&st);

    prng30_free(&st);
    return 0;
}
```

### Choosing a width

The `width` parameter controls the number of cells in the automaton.
Wider grids have a longer period and better statistical properties but
use more memory (2 × width bytes) and take longer to initialise.

| Width | Memory | Notes |
|---|---|---|
| 32 | 64 B | minimum; short period |
| 64 | 128 B | good default for most uses |
| 128 | 256 B | better quality, still fast |
| 256+ | 512 B+ | for high-volume generation |

### Error handling

`prng30_init` returns an error code, always check it:

```c
prng30_err err = prng30_init(&st, seed, width);
if (err != PRNG30_OK) {
    // PRNG30_ERR_NULL: st was NULL
    // PRNG30_ERR_ALLOC: malloc failed
    // PRNG30_ERR_BADWIDTH: width outside [32, 4096]
    return 1;
}
```

`prng30_free` is always safe to call, even after a failed init.

---

## API reference

```c
prng30_err prng30_init(prng30_state *st, uint64_t seed, int width);
```
Initialise the automaton. All seed values including `0` and `UINT64_MAX`
are valid. Returns `PRNG30_OK` on success.

```c
void prng30_free(prng30_state *st);
```
Release memory. Safe on a zeroed or already-freed state.

```c
uint64_t prng30_generate(prng30_state *st, int nbits);
```
Generate a random integer using `nbits` bits of output [1..64].
`nbits` is silently clamped if outside this range.

```c
double prng30_generate_double(prng30_state *st);
```
Generate a uniform double in [0, 1) using 53 bits of entropy.

```c
void prng30_step(prng30_state *st);
```
Advance the automaton by one generation. Called internally by
`prng30_generate`; exposed for direct CA experiments.

---

## How it works

Rule 30 is an elementary cellular automaton. Each cell's next state is
determined by its current neighbourhood of three cells:

```
neighbours:  111  110  101  100  011  010  001  000
next cell:    0    0    0    1    1    1    1    0
```

Closed form: `next = left XOR (mid OR right)`. Binary `00011110` = 30.

The automaton is initialised from the seed bits directly (cells 0–63),
with `splitmix64` for wider grids. It is warmed up for `width/2` steps to
ensure full diffusion before extraction begins. Each call to
`prng30_generate` steps the automaton once per output bit, extracting
three cell positions (centre, centre ± width/8) and XOR-ing them together.

---

## Project structure

```
prng-rule-30/
├── include/prng30.h          public API
├── src/prng.c                core library
├── visualizer/visualizer.c   terminal visualizer (standalone binary)
├── examples/example.c        usage examples
├── tests/
│   ├── framework.h           test utilities
│   ├── main.c                test runner
│   ├── test_core.c           correctness tests
│   ├── test_statistical.c    statistical quality tests
│   └── test_double.c         floating-point tests
├── .clang-format             code style config
├── CMakeLists.txt
└── LICENSE
```

---

## Statistical tests

| Test | Reference |
|---|---|
| Monobit frequency | NIST SP 800-22, Test 1 |
| Chi-squared uniformity | 10 bins, 10 000 samples |
| Bit distribution | all 64 bit positions ≈ 50% |
| Runs test | Knuth TAOCP §3.3.2 |
| Lag-1 autocorrelation | Pearson r |
| Birthday spacing | 500 × 32-bit samples |

---

## License

MIT — see [LICENSE](LICENSE).