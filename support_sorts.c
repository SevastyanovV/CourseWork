#include "Sorts.h"

#include <string.h>

void insertion_sort(void *base, void *buf, size_t num, size_t width, // сортировка вставками
            int (*compare)(void *, const void *, const void *), void *arg) {
    char *L = (char *) base, *L_runner, *R = L + num * width - width; // во избежание underflow
    for (char *i = L + width; i <= R; i += width) {                   // при вычислении num - 1U
        memcpy(buf, i, width);
        L_runner = i;
        while ((L_runner -= width) >= L && compare(arg, buf, L_runner) < 0)
            memmove(L_runner + width, L_runner, width);
        memcpy(L_runner + width, buf, width);
    }
}
// Предыдущая сортировка выгоднее на большом количестве элементов
void insertion_sort_end_to_end_copying(void *base, size_t num, size_t width, // сортировка вставками
            int (*compare)(void *, const void *, const void *), void *arg) { // медленнее предыдущей
    char *L = (char *) base, *i, *j, *L_runner, *R_runner = L,
        *R = L + num * width - width; // во избежание underflow при вычислении num - 1U
    while ((R_runner += width) <= R) {
        L_runner = R_runner - width;
        while (L_runner >= L && compare(arg, R_runner, L_runner) < 0)
            L_runner -= width;
        L_runner += width; // теперь указывает на первый элемент, "больший" рассматриваемого

        if (L_runner != R_runner) { // копируем по байту справа налево насквозь
            char *tmp = R_runner + width;
            while (--tmp >= R_runner) {
                char c = *tmp;
                for (j = i = tmp; (i -= width) >= L_runner; j = i)
                    *j = *i;
                *j = c;
            }
        }
    }
}

void insertion_bin_sort(void *base, void *buf, size_t num, size_t width, // сортировка вставками с бинпоиском
            int (*compare)(void *, const void *, const void *), void *arg) {
    size_t L, R, M;
    for (size_t i = 1; i < num; ++i) {
        L = 0, R = i;
        memcpy(buf, (char *) base + i * width, width);
        while (L < R) { // строгое неравенство обязательно, так как сдвиг правой границы влево в случае равного элемента
            M = (L + R) >> 1; // в дальнейшем означает лишние обмены. При большом количестве повторяющихся элементов это
            if (compare(arg, buf, (char *) base + M * width) < 0) // критично
                R = M;
            else
                L = M + 1U;
        } // если R = num - 1U, то в несуществующий элемент массива R + 1 будет скопировано 0 байт, это ошибка?
        // if (R != num - 1U)
        memmove((char *) base + (R + 1U) * width, (char *) base + R * width, (i - R) * width);
        memcpy((char *) base + R * width, buf, width);
    }
}
// Немного быстрее предыдущей благодаря тому, что написана "на указателях"
void insertion_bin_sort_on_ptrs(void *base, void *buf, size_t num, size_t width, // сортировка вставками с бинпоиском
            int (*compare)(void *, const void *, const void *), void *arg) {
    char *first = (char *) base, *L, *R, *M, *last = first + num * width;
    for (char *i = first + width; i < last; i += width) {
        L = first, R = i;
        memcpy(buf, i, width);
        while (L < R) { // строгое неравенство обязательно, так как сдвиг правой границы влево в случае равного элемента
            M = L + width * ((R - L) / width >> 1); // в дальнейшем означает лишние обмены. При большом количестве
            if (compare(arg, buf, M) < 0) // повторяющихся элементов это критично
                R = M;
            else
                L = M + width;
        } // если R + width указывает на несуществующий элемент массива, но будет скопировано 0 байт, это ошибка?
        // if (R != last - width)
        memmove(R + width, R, i - R);
        memcpy(R, buf, width);
    }
}

void pair_insertion_sort(void *base, void *buf1, void *buf2, size_t num, size_t width, // парная сортировка вставками
            int (*compare)(void *, const void *, const void *), void *arg) {
    if (num < 2U) return; // нужно для корректной вставки нечётного элемента в конце функции, чтобы ind в memcpy не был
    long long ind = 0;    // равен -1 при num = 0U, заодно прихватим и тривиальный случай, когда num = 1U
    for (long long k = ind; ++ind < (long long) num; k = ++ind) {
        if (compare(arg, (char *) base + k * width, (char *) base + ind * width) < 0) {
            memcpy(buf1, (char *) base + ind * width, width); // в buf1 помещаем максимальный
            memcpy(buf2, (char *) base + k * width, width);
        }
        else {
            memcpy(buf1, (char *) base + k * width, width); // в buf1 помещаем максимальный
            memcpy(buf2, (char *) base + ind * width, width);
        }
        // вставка большего элемента
        while (--k >= 0 && compare(arg, buf1, (char *) base + k * width) < 0)
            memmove((char *) base + (k + 2) * width, (char *) base + k * width, width);
        memcpy((char *) base + (++k + 1) * width, buf1, width);
        // вставка меньшего элемента
        while (--k >= 0 && compare(arg, buf2, (char *) base + k * width) < 0)
            memmove((char *) base + (k + 1) * width, (char *) base + k * width, width);
        memcpy((char *) base + (k + 1) * width, buf2, width);
    }
    ind = (long long) num - 1;  // если было чётное число элементов, то цикл сразу остановится, отдельный if не нужен
    memcpy(buf2, (char *) base + ind * width, width);
    while (--ind >= 0 && compare(arg, buf2, (char *) base + ind * width) < 0)
        memmove((char *) base + (ind + 1) * width, (char *) base + ind * width, width);
    memcpy((char *) base + (ind + 1) * width, buf2, width);
}

void pair_insertion_bin_sort(void *base, void *buf1, void *buf2, size_t num, size_t width, // парная сортировка
            int (*compare)(void *, const void *, const void *), void *arg) {               // вставками с бинпоиском
    size_t ind = 0, L, R, M;
    for (size_t k = ind; ++ind < num; k = ++ind) {
        if (compare(arg, (char *) base + k * width, (char *) base + ind * width) < 0) {
            memcpy(buf1, (char *) base + ind * width, width); // в buf1 помещаем максимальный
            memcpy(buf2, (char *) base + k * width, width);
        }
        else {
            memcpy(buf1, (char *) base + k * width, width); // в buf1 помещаем максимальный
            memcpy(buf2, (char *) base + ind * width, width);
        }
        // вставка большего элемента
        // ввиду того, что мы вставляем пару элементов, buf1 нужно вставлять в R + 1 позицию, сдвигая все элементы в
        // отсортированном подмассиве на 2 индекса вправо
        L = 0, R = ind - 1;
        while (L < R) {
            M = (L + R) >> 1;
            if (compare(arg, buf1, (char *) base + M * width) < 0)
                R = M;
            else
                L = M + 1;
        }
        // в силу условий цикла for случай R = num - 1 невозможен:
        // num - нечётное => max(ind) = num - 2 => max(R) = num - 3 => !
        // num - чётное => max(ind) = num - 1 => max(R) = num - 2 => !
        // if (R != num - 2U) для первого memcpy?
        memmove((char *) base + (R + 2U) * width, (char *) base + R * width, (ind - R - 1U) * width);
        memcpy((char *) base + (R + 1U) * width, buf1, width);
        // вставка меньшего элемента
        L = 0, k = R; // R уже имеет нужное значение
        while (L < R) {
            M = (L + R) >> 1;
            if (compare(arg, buf2, (char *) base + M * width) < 0)
                R = M;
            else
                L = M + 1;
        }
        memmove((char *) base + (R + 1U) * width, (char *) base + R * width, (k - R) * width);
        memcpy((char *) base + R * width, buf2, width);
    }
    if (num & 1U) {
        L = 0, R = num - 1U;
        memcpy((char *) buf2, (char *) base + R * width, width);
        while (L < R) {
            M = (L + R) >> 1;
            if (compare(arg, buf2, (char *) base + M * width) < 0)
                R = M;
            else
                L = M + 1U;
        } // if (R != num - 1U)
        memmove((char *) base + (R + 1U) * width, (char *) base + R * width, (num - 1U - R) * width);
        memcpy((char *) base + R * width, buf2, width);
    }
}
