# Maze Generator + A* Pathfinder in C

A terminal-based, full-color visualizer of two classic graph algorithms in one program — written entirely in C.

**Phase 1** carves a fresh perfect maze with the recursive-backtracker algorithm, animating every wall as it falls. **Phase 2** then runs A* search from the top-left to the bottom-right, showing you exactly which cells the algorithm visits, which cells are on the frontier, and which cells make up the final shortest path.

No graphics library. No GPU. Just stacks, priority queues, and stdout.

This project focuses on learning:

- depth-first maze generation (recursive backtracker)
- the A* search algorithm
- graph traversal in 2D grids
- open / closed / frontier sets
- admissible heuristics (Manhattan distance)
- ANSI 256-color terminal rendering
- real-time algorithm visualization in C

---

# Features

- Two algorithms in one program:
  - **Iterative recursive backtracker** for maze generation
  - **A\* search** for the shortest path
- Color-coded cell states:
  - **Dark gray** - wall
  - **Light gray** - open passage
  - **Yellow** - carving frontier / A\* frontier
  - **Dark blue** - A\* explored set
  - **Bright green** - final shortest path
  - **Cyan** - start
  - **Magenta** - goal
- Pure terminal rendering
- ANSI 256-color output
- Written entirely in C
- Standard libraries only

---

# How It Works

The world is a 2D grid `(2*MW + 1) x (2*MH + 1)`. Maze cells live on odd grid coordinates and walls live on even ones — so carving a passage means flipping the wall between two adjacent maze cells to *open*.

For every step:

- The current algorithm updates the grid
- The renderer paints every cell as a colored space using its state
- A short `usleep()` paces the animation

When the path is found (or no path exists), the program pauses for a beat and starts over with a brand new random maze.

---

# Tutorial / Visualization Pipeline

## 1. Grid Encoding

A single 2D array of integers does everything:

```
int grid[GH][GW];   // each cell is one of: WALL, OPEN, EXPLORED,
                    // FRONTIER, PATH, START, GOAL, CARVING
```

State changes are just integer assignments — the renderer reads `grid` and maps each value to a color.

---

## 2. Maze Generation: Recursive Backtracker

This is the classic "perfect maze" algorithm — every cell is reachable from every other cell by exactly one path:

```
mark a random cell as visited and push it
while the stack is not empty:
    look at the top cell
    if it has unvisited neighbors:
        pick one at random
        carve the wall between current and neighbor
        push the neighbor
    else:
        pop (backtrack)
```

The stack does the work that recursion would; iterative form avoids deep call stacks for large mazes.

---

## 3. A\* Search

A\* finds the shortest path from start to goal by always expanding the most promising node first. "Most promising" is defined by:

```
f(n) = g(n) + h(n)
```

where:

- `g(n)` is the actual cost from start to `n`
- `h(n)` is the *heuristic* estimate from `n` to goal

We use **Manhattan distance** as the heuristic, which is admissible (never overestimates) for a 4-connected grid, so A\* is guaranteed to find the shortest path.

```
push start onto the open set
while open set is not empty:
    pick the node with the lowest f
    if it's the goal: reconstruct path and return
    move it from open to closed
    for each walkable neighbor:
        new_g = g(current) + 1
        if new_g < g(neighbor): update parent + g + f, put back in open set
```

---

## 4. Open vs. Closed Sets

The visualizer paints cells in the open set (the *frontier*) yellow, and cells in the closed set (the *explored* set) dark blue. Watching these regions grow over the maze is how you can literally *see* A\* prioritizing the most promising direction first.

---

## 5. Path Reconstruction

When the goal is popped, we walk parent pointers backwards from goal to start, painting each cell bright green. The path animates frame-by-frame so you can see the shortest route emerge through the maze.

---

## 6. ANSI 256-Color Output

Each cell is a colored space character (two characters wide for square-ish cells):

```
printf("\x1b[48;5;%dm  ", index);
```

Adjacent cells with the same color share one escape code to keep output fast.

---

# Build

Compile using:

```
gcc maze.c -o maze
```

Or just:

```
make
```

No math library needed.

---

# Run

```
./maze
```

Press `Ctrl+C` to exit.

For best results, run in a 256-color terminal at roughly 82 columns × 23 rows.

---

# Customizing

Edit the constants near the top of `maze.c`:

- `MW`, `MH` — maze size in *cells*. Grid will be `2*MW+1 x 2*MH+1`.
- Per-step `usleep` calls control animation speed
- The heuristic (`heuristic()`) is Manhattan distance; try chebyshev, diagonal, or weighted variants
- Try Wilson's algorithm or Prim's for different maze textures

---

# Concepts Practiced

- Depth-first traversal
- Iterative backtracking with explicit stack
- A* search algorithm
- Open/closed sets in pathfinding
- Admissible heuristics
- Parent-pointer path reconstruction
- 2D grid encoding
- Algorithm visualization
- ANSI 256-color rendering
- Real-time animation
- Terminal graphics

---

# Dependencies

Standard C libraries only:

- `stdio.h`
- `stdlib.h`
- `string.h`
- `unistd.h`
- `time.h`

No graphics engine required.

---

# Inspiration

A\* was first described in 1968 by Peter Hart, Nils Nilsson, and Bertram Raphael — and is still the dominant pathfinding algorithm in everything from video games to robotics half a century later.

Pair it with a maze generator and you get one of the most satisfying algorithm visualizations there is: chaos becoming structure, then structure becoming a single bright line from start to finish.
