# openmp-parallel-computing
A collection of OpenMP-parallelized C/C++ projects: image box blur filter, parallel treasure hunt simulation, and Conway's Game of Life. Includes serial vs. parallel comparisons, scheduling strategies, dynamic thread handling, and performance analysis.


---

## 🔍 Projects Included

### 1. Box Blur Filter (Image Processing)
- Applies a 3×3 kernel box blur to grayscale images.
- Parallelized using OpenMP `parallel for`.
- Compares serial and parallel execution times across image sizes (1000×1000 to 5000×5000).
- Handles image boundaries accurately.

### 2. Parallel Treasure Hunt (Simulation)
- N×N grid with treasures, traps, dynamic barriers, and adventurers (threads).
- Threads accumulate or lose points based on discoveries.
- Supports random adventurer spawning, termination, dynamic barriers, and winner selection.
- Synchronization via OpenMP and dynamic thread behavior.

### 3. Conway's Game of Life
- Simulates life on a 100×100 toroidal grid for 100 generations.
- Includes:
  - Serial version
  - Parallel version (row-wise update)
  - Static scheduling
  - Guided scheduling
- Measures performance and computes speedups.
- Includes a report and optional graphical visualization.

