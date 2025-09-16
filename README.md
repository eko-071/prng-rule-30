# PRNG based on Cellular Automata

A simple **Cellular Automaton-based Pseudo-Random Number Generator (PRNG)** implemented in C.
This program uses the **Rule-30** cellular automata to generate random numbers from a seed, with a visual grid representation of each iteration.

## Features

- Generates random numbers of arbitrary size (up to 64 bits).
- Visualises the evolution of the cellular automaton grid in the terminal.
- Allows customs seeds or uses the current system time as default.

## How to Run

### Linux

1. Clone the repository:

```shell
git clone https://github.com/fahad-ali-07/prng-rule-30.git
cd prng-rule-30
```

2. Compile using gcc:

```shell
gcc rule-30.c
```

3. Run:

```shell
./a.out
```

### Windows

1. Clone the repository

2. Compile using [MinGW](https://www.mingw-w64.org/) or some other GCC-based toolchain.

3. Run:

```shell
./a.exe
```

## Usage

1. Run the program
2. Enter the bit size of the number you want to generate (1-64).
3. Choose whether to enter a custom seed or use the default (system time).
4. Observe the cellular automaton grid evolving in the terminal
5. At the end, the random number generated will be displayed in both binary and decimal format.

## How It Works

The program first initializes a 2D grid of size (bit size * 2) x (bit size * 2). It then sets the first row to the seed value. Each subsequent row is then generated using the Rule 30 logic, which can be simplified to:
 ```
next_cell = left XOR (center OR right)
 ```
 Then from the center column, a random string of length bit size is extracted as the random number.

 ## Notes

 - The grid animation is optional; `sleep_screen()` and `clear_screen()` can be commented out for faster number generation.
 - The maximum size of the random number is 64 bits `(limited by uint64_t)`.
 - The program highlights the random number in the grid in red for clarity.