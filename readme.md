# Maze

Animated maze generator and BFS solver, in C.

A small, self-contained demo written in **pure C** — no external libraries,
just the standard library and POSIX. Part of the [Corg-Labs](https://github.com/Corg-Labs)
collection of single-file C programs.

---

## How It Works

1. A recursive backtracker carves the maze, animated live
2. Walls are removed between visited cells as it digs
3. Once built, breadth-first search finds the shortest path
4. The solution is traced back from exit to entrance with '.'

---

## Build

```
gcc maze.c -o maze
```

## Run

```
./maze
```

## Controls

Runs automatically, then exits when solved.
