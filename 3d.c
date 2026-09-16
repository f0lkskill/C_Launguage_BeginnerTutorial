#include <stdio.h>
#include <math.h>
#include <string.h>
#include <unistd.h>

/* ================== 基础向量 ================== */
typedef struct { double x, y, z; } Vec3;

static Vec3 v_add(Vec3 a, Vec3 b) { Vec3 r = {a.x+b.x, a.y+b.y, a.z+b.z}; return r; }
static Vec3 v_sub(Vec3 a, Vec3 b) { Vec3 r = {a.x-b.x, a.y-b.y, a.z-b.z}; return r; }
static Vec3 v_mul(Vec3 a, double s) { Vec3 r = {a.x*s, a.y*s, a.z*s}; return r; }
static double v_dot(Vec3 a, Vec3 b) { return a.x*b.x + a.y*b.y + a.z*b.z; }
static Vec3 v_cross(Vec3 a, Vec3 b) {
    Vec3 r = { a.y*b.z - a.z*b.y,
               a.z*b.x - a.x*b.z,
               a.x*b.y - a.y*b.x };
    return r;
}
static double v_len(Vec3 a) { return sqrt(v_dot(a, a)); }
static Vec3 v_norm(Vec3 a) {
    double l = v_len(a);
    if (l < 1e-12) { Vec3 z = {0,0,0}; return z; }
    return v_mul(a, 1.0 / l);
}

/* ================== 旋转矩阵 ================== */
typedef struct {
    double m[3][3];
} Mat3;

static Mat3 mat_identity(void) {
    Mat3 r = {{{1,0,0},{0,1,0},{0,0,1}}};
    return r;
}

static Mat3 mat_mul(Mat3 a, Mat3 b) {
    Mat3 r;
    for (int i = 0; i < 3; i++)
        for (int j = 0; j < 3; j++) {
            double s = 0;
            for (int k = 0; k < 3; k++)
                s += a.m[i][k] * b.m[k][j];
            r.m[i][j] = s;
        }
    return r;
}

static Vec3 mat_apply(Mat3 m, Vec3 v) {
    Vec3 r;
    r.x = m.m[0][0]*v.x + m.m[0][1]*v.y + m.m[0][2]*v.z;
    r.y = m.m[1][0]*v.x + m.m[1][1]*v.y + m.m[1][2]*v.z;
    r.z = m.m[2][0]*v.x + m.m[2][1]*v.y + m.m[2][2]*v.z;
    return r;
}

static Mat3 mat_rot_x(double a) {
    double c = cos(a), s = sin(a);
    Mat3 r = {{{1,0,0},{0,c,-s},{0,s,c}}};
    return r;
}

static Mat3 mat_rot_y(double a) {
    double c = cos(a), s = sin(a);
    Mat3 r = {{{c,0,s},{0,1,0},{-s,0,c}}};
    return r;
}

static Mat3 mat_rot_z(double a) {
    double c = cos(a), s = sin(a);
    Mat3 r = {{{c,-s,0},{s,c,0},{0,0,1}}};
    return r;
}

/* ================== 画布 ================== */
#define CANVAS_W 200
#define CANVAS_H 100

typedef struct {
    int width;
    int height;
    char buf[CANVAS_H][CANVAS_W + 1];
    double zbuf[CANVAS_H][CANVAS_W];
} Canvas;

void canvas_clear(Canvas *c) {
    for (int y = 0; y < c->height; y++) {
        memset(c->buf[y], ' ', c->width);
        c->buf[y][c->width] = '\0';
        for (int x = 0; x < c->width; x++)
            c->zbuf[y][x] = 1e30;
    }
}

void canvas_plot(Canvas *c, int x, int y, double z, char ch) {
    if (x < 0 || x >= c->width || y < 0 || y >= c->height) return;
    if (z >= c->zbuf[y][x]) return;
    c->zbuf[y][x] = z;
    c->buf[y][x] = ch;
}

void canvas_line(Canvas *c, int x0, int y0, double z0,
                          int x1, int y1, double z1, char ch) {
    int dx = abs(x1 - x0), sx = x0 < x1 ? 1 : -1;
    int dy = -abs(y1 - y0), sy = y0 < y1 ? 1 : -1;
    int err = dx + dy;
    int steps = dx > -dy ? dx : -dy;
    if (steps == 0) steps = 1;
    int i = 0;
    for (;;) {
        double t = (double)i / steps;
        double z = z0 + (z1 - z0) * t;
        canvas_plot(c, x0, y0, z, ch);
        if (x0 == x1 && y0 == y1) break;
        int e2 = 2 * err;
        if (e2 >= dy) { err += dy; x0 += sx; }
        if (e2 <= dx) { err += dx; y0 += sy; }
        i++;
    }
}

void canvas_render(Canvas *c) {
    printf("\x1b[H");
    for (int y = 0; y < c->height; y++)
        printf("%s\n", c->buf[y]);
}

/* ================== 摄像机 ================== */
typedef struct {
    Vec3 pos;
    Vec3 forward;
    Vec3 up;
    double fov_scale;
} Camera;

static void camera_basis(Camera cam, Vec3 *right, Vec3 *up, Vec3 *fwd) {
    *fwd   = v_norm(cam.forward);
    *right = v_norm(v_cross(*fwd, cam.up));
    *up    = v_cross(*right, *fwd);
}

int camera_project(Camera cam, Vec3 p,
                   int screen_w, int screen_h,
                   int *out_x, int *out_y, double *out_z) {
    Vec3 right, up, fwd;
    camera_basis(cam, &right, &up, &fwd);

    Vec3 rel = v_sub(p, cam.pos);
    double z = v_dot(rel, fwd);
    if (z < 0.05) return 0;

    double x = v_dot(rel, right);
    double y = v_dot(rel, up);

    double sx = (x / z) * cam.fov_scale;
    double sy = (y / z) * cam.fov_scale;

    *out_x = (int)(screen_w * 0.5 + sx);
    *out_y = (int)(screen_h * 0.5 - sy);
    *out_z = z;
    return 1;
}

/* ================== 立方体 ================== */
typedef struct {
    Vec3 center;
    double half;
    char ch;
} Cube;

/*
   顶点编号（底面逆时针，顶面一一对应）:

        7 -------- 6
       /|         /|
      / |        / |
     4 -------- 5  |
     |  |       |  |
     |  3 ------|--2
     | /        | /
     |/         |/
     0 -------- 1
*/
static void cube_vertices(double half, Vec3 out[8]) {
    /* 底面 z = -half */
    out[0] = (Vec3){ -half, -half, -half };
    out[1] = (Vec3){  half, -half, -half };
    out[2] = (Vec3){  half,  half, -half };
    out[3] = (Vec3){ -half,  half, -half };
    /* 顶面 z = +half */
    out[4] = (Vec3){ -half, -half,  half };
    out[5] = (Vec3){  half, -half,  half };
    out[6] = (Vec3){  half,  half,  half };
    out[7] = (Vec3){ -half,  half,  half };
}

static const int CUBE_EDGES[12][2] = {
    /* 底面 4 条 */
    {0,1},{1,2},{2,3},{3,0},
    /* 顶面 4 条 */
    {4,5},{5,6},{6,7},{7,4},
    /* 竖直 4 条 */
    {0,4},{1,5},{2,6},{3,7}
};

/* ================== 场景 ================== */
#define MAX_CUBES 16

typedef struct {
    Cube cubes[MAX_CUBES];
    int count;
} Scene;

static void draw_cube(Canvas *canvas, Camera cam, Cube cube, Mat3 rot) {
    Vec3 local[8];
    cube_vertices(cube.half, local);

    int px[8], py[8];
    double pz[8];
    int ok[8];

    for (int i = 0; i < 8; i++) {
        Vec3 world = v_add(mat_apply(rot, local[i]), cube.center);
        ok[i] = camera_project(cam, world,
                               canvas->width, canvas->height,
                               &px[i], &py[i], &pz[i]);
    }

    for (int e = 0; e < 12; e++) {
        int a = CUBE_EDGES[e][0];
        int b = CUBE_EDGES[e][1];
        if (!ok[a] || !ok[b]) continue;
        canvas_line(canvas, px[a], py[a], pz[a],
                            px[b], py[b], pz[b], cube.ch);
    }
}

static void render_scene(Canvas *canvas, Camera cam, Scene scene, Mat3 rot) {
    canvas_clear(canvas);
    for (int i = 0; i < scene.count; i++)
        draw_cube(canvas, cam, scene.cubes[i], rot);
    canvas_render(canvas);
}

/* ================== 主循环 ================== */
int main(void) {
    Canvas canvas;
    canvas.width  = CANVAS_W;
    canvas.height = CANVAS_H;

    Camera cam;
    cam.pos       = (Vec3){ 0.0, 3.0, 14.0 };
    cam.forward   = v_norm((Vec3){ 0.0, -0.15, -1.0 });
    cam.up        = (Vec3){ 0.0, 1.0, 0.0 };
    cam.fov_scale = 200.0;

    Scene scene;
    scene.count = 0;

    /* 中心大立方体 */
    scene.cubes[scene.count++] = (Cube){ { 0.0,  0.0,  0.0}, 1.6, '#' };
    /* 六个小立方体围绕中心 */
    scene.cubes[scene.count++] = (Cube){ { 3.0,  0.0,  0.0}, 0.9, 'O' };
    scene.cubes[scene.count++] = (Cube){ {-3.0,  0.0,  0.0}, 0.9, 'O' };
    scene.cubes[scene.count++] = (Cube){ { 0.0,  3.0,  0.0}, 0.9, '+' };
    scene.cubes[scene.count++] = (Cube){ { 0.0, -3.0,  0.0}, 0.9, '+' };
    scene.cubes[scene.count++] = (Cube){ { 0.0,  0.0,  3.0}, 0.9, '*' };
    scene.cubes[scene.count++] = (Cube){ { 0.0,  0.0, -3.0}, 0.9, '*' };

    printf("\x1b[2J\x1b[?25l");

    double ax = 0.0, ay = 0.0, az = 0.0;

    for (int frame = 0; frame < 600; frame++) {
        ax += 0.025;
        ay += 0.040;
        az += 0.015;

        Mat3 rot = mat_mul(mat_mul(mat_rot_z(az), mat_rot_y(ay)), mat_rot_x(ax));

        render_scene(&canvas, cam, scene, rot);
        fflush(stdout);
        usleep(33000);
    }

    printf("\x1b[?25h\n");
    return 0;
}