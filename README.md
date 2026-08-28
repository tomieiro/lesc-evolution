# LESC Evolution

LESC Evolution is a C++ research prototype for studying evolutionary search in a two-dimensional maze. A population of agents incrementally evolves movement sequences while an FLTK interface renders the maze, population, and progress indicators.

The repository is intended as a transparent starting point for experiments on search heuristics, mutation schedules, fitness functions, and maze topology. It is not a benchmark suite and does not make performance claims.

## Research origin

This project originated alongside neuroscience research into whether cockroaches could be directed with electronic implants for navigation tasks. Potential applications included search-and-rescue scenarios, maze traversal, and exploration in constrained environments. LESC Evolution abstracts that motivation into a software-only maze: its cockroach agents are simulated search entities, and the repository does not control or model biological organisms or implants.

## Research model

Each entity starts at the configured initial cell and receives a finite sequence of directions. During an iteration, invalid moves and wall collisions terminate that entity's run. Surviving paths are compared using a directional progress heuristic and weighted Euclidean distance to the goal. The currently best path is copied into the next population with bounded directional mutations.

The active experiment constants are defined in [`const/macros.h`](const/macros.h): maze dimensions, starting and ending positions, maximum path length, timing, and the displayed maze image. The active maze is [`const/matrix.h`](const/matrix.h); `matrix2.h` and `matrixbase.h` are alternative maps for manual experiments.

## Requirements

- A C++14 compiler and GNU Make
- FLTK 1.3 development headers and libraries
- X11 development libraries on Linux

On Debian or Ubuntu, install the system packages with:

```shell
sudo apt install build-essential libfltk1.3-dev libfltk-images1.3-dev libx11-dev
```

On macOS, install FLTK through Homebrew and make sure its headers and libraries are discoverable by your compiler:

```shell
brew install fltk
```

## Build and run

```shell
make
make run
```

`make` creates `release/lesc-evolution`. Generated files in `bin/` and `release/` are intentionally ignored by Git. To remove the generated executable, run `make clean`.

## Running an experiment

1. Build and launch the application.
2. Set the genetic mixing factor, population size, initial mutation rate, and the number of stagnant generations before the search rotates.
3. Select **START** and observe the displayed fitness and best-path progress. When an agent reaches the goal, the application keeps the winning route visible and replays it continuously.
4. Record the maze, constants, input values, source revision, run duration, and outcome outside the application. The current prototype does not persist experiment results.

See [`docs/EXPERIMENTS.md`](docs/EXPERIMENTS.md) for a minimal reproducibility record and known limitations.

## Maze configuration

Maze cells use the following values:

| Value | Meaning |
| --- | --- |
| `0` | Traversable cell |
| `1` | Wall |
| `2` | Goal |

All maze headers must define a `mapWidth` × `mapHeight` matrix and retain a valid start and goal. To use another maze, replace the include of `const/matrix.h` in `src/main/main.cpp` with the desired matrix header, update the image path in `MATRIX_IMG`, and supply a matching 400 × 400 pixel image.

## Repository layout

```text
const/             Maze definitions and experiment constants
docs/              Research workflow and limitations
headers/           Shared C++ and GUI declarations
img/               Maze images
project/           FLUID interface project file
src/evolution/     Population initialization, mutation, and evaluation
src/main/          Application entry point and FLTK interface
```

## Contributing

Keep source comments, documentation, and commit messages in English. Do not commit generated binaries, object files, or operating-system metadata. Changes that affect search behavior should describe the affected parameters and include a reproducible manual test or automated check.

## Authors

- Felipe Oliveira
- Matheus Tomieiro
- Victor Vieira Custodio Reis
- Yago Poletto

## License

This project is licensed under the [MIT License](LICENSE).
