#include "Sorts.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

int sort_number(void *arg, const void *a, const void *b);
int sort_number_non_sequre(const void *a, const void *b);

int isSorted(int *mas, int num, int *numOfMistakes,
            int (*compare)(void *, const void *, const void *), void *arg) {
    *numOfMistakes = 0;
    for (int i = 0; i < num - 1; ++i)
        if (compare(arg, &(mas[i]), &(mas[i+1])) > 0) {
            printf(" #%d", i);
            ++*numOfMistakes;
        }
    if (*numOfMistakes)
        return 0;
    else
        return 1;
}

int compareWithSorted(int *mas, int *mas2, int num, int *numOfMistakes,
            int (*compare)(void *, const void *, const void *), void *arg) {
    *numOfMistakes = 0;
    for (int i = 0; i < num; ++i)
        if (compare(arg, &(mas[i]), &(mas2[i]))) {
            printf(" #%d", i);
            ++*numOfMistakes;
        }
    if (*numOfMistakes)
        return 0;
    else
        return 1;
}

void tests(const unsigned int N, int howToCheck) {
    int tmp1, tmp2;
    int *mas = (int *) malloc(N * sizeof(int)),
        *mas2 = (int *) malloc(N * sizeof(int)), *mas3;
    if (howToCheck)
        mas3 = (int *) malloc(N * sizeof(int));
    while (1) {
        for (int i = 0; i < N; ++i)
            mas[i] = rand();
        memcpy(mas2, mas, N * sizeof(int));
        if (howToCheck)
            memcpy(mas3, mas, N * sizeof(int));
        // вспомогательные
        //insertion_sort(mas, &tmp1, N, sizeof(int), sort_number, NULL);
        //insertion_sort_end_to_end_copying(mas, N, sizeof(int), sort_number, NULL);
        //insertion_bin_sort(mas, &tmp1, N, sizeof(int), sort_number, NULL);
        //insertion_bin_sort_on_ptrs(mas, &tmp1, N, sizeof(int), sort_number, NULL);
        //pair_insertion_sort(mas, &tmp1, &tmp2, N, sizeof(int), sort_number, NULL);
        pair_insertion_bin_sort(mas, &tmp1, &tmp2, N, sizeof(int), sort_number, NULL);
        // быстрые сортировки
        //my_qsort_simple(mas, N, sizeof(int), sort_number, NULL);
        //my_qsort_good_mid(mas, N, sizeof(int), sort_number, NULL);
        //my_qsort_with_insert(mas, N, sizeof(int), sort_number, NULL);
        //my_qsort(mas, N, sizeof(int), sort_number, NULL);
        // пирамидальная сортировка
        //my_heapSort(mas, N, sizeof(int), sort_number, NULL);
        // сортировки слиянием и тимсорт
        //my_mergeSort_simple(mas, N, sizeof(int), sort_number, NULL);
        //my_mergeSort_with_insert(mas, N, sizeof(int), sort_number, NULL);
        //my_mergeSort(mas, N, sizeof(int), sort_number, NULL);
        //my_timsort(mas, N, sizeof(int), sort_number, NULL);
        // cubesorts
        //my_cubesort(mas, N, sizeof(int), sort_number, NULL);
        //my_cubesort_non_recurs(mas, N, sizeof(int), sort_number, NULL);

        if (howToCheck) {
            qsort(mas3, N, sizeof(int), sort_number_non_sequre);
            if(0 == compareWithSorted(mas, mas3, N, &tmp1, sort_number, NULL))
                break;
        }
        else {
            if(0 == isSorted(mas, N, &tmp1, sort_number, NULL))
                break;
        }
        printf("correctly\n"); // желательно закомментировать при работе с маленькими массивами
    }
    printf(" - %d mistakes\n", tmp1);
    for (int i = 0; i < N; ++i) // печатаем индексы
        printf("%-8d", i);
    printf("\n");
    for (int i = 0; i < N; ++i) // исходный массив
        printf("%-8d", mas2[i]);
    printf("\n");
    for (int i = 0; i < N; ++i) // отсортированный массив
        printf("%-8d", mas[i]);
    printf("\n");
    if (howToCheck) { // как должен выглядеть результат
        for (int i = 0; i < N; ++i)
            printf("%-8d", mas3[i]);
        free(mas3);
    }
    else {
        qsort(mas2, N, sizeof(int), sort_number_non_sequre);
        for (int i = 0; i < N; ++i)
            printf("%-8d", mas2[i]);
    }
    free(mas);
    printf("\n");
}

void test() {
    int tmp1, tmp2, N;
    FILE *file = fopen("input.txt", "r");
    fscanf(file, "%d", &N);
    int *mas = (int *) malloc(N * sizeof(int)), *mas2 = (int *) malloc(N * sizeof(int));
    for (int i = 0; i < N; ++i)
        fscanf(file, "%d", &(mas[i]));
    memcpy(mas2, mas, N * sizeof(int));

    for (int i = 0; i < N; ++i)
        printf("%d ", mas[i]);
    printf("\n");

    //pair_insertion_bin_sort(mas, &tmp1, &tmp2, N, sizeof(int), sort_number, NULL);

    for (int i = 0; i < N; ++i)
        printf("%d ", mas[i]);
    qsort(mas2, N, sizeof(int), sort_number_non_sequre);
    if (compareWithSorted(mas, mas2, N, &tmp1, sort_number, NULL))
        printf("- correctly\n");
    else {
        printf(", not correct, %d mistake(s)!\n", tmp1);
        for (int i = 0; i < N; ++i)
            printf("%d ", mas2[i]);
        printf("- correct option\n");
    }

    fclose(file);
    free(mas);
}

void benchmark(int *mas, char *folderNameWithSlash, const int numOfDimensions,
            const int numOfDuplicatesInEachDimension, const int *dim, char toOutput) {
    int tmp1, tmp2;
    clock_t start, end;
    for (int i = 0; i < numOfDimensions; ++i) {
        for (int j = 1; j <= numOfDuplicatesInEachDimension; ++j) {
            char name[25] = "", tmp[20] = ""; // формируем название файла
            sprintf(name, "%s", folderNameWithSlash);
            sprintf(tmp, "%d", dim[i]);
            sprintf(tmp + strlen(tmp), "%c", '_');
            sprintf(tmp + strlen(tmp), "%d", j);
            sprintf(tmp + strlen(tmp), "%s", ".txt");
            sprintf(name + strlen(name), "%s", tmp);

            FILE *file = fopen(name, "r");
            for (int k = 0; k < dim[i]; ++k)
                fscanf(file, "%d", &(mas[k]));
            fclose(file);

            start = clock();

            // ВНИМАНИЕ: в случае смены сортировки это же нужно сделать и после
            // цикла по j (в 2 местах: строки 215 и 226)!!!

            // вспомогательные
            //insertion_sort(mas, &tmp1, dim[i], sizeof(int), sort_number, NULL);
            //insertion_sort_end_to_end_copying(mas, dim[i], sizeof(int), sort_number, NULL);
            //insertion_bin_sort(mas, &tmp1, dim[i], sizeof(int), sort_number, NULL);
            //insertion_bin_sort_on_ptrs(mas, &tmp1, dim[i], sizeof(int), sort_number, NULL);
            //pair_insertion_sort(mas, &tmp1, &tmp2, dim[i], sizeof(int), sort_number, NULL);
            //pair_insertion_bin_sort(mas, &tmp1, &tmp2, dim[i], sizeof(int), sort_number, NULL);
            // быстрые сортировки
            //my_qsort_simple(mas, dim[i], sizeof(int), sort_number, NULL);
            //my_qsort_good_mid(mas, dim[i], sizeof(int), sort_number, NULL);
            //my_qsort_with_insert(mas, dim[i], sizeof(int), sort_number, NULL);
            my_qsort(mas, dim[i], sizeof(int), sort_number, NULL);
            // пирамидальная сортировка
            //my_heapSort(mas, dim[i], sizeof(int), sort_number, NULL);
            // сортировки слиянием и тимсорт
            //my_mergeSort_simple(mas, dim[i], sizeof(int), sort_number, NULL);
            //my_mergeSort_with_insert(mas, dim[i], sizeof(int), sort_number, NULL);
            //my_mergeSort(mas, dim[i], sizeof(int), sort_number, NULL);
            //my_timsort(mas, dim[i], sizeof(int), sort_number, NULL);
            // cubesorts
            //my_cubesort(mas, dim[i], sizeof(int), sort_number, NULL);
            //my_cubesort_non_recurs(mas, dim[i], sizeof(int), sort_number, NULL);
            // библиотечные быстрые сортировки
            //qsort(mas, dim[i], sizeof(int), sort_number_non_sequre);
            //qsort_s(mas, dim[i], sizeof(int), sort_number, NULL);

            end = clock();

            printf("N = %d, #%d, time = %ldms", dim[i], j, end - start);
            if (isSorted(mas, dim[i], &tmp1, sort_number, NULL))
                printf(", correctly\n");
            else
                printf(", not correct, %d mistake(s)!\n", tmp1);

            if (toOutput) {
                sprintf(name, "%s", "outp/");
                sprintf(name + strlen(name), "%s", tmp);
                FILE *file2 = fopen(name, "w");
                if (name[3] == '1')
                    for (int k = 0; k < dim[i]; ++k)
                        fprintf(file2, "%-8d", mas[k]);
                else
                    for (int k = 0; k < dim[i]; ++k)
                        fprintf(file2, "%-16d", mas[k]);
                fclose(file2);
            }
        }
        // сортируем набор элементов по возрастанию
        for (int j = 0; j < dim[i]; ++j)
            mas[j] = j;
        start = clock();
        my_qsort(mas, dim[i], sizeof(int), sort_number, NULL);
        end = clock();
        printf("N = %d, forward order, time = %ldms", dim[i], end - start);
        if (isSorted(mas, dim[i], &tmp1, sort_number, NULL))
            printf(", correctly\n");
        else
            printf(", not correct, %d mistake(s)!\n", tmp1);
        // сортируем набор элементов по убыванию
        for (int j = 0, val = dim[i] - 1; j < dim[i]; ++j, --val)
            mas[j] = val;
        start = clock();
        my_qsort(mas, dim[i], sizeof(int), sort_number, NULL);
        end = clock();
        printf("N = %d, reverse order, time = %ldms", dim[i], end - start);
        if (isSorted(mas, dim[i], &tmp1, sort_number, NULL))
            printf(", correctly\n");
        else
            printf(", not correct, %d mistake(s)!\n", tmp1);
    }
}



int main() {
    printf("Start:\n");
    const int N = 5000000; // 50000, 100000, 500000, 1000000, 5000000
    int *mas = (int *) malloc(N * sizeof(int));
    const int dim[] = {N}; // здесь может быть указано несколько размерностей

    tests(100000, 1); // последний параметр: 1 - сравнение с тем же исходным
        // массивом, но отсортированным встроенной сортировкой, 0 - для проверки
        // на неубываемость по функции сравнения элементов
    //test();
    //benchmark(mas, "inp32/", 1, 5, dim, 0); // менять между inp16/ и inp32/
        // последний параметр 0, если не нужно выводить результат в файл, иначе 1

    free(mas);
    return EXIT_SUCCESS;
}



int sort_number(void *arg, const void *a, const void *b) {
    return *(int *) a - *(int *) b;
}

int sort_number_non_sequre(const void *a, const void *b) {
    return *(int *) a - *(int *) b;
}
