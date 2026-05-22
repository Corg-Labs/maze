/*
 * maze.c - Animated maze generator + A* pathfinder in C
 *
 * Phase 1: carves a perfect maze using recursive backtracking
 *          (iterative DFS), animating each carve.
 * Phase 2: runs A* from the top-left to the bottom-right corner,
 *          animating the explored set, frontier, and final path.
 *
 * ANSI 256-color terminal output.
 *
 * Build:  gcc maze.c -o maze
 * Run:    ./maze
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

#define MW   20      /* maze cells wide  */
#define MH   11      /* maze cells tall  */
#define GW   (2 * MW + 1)
#define GH   (2 * MH + 1)

enum {
    WALL = 0,
    OPEN = 1,
    EXPLORED,
    FRONTIER,
    PATH,
    START,
    GOAL,
    CARVING
};

static int grid[GH][GW];

/* ---------- rendering ---------- */
static int color_for(int v) {
    switch (v) {
        case WALL:     return 234;     /* dark gray   */
        case OPEN:     return 252;     /* light gray  */
        case EXPLORED: return 24;      /* dark blue   */
        case FRONTIER: return 220;     /* yellow      */
        case PATH:     return 46;      /* bright green*/
        case START:    return 51;      /* cyan        */
        case GOAL:     return 201;     /* magenta     */
        case CARVING:  return 226;     /* yellow-hot  */
        default:       return 16;
    }
}

static void render(void) {
    printf("\x1b[H");
    char line[GW * 4 * 16 + 32];
    for (int r = 0; r < GH; r++) {
        int prev = -1;
        char *q = line;
        for (int c = 0; c < GW; c++) {
            int col = color_for(grid[r][c]);
            if (col != prev) {
                q += sprintf(q, "\x1b[48;5;%dm", col);
                prev = col;
            }
            *q++ = ' '; *q++ = ' ';
        }
        q += sprintf(q, "\x1b[0m\n");
        fwrite(line, 1, q - line, stdout);
    }
    fflush(stdout);
}

/* ---------- maze generation ---------- */
static void generate_maze(void) {
    /* Fill with walls */
    for (int r = 0; r < GH; r++)
        for (int c = 0; c < GW; c++)
            grid[r][c] = WALL;

    int stk[MW * MH][2];
    int top = 0;
    int visited[MH][MW];
    memset(visited, 0, sizeof visited);

    int sc = rand() % MW;
    int sr = rand() % MH;
    visited[sr][sc] = 1;
    grid[2 * sr + 1][2 * sc + 1] = OPEN;
    stk[top][0] = sc;
    stk[top][1] = sr;
    top++;

    const int dx[4] = { 0,  0, -1,  1};
    const int dy[4] = {-1,  1,  0,  0};

    while (top > 0) {
        int mc = stk[top - 1][0];
        int mr = stk[top - 1][1];

        int opts[4];
        int nopt = 0;
        for (int i = 0; i < 4; i++) {
            int nc = mc + dx[i];
            int nr = mr + dy[i];
            if (nc < 0 || nc >= MW || nr < 0 || nr >= MH) continue;
            if (visited[nr][nc]) continue;
            opts[nopt++] = i;
        }
        if (nopt == 0) {
            grid[2 * mr + 1][2 * mc + 1] = OPEN;   /* clear carving highlight */
            top--;
        } else {
            int d  = opts[rand() % nopt];
            int nc = mc + dx[d];
            int nr = mr + dy[d];
            grid[2 * mr + 1 + dy[d]][2 * mc + 1 + dx[d]] = OPEN;
            grid[2 * nr + 1][2 * nc + 1] = OPEN;
            grid[2 * mr + 1][2 * mc + 1] = CARVING;
            visited[nr][nc] = 1;
            stk[top][0] = nc;
            stk[top][1] = nr;
            top++;
            render();
            usleep(18000);
        }
    }
    /* Wipe any residual carving cells */
    for (int r = 0; r < GH; r++)
        for (int c = 0; c < GW; c++)
            if (grid[r][c] == CARVING) grid[r][c] = OPEN;
}

/* ---------- A* ---------- */
typedef struct {
    int r, c;
    int g;
    int f;
    int parent;
    int in_open;
} Node;

static Node  nodes[GH * GW];
static int   n_nodes;
static int   node_at[GH][GW];

static int abs_i(int x) { return x < 0 ? -x : x; }

static int heuristic(int r, int c, int gr, int gc) {
    return abs_i(r - gr) + abs_i(c - gc);
}

static int extract_min(void) {
    int best = -1, best_f = 1 << 30;
    for (int i = 0; i < n_nodes; i++) {
        if (!nodes[i].in_open) continue;
        if (nodes[i].f < best_f) { best_f = nodes[i].f; best = i; }
    }
    return best;
}

static void solve(int sr, int sc, int gr, int gc) {
    n_nodes = 0;
    for (int r = 0; r < GH; r++)
        for (int c = 0; c < GW; c++)
            node_at[r][c] = -1;

    nodes[n_nodes] = (Node){sr, sc, 0, heuristic(sr, sc, gr, gc), -1, 1};
    node_at[sr][sc] = n_nodes++;
    grid[sr][sc] = START;

    const int dx[4] = { 0,  0, -1,  1};
    const int dy[4] = {-1,  1,  0,  0};

    int found = -1;
    while (n_nodes > 0) {
        int ci = extract_min();
        if (ci < 0) break;
        Node cur = nodes[ci];
        nodes[ci].in_open = 0;

        if (cur.r == gr && cur.c == gc) { found = ci; break; }

        if (grid[cur.r][cur.c] != START)
            grid[cur.r][cur.c] = EXPLORED;
        render();
        usleep(4000);

        for (int d = 0; d < 4; d++) {
            int nr = cur.r + dy[d];
            int nc = cur.c + dx[d];
            if (nr < 0 || nr >= GH || nc < 0 || nc >= GW) continue;
            if (grid[nr][nc] == WALL) continue;
            int ng = cur.g + 1;
            int idx = node_at[nr][nc];
            if (idx >= 0) {
                if (nodes[idx].g <= ng) continue;
                nodes[idx].g = ng;
                nodes[idx].f = ng + heuristic(nr, nc, gr, gc);
                nodes[idx].parent = ci;
                if (!nodes[idx].in_open) {
                    nodes[idx].in_open = 1;
                    if (grid[nr][nc] != GOAL && grid[nr][nc] != START)
                        grid[nr][nc] = FRONTIER;
                }
            } else {
                nodes[n_nodes] = (Node){nr, nc, ng, ng + heuristic(nr, nc, gr, gc), ci, 1};
                node_at[nr][nc] = n_nodes++;
                if (grid[nr][nc] != GOAL && grid[nr][nc] != START)
                    grid[nr][nc] = FRONTIER;
            }
        }
    }

    /* Walk parents to paint the path */
    if (found >= 0) {
        int i = found;
        while (i >= 0) {
            int rr = nodes[i].r, cc = nodes[i].c;
            if      (rr == sr && cc == sc) grid[rr][cc] = START;
            else if (rr == gr && cc == gc) grid[rr][cc] = GOAL;
            else                            grid[rr][cc] = PATH;
            i = nodes[i].parent;
            render();
            usleep(20000);
        }
    }
}

int main(void) {
    srand((unsigned)time(NULL));
    printf("\x1b[?25l\x1b[2J");

    for (;;) {
        generate_maze();
        usleep(700000);

        int sr = 1, sc = 1;
        int gr = GH - 2, gc = GW - 2;
        grid[sr][sc] = START;
        grid[gr][gc] = GOAL;
        render();
        usleep(500000);

        solve(sr, sc, gr, gc);
        usleep(2500000);
    }
    return 0;
}
