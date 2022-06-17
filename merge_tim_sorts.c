#include "Sorts.h"
#include "defines.h"

#include <stdlib.h>
#include <string.h>

// такая работа с доп. памятью допускает многопоточную обработку
void my_mergeSort_simple_helper(char *L, char *R, char *buf, size_t width,
            int (*compare)(void *, const void *, const void *), void *arg) {
    if (width == (size_t) (R - L)) {
        if (compare(arg, L, R) > 0)
            SWAP(L, R, width);
        return;
    }

    register char *M = L + ((R - L) / width >> 1) * width;
    if (L < M)
        my_mergeSort_simple_helper(L, M, buf, width, compare, arg);
    M += width;
    if (M < R)
        my_mergeSort_simple_helper(M, R, buf + (R - M), width, compare, arg);
    
    // Слияние
    register char *cur = buf, *i = L, *j = M;
    R += width;
    while (1) {
        if (i >= M) { // здесь и в следующем if'е можно, вместо копирования в
            memcpy(cur, j, R - j); // доп. массив и последующего обратного
            break; // копирования, наоборот скопировать данные в исходный массив
        } else if (j >= R) {
            memcpy(cur, i, M - i);
            break;
        } else if (compare(arg, i, j) > 0) {
            memcpy(cur, j, width);
            cur += width;
            j += width;
        } else {
            memcpy(cur, i, width);
            cur += width;
            i += width;
        }
    }
    memcpy(L, buf, R - L);
}
void my_mergeSort_simple(void *base, size_t num, size_t width,
            int (*compare)(void *, const void *, const void *), void *arg) {
    if (num < 2U) // чтобы не было underflow при вычислении num - 1 ниже, заодно прихватим тривиальный случай num = 1
        return;
    char *buf = (char *) malloc(num * width);
    if (NULL == buf)
        return;
    my_mergeSort_simple_helper(base, (char *) base + (num - 1U) * width,
                        buf, width, compare, arg);
    free(buf);
}



void my_mergeSort_insHelper(char *L, char *R, char *buf1, char *buf2, char *L_buf, size_t width,
            int (*compare)(void *, const void *, const void *), void *arg) {
    if ((size_t) (R - L) < 64U * width) {
        //insertion_sort(L, buf1, (R - L) / width + 1U, width, compare, arg);
        //insertion_bin_sort(L, buf1, (R - L) / width + 1U, width, compare, arg);
        //insertion_bin_sort_on_ptrs(L, buf1, (R - L) / width + 1U, width, compare, arg);
        //pair_insertion_sort(L, buf1, buf2, (R - L) / width + 1U, width, compare, arg);
        pair_insertion_bin_sort(L, buf1, buf2, (R - L) / width + 1U, width, compare, arg);
        return;
    }

    register char *M = L + ((R - L) / width >> 1) * width; // левая часть не длиннее правой
    if (L < M)
        my_mergeSort_insHelper(L, M, buf1, buf2, L_buf, width, compare, arg);
    M += width;
    if (M < R)
        my_mergeSort_insHelper(M, R, buf1, buf2, L_buf, width, compare, arg);
    
    // Слияние
    memcpy(L_buf, L, M - L); // копируем меньшую часть в буфер
    register char *cur = L, *i = L_buf, *j = M;
    M = L_buf + (M - L); // первый байт буфера после скопированной части
    R += width;
    while (1) {
        if (i >= M) {
            return; // можно было break
        } else if (j >= R) {
            memcpy(cur, i, M - i);
            return;
        } else if (compare(arg, i, j) > 0) {
            memcpy(cur, j, width);
            cur += width;
            j += width;
        } else {
            memcpy(cur, i, width);
            cur += width;
            i += width;
        }
    }
}
void my_mergeSort_with_insert(void *base, size_t num, size_t width,
            int (*compare)(void *, const void *, const void *), void *arg) {
    if (num < 2U) // чтобы не было underflow при вычислении num - 1 ниже, заодно прихватим тривиальный случай num = 1
        return;
    char *buf1 = (char *) malloc(( (num >> 1) + 2U) * width);
    if (NULL == buf1)
        return;
    char *buf2 = buf1 + width;
    my_mergeSort_insHelper(base, (char *) base + (num - 1U) * width, buf1, buf2,
                        buf2 + width, width, compare, arg);
    free(buf1);
}



void my_mergeSort(void *base, size_t num, size_t width,
            int (*compare)(void *, const void *, const void *), void *arg) {
    register char *buf1 = (char *) malloc((num + 2U) * width);
    if (NULL == buf1)
        return;
    register char *buf2 = buf1 + width, *L_buf = buf2 + width,
        *L = (char *) base, *R = L + (num - 1U) * width;

    size_t minrun = num, r = 0;
    while (minrun >= 64U) {
        r |= minrun & 1;
        minrun >>= 1;
    }
	minrun += r;

    stack_node stack[STACK_SIZE];
    stack_node *top = stack;
    PUSH(L, (char *) num);
    unsigned char stack_size = 1U;
    register char *ptr = L, *X, *Y, *Z, *L_merge, *R_merge, *M;
    while (ptr <= R) {
        top->L = ptr; // первый аргумент PUSH'а

        // Составление прогона
        size_t numToSort = ((R - ptr) / width + 1U < minrun) ? (R - ptr) / width + 1U : minrun;
        //insertion_sort(ptr, buf1, numToSort, width, compare, arg);
        //insertion_bin_sort(ptr, buf1, numToSort, width, compare, arg);
        //insertion_bin_sort_on_ptrs(ptr, buf1, numToSort, width, compare, arg);
        //pair_insertion_sort(ptr, buf1, buf2, numToSort, width, compare, arg);
        pair_insertion_bin_sort(ptr, buf1, buf2, numToSort, width, compare, arg);
        ptr += numToSort * width;
        
        top->R = (char *) numToSort; // второй аргумент PUSH'а - размер
        ++stack_size; // ++top сделаем после цикла (для удобства)
        while (stack_size > 2U) {
            Z = top->R, Y = (top - 1)->R, X = (top - 2)->R;
            if (X <= Y + (size_t) Z || Y <= Z) {
                if (X < Z) { // сливаем подмассивы с длинами X и Y
                    L_merge = (top - 2)->L, M = (top - 1)->L, R_merge = top->L - width;
                    // Перепишем данные с Z на Y
                    (top - 1)->L = top->L;
                    (top - 1)->R = top->R;
                    --top;
                    // Объединим X и Y
                    (top - 1)->R += (size_t) Y;
                }
                else { // Y и Z
                    L_merge = (top - 1)->L, M = top->L, R_merge = ptr - width;
                    --top;
                    top->R += (size_t) Z;
                }
                --stack_size;

                // Слияние
                memcpy(L_buf, L_merge, M - L_merge); // копируем левую часть в буфер
                char *cur = L_merge, *i = L_buf, *j = M;
                M = L_buf + (M - L_merge); // первый байт буфера после скопированной части
                R_merge += width;
                while (1) {
                    if (i >= M) {
                        break;
                    } else if (j >= R_merge) {
                        memcpy(cur, i, M - i);
                        break;
                    } else if (compare(arg, i, j) > 0) {
                        memcpy(cur, j, width);
                        cur += width;
                        j += width;
                    } else {
                        memcpy(cur, i, width);
                        cur += width;
                        i += width;
                    }
                }
            }
            else
                break;
        }
        ++top;
    }

    // Если в стеке остались неслитые подмассивы, то это нужно исправить
    while (stack_size > 2U) { // не забываем, что первый элемент - фиктивный
        --top;
        --stack_size;
        L_merge = (top - 1)->L, M = top->L, R_merge = R;

        memcpy(L_buf, L_merge, M - L_merge); // копируем левую часть в буфер
        char *cur = L_merge, *i = L_buf, *j = M;
        M = L_buf + (M - L_merge); // первый байт буфера после скопированной части
        R_merge += width;
        while (1) {
            if (i >= M) {
                break;
            } else if (j >= R_merge) {
                memcpy(cur, i, M - i);
                break;
            } else if (compare(arg, i, j) > 0) {
                memcpy(cur, j, width);
                cur += width;
                j += width;
            } else {
                memcpy(cur, i, width);
                cur += width;
                i += width;
            }
        }
    }

    free(buf1);
}





void my_timsort(void *base, size_t num, size_t width,
            int (*compare)(void *, const void *, const void *), void *arg) {
    char *buf1 = (char *) malloc((num + 2U) * width);
    if (NULL == buf1)
        return;
    char *buf2 = buf1 + width, *L_buf = buf2 + width,
        *L = (char *) base, *R = L + (num - 1U) * width;

    size_t minrun = num, r = 0;
    while (minrun >= 64U) {
        r |= minrun & 1;
        minrun >>= 1;
    }
	minrun += r;

    size_t gallopStep;
    int gallopCounter;
    const int gallopTrigger = 7;
    char gallopSideOfSubarr, *tmp, *tmpPrev; // *tmpM - понадобится для бинпоиска по полуинтервалу
    // к ним не применять register

    stack_node stack[STACK_SIZE];
    stack_node *top = stack;
    PUSH(L, (char *) num);
    unsigned char stack_size = 1U;
    register char *ptr = L, *L_merge, *R_merge, *M;
    char *X, *Y, *Z;
    while (ptr <= R) {
        top->L = ptr; // первый аргумент PUSH'а

        // Составление прогона
        register size_t numToSort = 1U;
        while (ptr < R && compare(arg, ptr, ptr + width) <= 0) {
            ++numToSort;
            ptr += width;
        }
        if (1U == numToSort) { // обратно отсортированы
            char *i = ptr, *j;
            while (ptr < R && compare(arg, ptr, ptr + width) >= 0) {
                ++numToSort;
                ptr += width;
            }
            // теперь необходимо отзеркалить
            j = ptr;
            while (i < j) {
                SWAP(i, j, width);
                i += width, j -= width;
            }
        }
        ptr += width; // чтобы ptr указывал на элемент после пробега
        if (numToSort < minrun && ptr < R) {
            r = (minrun - numToSort <= (R - ptr) / width) ? minrun - numToSort : (R - ptr) / width + 1U;
            numToSort += r;
            ptr += r * width;
            //insertion_sort(ptr, buf1, numToSort, width, compare, arg);
            //insertion_bin_sort(ptr, buf1, numToSort, width, compare, arg);
            //insertion_bin_sort_on_ptrs(ptr, buf1, numToSort, width, compare, arg);
            //pair_insertion_sort(ptr, buf1, buf2, numToSort, width, compare, arg);
            pair_insertion_bin_sort(top->L, buf1, buf2, numToSort, width, compare, arg);
        }
        
        top->R = (char *) numToSort; // второй аргумент PUSH'а - размер
        ++stack_size; // ++top сделаем после цикла (для удобства)
        while (stack_size > 2U) {
            Z = top->R, Y = (top - 1)->R, X = (top - 2)->R;
            if (X <= Y + (size_t) Z || Y <= Z) {
                if (X < Z) { // сливаем подмассивы с длинами X и Y
                    L_merge = (top - 2)->L, M = (top - 1)->L, R_merge = top->L - width;
                    // Перепишем данные с Z на Y
                    (top - 1)->L = top->L;
                    (top - 1)->R = top->R;
                    --top;
                    // Объединим X и Y
                    (top - 1)->R += (size_t) Y;
                }
                else { // Y и Z
                    L_merge = (top - 1)->L, M = top->L, R_merge = ptr - width;
                    --top;
                    top->R += (size_t) Z;
                }
                --stack_size;

                // Слияние
                gallopSideOfSubarr = 'n'; // n - neutral
                memcpy(L_buf, L_merge, M - L_merge); // копируем левую часть в буфер
                register char *cur = L_merge, *i = L_buf, *j = M;
                M = L_buf + (M - L_merge); // первый байт буфера после скопированной части
                R_merge += width;
                while (1) {
                    if (i >= M) {
                        break;
                    } else if (j >= R_merge) {
                        memcpy(cur, i, M - i);
                        break;
                    } else if (compare(arg, i, j) > 0) {
                        if ('r' == gallopSideOfSubarr) {
                            ++gallopCounter;
                            if (gallopCounter >= gallopTrigger) { // переходим в режим галопа
                                gallopStep = 2U;
                                tmpPrev = j;
                                tmp = j + gallopStep * width;
                                while (tmp < R_merge && compare(arg, i, tmp) > 0) {
                                    gallopStep <<= 1;
                                    tmpPrev = tmp;
                                    tmp += gallopStep * width;
                                } // получили полуинтервал [tmpPrev, tmp)
                                /*// бинпоиск по полуинтервалу (не забыть подставить в другие места сортировки)
                                if (tmp >= R_merge) {tmp = R_merge - width; tmpPrev += width;}
                                while (tmpPrev < tmp) {
                                    tmpM = tmpPrev + width * ((tmp - tmpPrev) / width >> 1);
                                    if (compare(arg, i, tmpM) < 0)
                                        tmp = tmpM;
                                    else
                                        tmpPrev = tmpM + width;
                                } // tmpPrev будет указывать на первый элемент после "галопа",
                                  // поэтому нужно закомментировать строчку "tmpPrev += width;"
                                */
                                tmpPrev += width;
                                memcpy(cur, j, tmpPrev - j);
                                cur += tmpPrev - j;
                                j = tmpPrev;
                                continue;
                            }
                        }
                        else {
                            gallopSideOfSubarr = 'r';
                            gallopCounter = 1;
                        }
                        memcpy(cur, j, width);
                        cur += width;
                        j += width;
                    } else {
                        if ('l' == gallopSideOfSubarr) {
                            ++gallopCounter;
                            if (gallopCounter >= gallopTrigger) {
                                gallopStep = 2U;
                                tmpPrev = i;
                                tmp = i + gallopStep * width;
                                while (tmp < M && compare(arg, j, tmp) > 0) {
                                    gallopStep <<= 1;
                                    tmpPrev = tmp;
                                    tmp += gallopStep * width;
                                }
                                tmpPrev += width;
                                memcpy(cur, i, tmpPrev - i);
                                cur += tmpPrev - i;
                                i = tmpPrev;
                                continue;
                            }
                        }
                        else {
                            gallopSideOfSubarr = 'l';
                            gallopCounter = 1;
                        }
                        memcpy(cur, i, width);
                        cur += width;
                        i += width;
                    }
                }
            }
            else
                break;
        }
        ++top;
    }

    // Если в стеке остались неслитые подмассивы, то это нужно исправить
    while (stack_size > 2U) { // не забываем, что первый элемент - фиктивный
        --top;
        --stack_size;
        L_merge = (top - 1)->L, M = top->L, R_merge = R;

        gallopSideOfSubarr = 'n'; // n - neutral
        memcpy(L_buf, L_merge, M - L_merge); // копируем левую часть в буфер
        register char *cur = L_merge, *i = L_buf, *j = M;
        M = L_buf + (M - L_merge); // первый байт буфера после скопированной части
        R_merge += width;
        while (1) {
            if (i >= M) {
                break;
            } else if (j >= R_merge) {
                memcpy(cur, i, M - i);
                break;
            } else if (compare(arg, i, j) > 0) {
                if ('r' == gallopSideOfSubarr) {
                    ++gallopCounter;
                    if (gallopCounter >= gallopTrigger) {
                        gallopStep = 2U;
                        tmpPrev = j;
                        tmp = j + gallopStep * width;
                        while (tmp < R_merge && compare(arg, i, tmp) > 0) {
                            gallopStep <<= 1;
                            tmpPrev = tmp;
                            tmp += gallopStep * width;
                        }
                        tmpPrev += width;
                        memcpy(cur, j, tmpPrev - j);
                        cur += tmpPrev - j;
                        j = tmpPrev;
                        continue;
                    }
                }
                else {
                    gallopSideOfSubarr = 'r';
                    gallopCounter = 1;
                }
                memcpy(cur, j, width);
                cur += width;
                j += width;
            } else {
                if ('l' == gallopSideOfSubarr) {
                    ++gallopCounter;
                    if (gallopCounter >= gallopTrigger) {
                        gallopStep = 2U;
                        tmpPrev = i;
                        tmp = i + gallopStep * width;
                        while (tmp < M && compare(arg, j, tmp) > 0) {
                            gallopStep <<= 1;
                            tmpPrev = tmp;
                            tmp += gallopStep * width;
                        }
                        tmpPrev += width;
                        memcpy(cur, i, tmpPrev - i);
                        cur += tmpPrev - i;
                        i = tmpPrev;
                        continue;
                    }
                }
                else {
                    gallopSideOfSubarr = 'l';
                    gallopCounter = 1;
                }
                memcpy(cur, i, width);
                cur += width;
                i += width;
            }
        }
    }

    free(buf1);
}
