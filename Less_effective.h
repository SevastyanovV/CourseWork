#ifndef Less_effective_h
#define Less_effective_h

#include <stdlib.h>
#include <string.h>

#include "defines.h"

void my_heapSort( 
    void *base,
    size_t num,
    size_t width,
    int (*compare)(void *, const void *, const void *),
    void *arg
    );

void my_mergeSort(
    void *base,
    size_t num,
    size_t width,
    int (*compare)(void *, const void *, const void *),
    void *arg
    );




INLINE void my_heapify(void *base, size_t pos, size_t num, size_t width,
            int (*compare)(void *, const void *, const void *), void *arg) {
    long long parent = pos, max_ind = parent; // индекс максимума
    for (;;) {
        long long left = 2 * parent + 1, right = 2 * parent + 2;
        if (left < (long long) num && compare(arg, (char *) base + max_ind * width, (char *) base + left * width) < 0)
            max_ind = left;
        if (right < (long long) num && compare(arg, (char *) base + max_ind * width, (char *) base + right * width) < 0)
            max_ind = right;
        if (max_ind == parent)
            break;
        else {
            SWAP((char *) base + max_ind * width, (char *) base + parent * width, width);
            parent = max_ind;
        }
    }
}
void my_heapSort(void *base, size_t num, size_t width,
            int (*compare)(void *, const void *, const void *), void *arg) {
    for (long long i = (num >> 1) - 1; i >= 0; --i)
        my_heapify(base, i, num, width, compare, arg);
    for (long long i = num - 1; i > 0; --i) {
        SWAP(base, (char *) base + i * width, width);
        my_heapify(base, 0, i, width, compare, arg);
    }
}



void my_mergeSortHelper(void *base, void *buf, size_t L, size_t R, size_t width,
            int (*compare)(void *, const void *, const void *), void *arg) {
    if (1U == R - L) {
        if (compare(arg, (char *) base + L * width, (char *) base + R * width) > 0)
            SWAP((char *) base + L * width, (char *) base + R * width, width);
        return;
    }

    size_t M = (L + R) >> 1;
    if (L < M)
        my_mergeSortHelper(base, buf, L, M, width, compare, arg);
    if (M + 1U < R)
        my_mergeSortHelper(base, buf, M + 1U, R, width, compare, arg);
    
    // слияние
    size_t cur = L, i = L, j = M + 1U;
    while (R + 1U != cur) {
        if (i > M) {
            memcpy((char *) buf + cur * width, (char *) base + j * width, (R - j + 1U) * width);
            break;
        } else if (j > R) {
            memcpy((char *) buf + cur * width, (char *) base + i * width, (M - i + 1U) * width);
            break;
        } else if (compare(arg, (char *) base + i * width, (char *) base + j * width) > 0) {
            memcpy((char *) buf + cur * width, (char *) base + j * width, width);
            ++cur;
            ++j;
        } else {
            memcpy((char *) buf + cur * width, (char *) base + i * width, width);
            ++cur;
            ++i;
        }
    }
    memcpy((char *) base + L * width, (char *) buf + L * width, (R - L + 1U) * width);
}
void my_mergeSort(void *base, size_t num, size_t width,
            int (*compare)(void *, const void *, const void *), void *arg) {
    if (num < 2U) // чтобы не было underflow при вычислении num - 1 ниже, заодно прихватим тривиальный случай num = 1
        return;
    void *buf = (void *) malloc(width * num);
    if (NULL == buf) // может функция должна возвращать значение, сообщающее успешность или выводить сообщение?
        return;
    my_mergeSortHelper(base, buf, 0, num - 1U, width, compare, arg);
    free(buf);
}

#endif