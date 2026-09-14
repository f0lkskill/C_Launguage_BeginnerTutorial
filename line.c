#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include<stdbool.h>
#include<windows.h>

#define size 25

typedef struct{
    bool u;
    bool r;
    bool d;
    bool l;
} Tile;

int randint(int a, int b){
    return rand()%(b-a+1)+a;
}

Tile map[size][size];

void delay(int ms){
    Sleep(ms);
}

void inittile(Tile *t){
    t->u=false;
    t->r=false;
    t->d=false;
    t->l=false;
}

char (*pmtile(Tile t))[3] {
    static char out[3][3];
    char c = '&';
    char a = ' ';

    out[0][0] = c;
    out[0][1] = t.u ? a : c;
    out[0][2] = c;

    out[1][0] = t.l ? a : c;
    if (t.u || t.d || t.l || t.r) out[1][1] = a;
    else out[1][1] = c;
    out[1][2] = t.r ? a : c;

    out[2][0] = c;
    out[2][1] = t.d ? a : c;
    out[2][2] = c;

    return out;
}

void initmap(){
    for(int y=0;y<size;y++)
        for(int x=0;x<size;x++){
            Tile newtile;
            inittile(&newtile);
            map[y][x]=newtile;
        }
}

void printmap(){
    for(int y = 0; y < size; y++){
        for(int row = 0; row < 3; row++){
            for(int x = 0; x < size; x++){
                char (*t)[3] = pmtile(map[y][x]);
                for(int col = 0; col < 3; col++)
                    printf(" %c ",t[row][col]);
            }
            putchar('\n');
        }
    }
}

bool checkpos(int x, int y){
    if (0<=x&&x<size && 0<=y&&y<size) return true;
    return false;
}

bool checkdone(){
    for (int y = 0; y < size; y++){
        for (int x = 0; x < size; x++){
            // 不能有一个格子是完全不开放的
            if (!(map[y][x].u || map[y][x].r || map[y][x].d || map[y][x].l)) return false;
        }
    }
    return true;
}

// 纯随机 + 撞死路回退的试错算法
bool randmap(){
    int ex = size-1, ey = size-1;

    int mempos[1000][2];   // 路径栈：走过的格子 (y, x)
    int tried[1000];       // 每格已试方向 bitmask
    int memindex = 0;      // 路径长度，只增不减
    int cur = 0;           // 当前在路径栈中的位置

    int loop[2] = {1000000, 0};

    // 方向表：0上 1右 2下 3左
    int dx[4] = {0, 1, 0, -1};
    int dy[4] = {-1, 0, 1, 0};
    int bit[4] = {1, 2, 4, 8};

    // 起点入栈
    mempos[memindex][0] = 0;
    mempos[memindex][1] = 0;
    tried[memindex] = 0;
    memindex++;
    cur = 0;

    int nx = 0, ny = 0;
    bool success_flag = false;

    while (!(success_flag && checkdone())){
        if (loop[1] == loop[0]) {
            printf("loop limit reached\n");
            return false;
        }
        loop[1]++;

        if (nx==ex && ny==ey)
            success_flag = true;

        // === 随机挑一个没试过的 方向 ===
        // 先收集所有没试过的方向
        int cand[4];
        int candn = 0;
        for (int d = 0; d < 4; d++){
            if (!(tried[cur] & bit[d])) cand[candn++] = d;
        }

        if (candn == 0){
            // 四个方向都试过 → 死路，回退
            if (cur == 0){
                printf("no solution, back to start\n");
                return false;
            }
            cur--;
            nx = mempos[cur][1];
            ny = mempos[cur][0];

            printf("backtrack to (y=%d,x=%d)\n", ny, nx);
            // delay(100);
            // system("cls");
            // printmap();
            printf("\n");
            // delay(100);
            continue;
        }

        // 从没试过的方向里随机挑一个
        int d = cand[randint(0, candn-1)];
        tried[cur] |= bit[d];

        int tx = nx + dx[d];
        int ty = ny + dy[d];

        // 无效位置或已访问 → 当作这个方向失败，回去重新随机
        if (!checkpos(tx, ty)) continue;

        bool visited = false;
        for (int i = 0; i < memindex; i++){
            if (mempos[i][0] == ty && mempos[i][1] == tx){
                visited = true;
                break;
            }
        }
        if (visited) continue;

        // 打通墙
        if (d == 0){ map[ny][nx].u = true; map[ny-1][nx].d = true; }
        if (d == 1){ map[ny][nx].r = true; map[ny][nx+1].l = true; }
        if (d == 2){ map[ny][nx].d = true; map[ny+1][nx].u = true; }
        if (d == 3){ map[ny][nx].l = true; map[ny][nx-1].r = true; }

        // 前进
        nx = tx;
        ny = ty;

        // 新格子入栈
        if (memindex < 1000){
            mempos[memindex][0] = ny;
            mempos[memindex][1] = nx;
            tried[memindex] = 0;
            memindex++;
            cur = memindex - 1;
        }

        printf("move to (y=%d,x=%d)\n", ny, nx);
        // delay(1);
        // system("cls");
        // printmap();
        printf("checkdone: %d\n", checkdone());
        printf("success_flag: %d\n", success_flag);
        printf("\n");
    }

    return true;
}

int main()
{
    srand(time(NULL));

    int attempt = 0;
    while (1){
        attempt++;
        printf("=== attempt %d ===\n", attempt);

        initmap();
        if (randmap()){
            printf("success on attempt %d\n", attempt);
            break;
        }
        printf("failed on attempt %d, retrying...\n", attempt);
        // delay(300);
        system("cls");
    }

    printf("\n=== final map ===\n");
    printmap();
    system("pause");
    return 0;
}