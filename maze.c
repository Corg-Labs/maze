/*
 * maze.c - Maze generator + BFS solver (SDL2)
 *
 * Recursive-backtracker carves the maze, then breadth-first
 * search finds the shortest path from top-left to bottom-right,
 * traced step by step.  Rendered as coloured blocks via SDL2.
 *
 * Build:
 *   gcc maze.c -o maze $(sdl2-config --cflags --libs)
 * Run:
 *   ./maze
 *
 * Controls:
 *   ESC or Q  - quit
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <signal.h>
#include <SDL.h>

#define CW     39
#define CH     19
#define W      (CW * 2 + 1)
#define H      (CH * 2 + 1)
#define CELL   10
#define WIN_W  (W * CELL)
#define WIN_H  (H * CELL)

static volatile int running = 1;
static void handle_sigint(int s) { (void)s; running = 0; }

static SDL_Window *win;
static SDL_Surface *surf;
static SDL_PixelFormat *fmt;
static int pitch;
static char g[H][W];

static Uint32 col_wall, col_path, col_sol, col_start, col_end;

static void render(void)
{
    if (!running) return;

    SDL_Event ev;
    while (SDL_PollEvent(&ev)) {
        if (ev.type == SDL_QUIT) running = 0;
        if (ev.type == SDL_KEYDOWN &&
            (ev.key.keysym.sym == SDLK_ESCAPE ||
             ev.key.keysym.sym == SDLK_q))
            running = 0;
    }

    if (SDL_MUSTLOCK(surf))
        SDL_LockSurface(surf);

    Uint32 *pixels = (Uint32 *)surf->pixels;

    for (int y = 0; y < H; y++) {
        for (int x = 0; x < W; x++) {
            Uint32 color;
            switch (g[y][x]) {
                case '#': color = col_wall;  break;
                case '.': color = col_sol;   break;
                case 'S': color = col_start; break;
                case 'E': color = col_end;   break;
                default:  color = col_path;  break;
            }
            for (int py = 0; py < CELL; py++)
                for (int px = 0; px < CELL; px++)
                    pixels[(y * CELL + py) * pitch + (x * CELL + px)] = color;
        }
    }

    if (SDL_MUSTLOCK(surf))
        SDL_UnlockSurface(surf);

    SDL_UpdateWindowSurface(win);
}

static void carve(int cx, int cy)
{
    if (!running) return;
    g[cy * 2 + 1][cx * 2 + 1] = ' ';

    int dirs[4] = {0, 1, 2, 3};
    for (int i = 3; i > 0; i--) {
        int j = rand() % (i + 1);
        int t = dirs[i]; dirs[i] = dirs[j]; dirs[j] = t;
    }

    int dx[4] = {0, 0, -1, 1};
    int dy[4] = {-1, 1, 0, 0};

    for (int d = 0; d < 4; d++) {
        int nx = cx + dx[dirs[d]];
        int ny = cy + dy[dirs[d]];
        if (nx < 0 || ny < 0 || nx >= CW || ny >= CH) continue;
        if (g[ny * 2 + 1][nx * 2 + 1] == '#') {
            g[cy * 2 + 1 + dy[dirs[d]]][cx * 2 + 1 + dx[dirs[d]]] = ' ';
            render();
            SDL_Delay(4);
            carve(nx, ny);
            if (!running) return;
        }
    }
}

int main(void)
{
    signal(SIGINT, handle_sigint);
    srand((unsigned)time(NULL));

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        fprintf(stderr, "SDL_Init: %s\n", SDL_GetError());
        return 1;
    }

    win = SDL_CreateWindow(
        "Maze Generator + BFS Solver",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        WIN_W, WIN_H, SDL_WINDOW_SHOWN);
    if (!win) {
        fprintf(stderr, "SDL_CreateWindow: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    surf = SDL_GetWindowSurface(win);
    fmt = surf->format;
    pitch = surf->pitch / 4;

    col_wall = SDL_MapRGB(fmt, 40,  40,  60);
    col_path = SDL_MapRGB(fmt, 220, 200, 170);
    col_sol  = SDL_MapRGB(fmt, 80,  255, 80);
    col_start = SDL_MapRGB(fmt, 60,  120, 255);
    col_end   = SDL_MapRGB(fmt, 255, 200, 50);

    memset(g, '#', sizeof(g));
    render();

    carve(0, 0);

    /* BFS solve */
    int sx = 1, sy = 1, ex = W - 2, ey = H - 2;
    static int prev[H * W];
    static int q[H * W];
    static char seen[H][W];
    memset(seen, 0, sizeof(seen));

    int head = 0, tail = 0;
    q[tail++] = sy * W + sx;
    seen[sy][sx] = 1;
    prev[sy * W + sx] = -1;

    int dx[4] = {0, 0, -1, 1};
    int dy[4] = {-1, 1, 0, 0};

    while (head < tail && running) {
        int cur = q[head++];
        int cx = cur % W, cy = cur / W;
        if (cx == ex && cy == ey) break;
        for (int d = 0; d < 4; d++) {
            int nx = cx + dx[d], ny = cy + dy[d];
            if (nx < 0 || ny < 0 || nx >= W || ny >= H) continue;
            if (g[ny][nx] != '#' && !seen[ny][nx]) {
                seen[ny][nx] = 1;
                prev[ny * W + nx] = cur;
                q[tail++] = ny * W + nx;
            }
        }
    }

    /* Trace solution */
    int cur = ey * W + ex;
    while (cur != -1 && running) {
        int cx = cur % W, cy = cur / W;
        if (g[cy][cx] == ' ') g[cy][cx] = '.';
        render();
        SDL_Delay(8);
        cur = prev[cur];
    }

    g[sy][sx] = 'S';
    g[ey][ex] = 'E';
    render();

    /* Keep window open until user quits */
    while (running) {
        SDL_Event ev;
        while (SDL_PollEvent(&ev)) {
            if (ev.type == SDL_QUIT) running = 0;
            if (ev.type == SDL_KEYDOWN &&
                (ev.key.keysym.sym == SDLK_ESCAPE ||
                 ev.key.keysym.sym == SDLK_q))
                running = 0;
        }
        SDL_Delay(16);
    }

    SDL_DestroyWindow(win);
    SDL_Quit();
    return 0;
}
