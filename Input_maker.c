#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define folderNameWithSlash "inp32/" // менять между inp16/ и inp32/
// в зависимости от имени папки автоматически будет выбрана размерность генерируемых чисел

int main() {    
    const int numOfDimensions = 1, numOfDuplicates = 5;
    time_t t;
    srand((unsigned) time(&t));

    int dim[] = {10}; // 50000, 100000, 500000, 1000000, 5000000
    for(int i = 0; i < numOfDimensions; ++i)
        for(int j = 1; j <= numOfDuplicates; ++j) {
            char name[25] = folderNameWithSlash; // формируем название файла
            sprintf(name + 6, "%d", dim[i]);
            sprintf(name + strlen(name), "%c", '_');
            sprintf(name + strlen(name), "%d", j);
            sprintf(name + strlen(name), "%s", ".txt");

            FILE *file = fopen(name, "w");
            if (NULL == file) {
                printf("ERROR: failed to open/create file!");
                return EXIT_FAILURE;
            }
            if (name[3] == '1')
                for (int k = 0; k < dim[i]; ++k)
                    fprintf(file, "%-8d", rand());
            else {
                int t; // но 7-ой справа бит всегда будет нулевым
                short *p1 = (short *) &t, *p2 = p1 + 1;
                for (int k = 0; k < dim[i]; ++k) {
                    *p1 = rand();
                    *p2 = rand();
                    fprintf(file, "%-16d", t);
                }
            }
            fclose(file);
        }
    printf("Files created successfully!");
    return EXIT_SUCCESS;
}
