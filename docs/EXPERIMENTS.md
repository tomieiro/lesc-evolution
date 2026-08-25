# Experiment notes

## Minimum run record

Record the following for every comparable run:

- Git commit ID and local changes, if any
- Maze header and maze image
- `GE`, population size, and initial mutation rate entered in the interface
- Values in `const/macros.h`, especially path length and start/goal coordinates
- Random seed policy, run duration, outcome, and observed best fitness

The application seeds the C standard pseudo-random generator from the current time. Exact replay is therefore not available yet; record the source revision and input parameters to preserve the experiment context.

## Current limitations

- Results are rendered in the GUI and are not written to a machine-readable file.
- The evolutionary worker and GUI share state; runs should be treated as exploratory visualization, not as a deterministic measurement pipeline.
- The map, start location, goal location, and image are configured in separate source constants. Verify they agree before comparing runs.

## Next research improvements

Prioritize a command-line experiment mode with explicit seeds and structured result files. That would permit repeated trials, statistical summaries, and automated regression checks without changing the interactive visualization workflow.
