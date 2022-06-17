#include "Sorts.h"
#include "defines.h"

#include <limits.h>
#include <stdlib.h>
#include <string.h>

void my_qsort_simple(void *base, size_t num, size_t width,
            int (*compare)(void *, const void *, const void *), void *arg) {
    if (num < 2U) // во избежание underflow при вычислении num - 2U
        return;
    register char *i = (char *) base, *j = (char *) base + (num - 2U) * width,
                  *R = j + width;
    SWAP((char *) base + (num >> 1) * width, R, width); // свапаем средний и последний
    do {
        while (compare(arg, i, R) < 0)
            i += width;
        while (compare(arg, R, j) < 0)
            j -= width;
        if (i < j) {
            SWAP(i, j, width);
            i += width;
            j -= width;
        }
        else if (i == j) { // это условие проверится всего один раз, поэтому
            i += width;    // лишних проверок во время основной работы цикла
            j -= width;    // не будет
            break;
        }
    } while (i <= j);

    SWAP(i, R, width);   // меняем бывший средний с первым числом, большим него,
    if ((char *) base < j)  // и далее считаем, что средний на своём месте
        my_qsort_simple(base, (j - (char *) base) / width + 1, width, compare, arg);
    if (i + width < R)
        my_qsort_simple(i + width, (R - i) / width, width, compare, arg);
}



void my_qsort_good_mid(void *base, size_t num, size_t width,
            int (*compare)(void *, const void *, const void *), void *arg) {
    if (num < 2U)
        return;
    // Медиана - среднее знаение среди первого, среднего и последнего элементов
    // слова "больше" и "меньше" взяты в кавычки ниже, т.к. порядок сортировки определяет
    // передаваемая функция и сортировка может осуществляться, н-р, в порядке убывания
    register char *i = (char *) base, *mid = i + (num >> 1) * width,
                  *j = i + (num - 2U) * width, *R = j + width;
    if (compare(arg, mid, base) < 0)
        SWAP(mid, base, width); // теперь по центру "больший" среди первого и mid
    if (compare(arg, R, mid) < 0) { // теперь сравним поставленный в центр элемент с правым
        SWAP(R, mid, width); // если правый "меньше", то меняем их местами
        if (compare(arg, mid, i) < 0) // и заново сраниваем новый центральный с левым
                SWAP(mid, i, width);
    } // теперь по центру у нас средний из трёх, а края уже отсортированы =>
    i += width; // j уже указывает на предпоследний элемент

    do {
        while (compare(arg, i, mid) < 0)
            i += width;
        while (compare(arg, mid, j) < 0)
            j -= width;
        if (i < j) {
            SWAP(i, j, width);
            if (mid == i)
                mid = j;
            else if (mid == j)
                mid = i;
            i += width;
            j -= width;
        }
        else if (i == j) { // это условие проверится всего один раз, поэтому
            i += width;    // лишних проверок во время основной работы цикла
            j -= width;    // не будет
            break;
        }
    } while (i <= j);

    if ((char *) base < j)
        my_qsort_good_mid(base, (j - (char *) base) / width + 1, width, compare, arg);
    if (i < R)
        my_qsort_good_mid(i, (R - i) / width + 1, width, compare, arg);
}



void my_qsort_with_insert_helper(void *base, void *buf1, void *buf2, size_t num, size_t width,
            int (*compare)(void *, const void *, const void *), void *arg) {
    if (num < 65U) {
        //insertion_sort(base, buf1, num, width, compare, arg);
        //insertion_bin_sort(base, buf1, num, width, compare, arg);
        //insertion_bin_sort_on_ptrs(base, buf1, num, width, compare, arg);
        //pair_insertion_sort(base, buf1, buf2, num, width, compare, arg);
        pair_insertion_bin_sort(base, buf1, buf2, num, width, compare, arg);
        return;
    }
    register char *i = (char *) base, *mid = i + (num >> 1) * width,
                  *j = i + (num - 2U) * width, *R = j + width;
    if (compare(arg, mid, base) < 0)
        SWAP(mid, base, width);
    if (compare(arg, R, mid) < 0) {
        SWAP(R, mid, width);
        if (compare(arg, mid, i) < 0)
                SWAP(mid, i, width);
    }
    i += width;

    do {
        while (compare(arg, i, mid) < 0)
            i += width;
        while (compare(arg, mid, j) < 0)
            j -= width;
        if (i < j) {
            SWAP(i, j, width);
            if (mid == i)
                mid = j;
            else if (mid == j)
                mid = i;
            i += width;
            j -= width;
        }
        else if (i == j) {
            i += width;
            j -= width;
            break;
        }
    } while (i <= j);

    if ((char *) base < j)
        my_qsort_with_insert_helper(base, buf1, buf2, (j - (char *) base) / width + 1, width, compare, arg);
    if (i < R)
        my_qsort_with_insert_helper(i, buf1, buf2, (R - i) / width + 1, width, compare, arg);
}
void my_qsort_with_insert(void *base, size_t num, size_t width,
            int (*compare)(void *, const void *, const void *), void *arg) {
    void *mas = (void *) malloc(2 * width);
    my_qsort_with_insert_helper(base, mas, (char *) mas + width, num, width, compare, arg);
    free(mas);
}



void my_qsort(void *base, size_t num, size_t width,
        int (*compare)(void *, const void *, const void *), void *arg) {
    if (num < 2U) // во избежание underflow при вычислении num - 1U, заодно
        return;   // прихватим тривиальный случай
    const size_t max_subarray_elements = 8U, max_subarray = max_subarray_elements * width;
    register char *L = (char *) base, *R = L + (num - 1U) * width, *i, *j;
    if (num > max_subarray_elements) {
        stack_node stack[STACK_SIZE];
        stack_node *top = stack;

        PUSH (NULL, NULL);
        while (STACK_NOT_EMPTY) {
            char *mid = L + ((R - L) / width >> 1) * width;
            if (compare(arg, mid, L) < 0)
                SWAP(mid, L, width);
            if (compare(arg, R, mid) < 0) {
                SWAP(mid, R, width);
                if (compare(arg, mid, L) < 0)
                        SWAP(mid, L, width);
            }
            i = L + width, j = R - width;
            
            do {
                while (compare(arg, i, mid) < 0)
                    i += width;
                while (compare(arg, mid, j) < 0)
                    j -= width;
                if (i < j) {
                    SWAP(i, j, width);
                    if (mid == i)
                        mid = j;
                    else if (mid == j)
                        mid = i;
                    i += width;
                    j -= width;
                }
                else if (i == j) {
                    i += width;
                    j -= width;
                    break;
                }
            } while (i <= j);

            // Если один из подмассивов / оба меньше нашего минимума, то
            // игнорируем его / их обработку, иначе помещаем границы большего(!)
            // из подмассивов в стек и продолжаем работу с меньшим
            // большего - чтобы гарантировать минимальную высоту стека
            if ((size_t) (j - L) <= max_subarray) { // левый меньше необходимого
                if ((size_t) (R - i) <= max_subarray)  // игнорируем оба
                    POP (L, R);
                else                                   // игнорируем левый
                    L = i;
            }
            else if ((size_t) (R - i) <= max_subarray) // игнорируем правый
                R = j;
            else if ((j - L) > (R - i)) {              // пушим левый
                PUSH (L, j);
                L = i;
            }
            else {                                     // пушим правый
                PUSH (i, R);
                R = j;
            }
        }
    }

    // Теперь наш массив состоит из неотстортированных внутри себя, но отсортированных
    // между собой минимальных подмассивов, отсортируем каждый: находим минимальный
    // элемент в первом подмассиве и ставим его в начало, он является минимальным
    // во всём массиве и далее будет опорным для сортировки, что позволит
    // исключить проверку в каждой итерации на выход за левую границу массива.
    // Далее просто сортировка вставками, которая не будет уходить от каждого
    // рассматриваемого элемента влево более, чем на max_subarray_elements элементов
    L = (char *) base, R = L + (num - 1U) * width;
    register char *L_runner = L, *R_runner;
    char *R_subarray = R < (L + max_subarray) ? R : L + max_subarray;
    for (R_runner = L + width; R_runner <= R_subarray; R_runner += width)
        if (compare(arg, R_runner, L_runner) < 0)
            L_runner = R_runner;
    if (L_runner != L)
        SWAP(L_runner, L, width);

    // Сортировка вставками
    R_runner = L + width;
    while ((R_runner += width) <= R) {
        L_runner = R_runner - width;
        while (compare(arg, R_runner, L_runner) < 0)
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
