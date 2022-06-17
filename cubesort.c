#include "defines.h"

#include <stdlib.h>
#include <string.h>

#define BSC_M 64
#define BSC_X 128
#define BSC_Y 64
#define BSC_Z 64

struct y_node {
    void *z_keys[BSC_Z];
};

struct x_node {
    void *y_floor[BSC_Y];
    struct y_node *y_axis[BSC_Y];
    unsigned char z_size[BSC_Y];
};

struct w_node {
    void *x_floor[BSC_X];
    struct x_node *x_axis[BSC_X];
    unsigned char y_size[BSC_X];
};

struct cube {
    void **w_floor;
    struct w_node **w_axis;
    unsigned char *x_size;
    unsigned short w_size;
};

void split_w_node(struct cube *cube, short w);
void split_x_node(struct cube *cube, short w, short x);
void split_y_node(struct w_node *w_node, short x, short y);

INLINE void insert_z_node(struct cube *cube, void *key_ptr,
            int (*compare)(void *, const void *, const void *), void *arg) {
    struct w_node *w_node;
    struct x_node *x_node;
    struct y_node *y_node;

    register short mid, w, x, y, z;

    if (0 == cube->w_size) {
        cube->w_floor = (void *) malloc(BSC_M * sizeof(void *));
        cube->w_axis = (struct w_node **) malloc(BSC_M * sizeof(struct w_node *));
        cube->x_size = (unsigned char *) malloc(BSC_M * sizeof(unsigned char));

        w_node = cube->w_axis[0] = (struct w_node *) malloc(sizeof(struct w_node));
        x_node = w_node->x_axis[0] = (struct x_node *) malloc(sizeof(struct x_node));
        y_node = x_node->y_axis[0] = (struct y_node *) malloc(sizeof(struct y_node));
        x_node->z_size[0] = 0;

        cube->w_size = cube->x_size[0] = w_node->y_size[0] = 1;

        w = x = y = z = 0;
        cube->w_floor[0] = w_node->x_floor[0] = x_node->y_floor[0] = key_ptr;
        goto insert;
    }

    if (compare(arg, key_ptr, cube->w_floor[0]) < 0) {
        w_node = cube->w_axis[0];
        x_node = w_node->x_axis[0];
        y_node = x_node->y_axis[0];

        w = x = y = z = 0;
        cube->w_floor[0] = w_node->x_floor[0] = x_node->y_floor[0] = key_ptr;
        goto insert;
    }

    // w
    mid = w = cube->w_size - 1;
    while (mid > 7) { // хвостатый бинпоиск
        mid >>= 1;
        if (compare(arg, key_ptr, cube->w_floor[w - mid]) < 0) w -= mid;
    }
    while (compare(arg, key_ptr, cube->w_floor[w]) < 0) --w;
    w_node = cube->w_axis[w];

    // x
    mid = x = cube->x_size[w] - 1;
    while (mid > 7) {
        mid >>= 1;
        if (compare(arg, key_ptr, w_node->x_floor[x - mid]) < 0)
            x -= mid;
    }
    while (compare(arg, key_ptr, w_node->x_floor[x]) < 0) --x;
    x_node = w_node->x_axis[x];

    // y
    mid = y = w_node->y_size[x] - 1;
    while (mid > 7) { // хвостатый четвертичный бинпоиск
        mid >>= 2;
        if (compare(arg, key_ptr, x_node->y_floor[y - mid]) < 0) {
            y -= mid;
            if (compare(arg, key_ptr, x_node->y_floor[y - mid]) < 0) {
                y -= mid;
                if (compare(arg, key_ptr, x_node->y_floor[y - mid]) < 0)
                    y -= mid;
            }
        }
    }
    while (compare(arg, key_ptr, x_node->y_floor[y]) < 0) --y;
    y_node = x_node->y_axis[y];

    // z
    mid = z = x_node->z_size[y] - 1;
    while (mid > 7) {
        mid >>= 2;
        if (compare(arg, key_ptr, y_node->z_keys[z - mid]) < 0) {
            z -= mid;
            if (compare(arg, key_ptr, y_node->z_keys[z - mid]) < 0) {
                z -= mid;
                if (compare(arg, key_ptr, y_node->z_keys[z - mid]) < 0)
                    z -= mid;
            }
        }
    }
    while (compare(arg, key_ptr, y_node->z_keys[z]) < 0) --z;

    // Раскомментировать для удаления дубликатов
    //if (0 == compare(arg, key_ptr, y_node->z_keys[z]))
    //    return;
    ++z;

insert:
    ++x_node->z_size[y];
    if (z + 1 != x_node->z_size[y])
        memmove(&y_node->z_keys[z + 1], &y_node->z_keys[z], (x_node->z_size[y] - z - 1) * sizeof(void *));
    y_node->z_keys[z] = key_ptr;

    if (BSC_Z == x_node->z_size[y]) {
        split_y_node(w_node, x, y);
        if (BSC_Y == w_node->y_size[x]) {
            split_x_node(cube, w, x);
            if (BSC_X == cube->x_size[w])
                split_w_node(cube, w);
        }
    }
}

void split_w_node(struct cube *cube, short w) {
    struct w_node *w_node1, *w_node2;
    short w_tmp = w + 1;

    // вставка w_node
    ++cube->w_size;
    if (0 == cube->w_size % BSC_M) {
        cube->w_floor = (void *) realloc(cube->w_floor, (cube->w_size + BSC_M) * sizeof(void *));
        cube->w_axis = (struct w_node **) realloc(cube->w_axis, (cube->w_size + BSC_M) * sizeof(struct w_node *));
        cube->x_size = (unsigned char *) realloc(cube->x_size, (cube->w_size + BSC_M) * sizeof(unsigned char));
    }
    if (w_tmp + 1 != cube->w_size) {
        memmove(&cube->w_floor[w_tmp + 1], &cube->w_floor[w_tmp], (cube->w_size - w_tmp - 1) * sizeof(void *));
        memmove(&cube->w_axis[w_tmp + 1], &cube->w_axis[w_tmp], (cube->w_size - w_tmp - 1) * sizeof(struct w_node *));
        memmove(&cube->x_size[w_tmp + 1], &cube->x_size[w_tmp], (cube->w_size - w_tmp - 1) * sizeof(unsigned char));
    }
    cube->w_axis[w_tmp] = (struct w_node *) malloc(sizeof(struct w_node));

    w_node1 = cube->w_axis[w];
    w_node2 = cube->w_axis[w_tmp];

    cube->x_size[w_tmp] = cube->x_size[w] >> 1;
    cube->x_size[w] -= cube->x_size[w_tmp];

    memcpy(&w_node2->x_floor[0], &w_node1->x_floor[cube->x_size[w]], cube->x_size[w_tmp] * sizeof(void *));
    memcpy(&w_node2->x_axis[0], &w_node1->x_axis[cube->x_size[w]], cube->x_size[w_tmp] * sizeof(struct x_node *));
    memcpy(&w_node2->y_size[0], &w_node1->y_size[cube->x_size[w]], cube->x_size[w_tmp] * sizeof(unsigned char));

    cube->w_floor[w_tmp] = w_node2->x_floor[0];
}

void split_x_node(struct cube *cube, short w, short x) {
    struct w_node *w_node = cube->w_axis[w];
    struct x_node *x_node1, *x_node2;
    short x_tmp = x + 1;

    // вставка x_node
    short x_size = ++cube->x_size[w];
    if (x_size != x_tmp + 1) {
        memmove(&w_node->x_floor[x_tmp + 1], &w_node->x_floor[x_tmp], (x_size - x_tmp - 1) * sizeof(void *));
        memmove(&w_node->x_axis[x_tmp + 1], &w_node->x_axis[x_tmp], (x_size - x_tmp - 1) * sizeof(struct x_node *));
        memmove(&w_node->y_size[x_tmp + 1], &w_node->y_size[x_tmp], (x_size - x_tmp - 1) * sizeof(unsigned char));
    }
    w_node->x_axis[x_tmp] = (struct x_node *) malloc(sizeof(struct x_node));

    x_node1 = w_node->x_axis[x];
    x_node2 = w_node->x_axis[x_tmp];

    w_node->y_size[x_tmp] = w_node->y_size[x] >> 1;
    w_node->y_size[x] -= w_node->y_size[x_tmp];

    memcpy(&x_node2->y_floor[0], &x_node1->y_floor[w_node->y_size[x]], w_node->y_size[x_tmp] * sizeof(void *));
    memcpy(&x_node2->y_axis[0], &x_node1->y_axis[w_node->y_size[x]], w_node->y_size[x_tmp] * sizeof(struct y_node *));
    memcpy(&x_node2->z_size[0], &x_node1->z_size[w_node->y_size[x]], w_node->y_size[x_tmp] * sizeof(unsigned char));

    w_node->x_floor[x_tmp] = x_node2->y_floor[0];
}

void split_y_node(struct w_node *w_node, short x, short y) {
    struct x_node *x_node = w_node->x_axis[x];
    struct y_node *y_node1, *y_node2;
    short y_tmp = y + 1;

    // вставка y_node
    short y_size = ++w_node->y_size[x];
    if (y_size != y_tmp + 1) {
        memmove(&x_node->y_floor[y_tmp + 1], &x_node->y_floor[y_tmp], (y_size - y_tmp - 1) * sizeof(void *));
        memmove(&x_node->y_axis[y_tmp + 1], &x_node->y_axis[y_tmp], (y_size - y_tmp - 1) * sizeof(struct y_node *));
        memmove(&x_node->z_size[y_tmp + 1], &x_node->z_size[y_tmp], (y_size - y_tmp - 1) * sizeof(unsigned char));
    }
    x_node->y_axis[y_tmp] = (struct y_node *) malloc(sizeof(struct y_node));

    y_node1 = x_node->y_axis[y];
    y_node2 = x_node->y_axis[y_tmp];

    x_node->z_size[y_tmp] = x_node->z_size[y] >> 1;
    x_node->z_size[y] -= x_node->z_size[y_tmp];

    memcpy(&y_node2->z_keys[0], &y_node1->z_keys[x_node->z_size[y]], x_node->z_size[y_tmp] * sizeof(void *));

    x_node->y_floor[y_tmp] = y_node2->z_keys[0];
}

#define SECTION 100000

void my_cubesort(void *base, size_t num, size_t width,
            int (*compare)(void *, const void *, const void *), void *arg) {
    void *buf = (void *) malloc(num * width);
    if (NULL == buf)
        return;
    register char *buf_runner = buf;

    register char *L = (char *) base, *R = L + num * width;
    if (num > 1000000) {
        size_t cnt = SECTION, addition = cnt * width;
        for (L += addition; cnt + SECTION < num; cnt += SECTION, L += addition)
            my_cubesort(L, SECTION, width, compare, arg);
        my_cubesort(L, num - cnt, width, compare, arg);
        L = (char *) base;
    }

    struct cube *cube = (struct cube *) calloc(1, sizeof(struct cube));
    while (L < R) {
        insert_z_node(cube, L, compare, arg);
        L += width;
    }

    // удалим куб, переписав из него данные
    if (cube->w_size) {
        struct w_node *w_node;
        struct x_node *x_node;
        struct y_node *y_node;
        register short w, x, y;
        for (w = 0; w < cube->w_size; ++w) {
            w_node = cube->w_axis[w];
            for (x = 0; x < cube->x_size[w]; ++x) {
                x_node = w_node->x_axis[x];
                for (y = 0; y < w_node->y_size[x]; ++y) {
                    y_node = x_node->y_axis[y];
                    for (int i = 0; i < x_node->z_size[y]; ++i, buf_runner += width)
                        memcpy(buf_runner, y_node->z_keys[i], width);
                    free(y_node);
                }
                free(x_node);
            }
            free(w_node);
        }
        free(cube->w_floor);
        free(cube->w_axis);
        free(cube->x_size);
    }
    free(cube);
    memcpy(base, buf, num * width);
}

// Здесь развёрнутая рекурсия оказалась немного медленнее
void my_cubesort_non_recurs(void *base, size_t num, size_t width,
            int (*compare)(void *, const void *, const void *), void *arg) {
    void *buf = (void *) malloc(num * width);
    if (NULL == buf || num < 2U) // чтобы исключить проверку в цикле на непустоту
        return;                  // куба, заодно "прихватим" тривиальный случай
    register char *buf_runner;

    char *L = (char *) base, *R = L + num * width;
    register char *L_runner, *R_runner;

    stack_node stack[2];
    stack_node *top = stack;
    PUSH(L, R);
    size_t addition = width;

    if (num > 1000000) {
        size_t tmp = num % SECTION;
        if (tmp) // чтобы исключить проверку в цикле на непустоту куба
            PUSH(R - tmp * width, R);
        addition = SECTION * width;
        L_runner = L + addition, R_runner = L_runner + addition;
        while (R_runner <= R) {
    cubesort:
            L = L_runner;
            struct cube *cube = (struct cube *) calloc(1, sizeof(struct cube));
            while (L_runner < R_runner) {
                insert_z_node(cube, L_runner, compare, arg);
                L_runner += width;
            }
            buf_runner = buf;
            struct w_node *w_node;
            struct x_node *x_node;
            struct y_node *y_node;
            register short w, x, y;
            for (w = 0; w < cube->w_size; ++w) {
                w_node = cube->w_axis[w];
                for (x = 0; x < cube->x_size[w]; ++x) {
                    x_node = w_node->x_axis[x];
                    for (y = 0; y < w_node->y_size[x]; ++y) {
                        y_node = x_node->y_axis[y];
                        for (int i = 0; i < x_node->z_size[y]; ++i, buf_runner += width)
                            memcpy(buf_runner, y_node->z_keys[i], width);
                        free(y_node);
                    }
                    free(x_node);
                }
                free(w_node);
            }
            free(cube->w_floor);
            free(cube->w_axis);
            free(cube->x_size);
            free(cube);
            memcpy(L, buf, R_runner - L);
            R_runner += addition;
        }
    }
    while (STACK_NOT_EMPTY) {
        POP(L_runner, R_runner);
        goto cubesort;
    }
}
