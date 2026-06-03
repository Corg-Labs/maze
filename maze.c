/* maze.c - Maze generator (recursive backtracker) + BFS solver,
 * both animated in the terminal. Build: gcc maze.c -o maze */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#define CW 39          /* cells wide  */
#define CH 19          /* cells high  */
#define W (CW*2+1)
#define H (CH*2+1)

static char g[H][W];

static void draw(void){
    printf("\033[H");
    for (int y=0;y<H;y++){ fwrite(g[y],1,W,stdout); putchar('\n'); }
    fflush(stdout);
}

static void carve(int cx,int cy){
    g[cy*2+1][cx*2+1]=' ';
    int dirs[4]={0,1,2,3};
    for (int i=3;i>0;i--){ int j=rand()%(i+1); int t=dirs[i];dirs[i]=dirs[j];dirs[j]=t; }
    int dx[4]={0,0,-1,1}, dy[4]={-1,1,0,0};
    for (int d=0;d<4;d++){
        int nx=cx+dx[dirs[d]], ny=cy+dy[dirs[d]];
        if (nx<0||ny<0||nx>=CW||ny>=CH) continue;
        if (g[ny*2+1][nx*2+1]=='#'){
            g[cy*2+1+dy[dirs[d]]][cx*2+1+dx[dirs[d]]]=' ';
            draw(); usleep(4000);
            carve(nx,ny);
        }
    }
}

int main(void){
    srand((unsigned)time(NULL));
    memset(g,'#',sizeof(g));
    printf("\033[2J\033[?25l");
    carve(0,0);
    /* BFS solve from top-left to bottom-right */
    int sx=1,sy=1,ex=W-2,ey=H-2;
    static int prev[H*W];
    static int q[H*W]; int head=0,tail=0;
    static char seen[H][W]; memset(seen,0,sizeof(seen));
    q[tail++]=sy*W+sx; seen[sy][sx]=1; prev[sy*W+sx]=-1;
    int dx[4]={0,0,-1,1}, dy[4]={-1,1,0,0};
    while(head<tail){
        int cur=q[head++]; int cx=cur%W, cy=cur/W;
        if (cx==ex&&cy==ey) break;
        for(int d=0;d<4;d++){
            int nx=cx+dx[d], ny=cy+dy[d];
            if(nx<0||ny<0||nx>=W||ny>=H) continue;
            if(g[ny][nx]!='#'&&!seen[ny][nx]){ seen[ny][nx]=1; prev[ny*W+nx]=cur; q[tail++]=ny*W+nx; }
        }
    }
    int cur=ey*W+ex;
    while(cur!=-1){ int cx=cur%W,cy=cur/W; g[cy][cx]='.'; draw(); usleep(8000); cur=prev[cur]; }
    g[sy][sx]='S'; g[ey][ex]='E';
    draw();
    printf("\033[?25h\nMaze solved. Path marked with '.'\n");
    return 0;
}
