# Maze Generator + BFS Solver in C

An animated maze generator using recursive-backtracker carving, followed by a breadth-first search solver that traces the shortest path. Rendered as coloured blocks via SDL2.

Watch the maze being dug cell by cell, then see the solver find its way from the top-left corner to the bottom-right.

---

# Features

- Recursive-backtracker maze generation (randomised depth-first search)
- BFS shortest-path solver from start to exit
- Animated carving — each wall removal rendered live
- Animated solution trace — path revealed step by step
- True 24‑bit color rendering via SDL2 framebuffer
- 790 × 390 window (79 × 39 grid, 10 px per cell)
- Five-color palette: walls, paths, solution, start, end
- Keyboard and Ctrl-C quit handling
- Written entirely in C

---

# How It Works

The grid starts entirely filled with walls (`#`). A recursive backtracker picks a random direction at each cell, carves a passage by removing the wall between the current and next cell, and recurses — this produces a perfect maze (every cell reachable from every other, exactly one path between any two).

Once the maze is fully carved, BFS explores from the start cell outward in all four directions, marking each visited cell with its predecessor. When the exit is reached, the path is traced back to the start and rendered in green.

---

# Tutorial / Algorithms

## 1. The Grid

The maze is stored as a 2‑D `char` grid where `CW × CH` cells are surrounded by walls:

```c
#define CW     39          /* cells wide */
#define CH     19          /* cells high */
#define W      (CW * 2 + 1)
#define H      (CH * 2 + 1)

static char g[H][W];
```

Grid cells at odd coordinates `(cy*2+1, cx*2+1)` are passages; even coordinates are potential walls. The full grid is 79 × 39.

---

## 2. Recursive-Backtracker Carving

The `carve` function picks a random unvisited neighbour, removes the wall between them, and recurses:

```c
static void carve(int cx, int cy) {
    g[cy * 2 + 1][cx * 2 + 1] = ' ';

    int dirs[4] = {0, 1, 2, 3};
    /* Fisher-Yates shuffle */
    for (int i = 3; i > 0; i--) {
        int j = rand() % (i + 1);
        int t = dirs[i]; dirs[i] = dirs[j]; dirs[j] = t;
    }

    for (int d = 0; d < 4; d++) {
        int nx = cx + dx[dirs[d]], ny = cy + dy[dirs[d]];
        if (out of bounds) continue;
        if (g[ny*2+1][nx*2+1] == '#') {
            g[cy*2+1 + dy[dirs[d]]][cx*2+1 + dx[dirs[d]]] = ' ';  /* remove wall */
            render();
            SDL_Delay(4);
            carve(nx, ny);
        }
    }
}
```

The shuffle ensures a random maze each run. Each wall removal triggers a redraw so the carving animation is visible.

---

## 3. BFS Solver

After carving, BFS explores from the start `(1, 1)` to the exit `(W-2, H-2)`:

```c
static int prev[H * W];
static int q[H * W];
static char seen[H][W];

q[tail++] = sy * W + sx;
seen[sy][sx] = 1;
prev[sy * W + sx] = -1;

while (head < tail) {
    int cur = q[head++];
    int cx = cur % W, cy = cur / W;
    if (cx == ex && cy == ey) break;
    for (each neighbour) {
        if (not a wall and not seen) {
            seen[ny][nx] = 1;
            prev[ny * W + nx] = cur;
            q[tail++] = ny * W + nx;
        }
    }
}
```

BFS guarantees the shortest path in an unweighted grid. The `prev` array stores each cell's predecessor, forming a linked list from the exit back to the start.

---

## 4. Solution Tracing

The solution path is reconstructed by walking `prev` backwards:

```c
int cur = ey * W + ex;
while (cur != -1) {
    int cx = cur % W, cy = cur / W;
    if (g[cy][cx] == ' ') g[cy][cx] = '.';
    render();
    SDL_Delay(8);
    cur = prev[cur];
}
g[sy][sx] = 'S';
g[ey][ex] = 'E';
render();
```

Each step is drawn live, showing the path being traced from exit to entrance.

---

## 5. Color Palette

| Element | Character | RGB         | Description        |
|---------|-----------|-------------|--------------------|
| Wall    | `#`       | 40, 40, 60  | Dark navy          |
| Path    | ` `       | 220, 200, 170 | Warm sandstone   |
| Solution| `.`       | 80, 255, 80 | Bright green       |
| Start   | `S`       | 60, 120, 255| Blue               |
| Exit    | `E`       | 255, 200, 50| Gold               |

---

## 6. Cell Rendering

Each grid cell is a 10 × 10 pixel block:

```c
for (int py = 0; py < CELL; py++)
    for (int px = 0; px < CELL; px++)
        pixels[(y * CELL + py) * pitch + (x * CELL + px)] = color;
```

---

# Build

```
git clone <this-repo>
cd maze
make
```

Or manually:

```
gcc maze.c -o maze $(sdl2-config --cflags --libs)
```

**Dependencies:** SDL2 and a C compiler (`gcc`/`clang`).

Install SDL2 via Homebrew:

```
brew install sdl2
```

Or apt:

```
sudo apt install libsdl2-dev
```

---

# Run

```
./maze
```

The program runs automatically: watch the maze being carved, then the solution being traced. After completion, the window stays open — press **ESC** or **Q** to quit.

---

# Customizing

Edit constants at the top of `maze.c`:

- `CW`, `CH` — maze cell count (larger = bigger maze)
- `CELL` — pixel size per grid cell
- Carve delay (`4` ms) and trace delay (`8` ms)
- Colour values for walls, paths, solution, start, exit

---

# Concepts Practiced

- Recursive backtracker maze generation (randomised DFS)
- Fisher-Yates shuffle for unbiased direction selection
- Breadth-first search on a grid for shortest path
- BFS predecessor array for path reconstruction
- Character-grid to pixel-block rendering
- SDL2 surface/pixel-buffer graphics
- Real-time animation with per-step redraws

---

# Dependencies

- `SDL.h` — window management and pixel buffer
- `stdio.h` — stderr diagnostics
- `stdlib.h` — `rand`, `srand`, `NULL`
- `string.h` — `memset`
- `time.h` — `time` for RNG seed
- `signal.h` — `SIGINT` handler
