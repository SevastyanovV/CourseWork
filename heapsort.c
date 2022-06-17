#include "Sorts.h"
#include "defines.h"

INLINE void my_heapify(char *L, char *pos, char *R, size_t width,
            int (*compare)(void *, const void *, const void *), void *arg) {
    char *parent = pos, *max_ind = parent; // указатель на максимум
    while (1) {
        char *left = L + 2 * (parent - L) + width,
            *right = L + 2 * (parent - L) + 2 * width;
        if (left <= R && compare(arg, max_ind, left) < 0)
            max_ind = left;
        if (right <= R && compare(arg, max_ind, right) < 0)
            max_ind = right;
        if (max_ind == parent)
            break;
        else {
            SWAP(max_ind, parent, width);
            parent = max_ind;
        }
    }
}
void my_heapSort(void *base, size_t num, size_t width,
            int (*compare)(void *, const void *, const void *), void *arg) {
    register char *L = (char *) base, *R = L + num * width - width, *i;
    for (i = L + (num >> 1) * width - width; i >= L; i -= width)
        my_heapify(L, i, R, width, compare, arg);
    for (i = R; i > L; i -= width) {
        SWAP(base, i, width);
        my_heapify(L, L, i - width, width, compare, arg);
    }
}
