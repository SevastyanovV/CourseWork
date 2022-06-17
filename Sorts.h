#ifndef Sorts_h
#define Sorts_h

#include <stddef.h>

// ВСПОМОГАТЕЛЬНЫЕ СОРТИРОВКИ - СОРТИРОВКИ ВСТАВКАМИ

void insertion_sort( // обычная сортировка вставками
    void *base,
    void *buf,
    size_t num,
    size_t width,
    int (*compare)(void *, const void *, const void *),
    void *arg
    );
    
void insertion_sort_end_to_end_copying( // сортировка вставками со сквозным копированием
    void *base,
    size_t num,
    size_t width, 
    int (*compare)(void *, const void *, const void *),
    void *arg
    );

void insertion_bin_sort( // сортировка вставками с бинарным поиском
    void *base,
    void *buf,
    size_t num,
    size_t width,
    int (*compare)(void *, const void *, const void *),
    void *arg
    );

void insertion_bin_sort_on_ptrs( // сортировка вставками с бинпоиском, написанная "на указателях"
    void *base,
    void *buf,
    size_t num, 
    size_t width,
    int (*compare)(void *, const void *, const void *),
    void *arg
    );

void pair_insertion_sort( // парная сортировка вставками
    void *base,
    void *buf1,
    void *buf2,
    size_t num,
    size_t width,
    int (*compare)(void *, const void *, const void *),
    void *arg
    );

void pair_insertion_bin_sort( // парная сортировка с бинпосиком
    void *base,
    void *buf1,
    void *buf2,
    size_t num,
    size_t width, 
    int (*compare)(void *, const void *, const void *),
    void *arg
    );



// БЫСТРЫЕ СОРТИРОВКИ

void my_qsort_simple( // берёт медиану как центральный элемент
    void *base,
    size_t num,
    size_t width,
    int (*compare)(void *, const void *, const void *), 
    void *arg
    );

void my_qsort_good_mid( // берёт медиану как среднее между центральным и крайними элементами
    void *base,
    size_t num,
    size_t width,
    int (*compare)(void *, const void *, const void *), 
    void *arg
    );

void my_qsort_with_insert( // использует сортировку вставками
    void *base,
    size_t num,
    size_t width,
    int (*compare)(void *, const void *, const void *), 
    void *arg
    );
        
void my_qsort( // "развёрнутая" рекурсия, сквозная встроенная сортировка вставками
    void *base,
    size_t num,
    size_t width,
    int (*compare)(void *, const void *, const void *), 
    void *arg
    );



// ПИРАМИДАЛЬНАЯ СОРТИРОВКА

void my_heapSort(
    void *base,
    size_t num,
    size_t width,
    int (*compare)(void *, const void *, const void *), 
    void *arg
    );



// СОРТИРОВКИ СЛИЯНИЕМ & TIMSORT

void my_mergeSort_simple( // использует параллельное индексирование
    void *base,
    size_t num,
    size_t width,
    int (*compare)(void *, const void *, const void *), 
    void *arg
    );

void my_mergeSort_with_insert(  // использует сортировку вставками
    void *base,
    size_t num,
    size_t width,
    int (*compare)(void *, const void *, const void *), 
    void *arg
    );

void my_mergeSort( // "развёрнутая" рекурсия по приницпу timsort
    void *base,
    size_t num,
    size_t width,
    int (*compare)(void *, const void *, const void *), 
    void *arg
    );

void my_timsort( // допольнительно к стандартному алгоритму имеет закомментированный
    void *base,  // блок кода, ответственный за бинпоиск по полуинтервалу после галопа
    size_t num,
    size_t width,
    int (*compare)(void *, const void *, const void *), 
    void *arg
    );



// CUBESORTS

void my_cubesort(
    void *base,
    size_t num,
    size_t width,
    int (*compare)(void *, const void *, const void *), 
    void *arg
    );

void my_cubesort_non_recurs( // "развёрнутая" рекурсия, почти всегда уступает
    void *base,              // предыдущей сортировке
    size_t num,
    size_t width,
    int (*compare)(void *, const void *, const void *), 
    void *arg
    );

#endif