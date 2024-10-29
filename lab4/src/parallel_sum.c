#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <getopt.h>
#include <time.h>
#include "utils.h"
#include "sum.h"  

int Sum(const struct SumArgs *args);

void *ThreadSum(void *args) {
    struct SumArgs *sum_args = (struct SumArgs *)args;
    int *result = malloc(sizeof(int));
    *result = Sum(sum_args);
    return (void *)result;     
}

int main(int argc, char **argv) {
    uint32_t threads_num = 0;
    uint32_t array_size = 0;
    uint32_t seed = 0;

    static struct option options[] = {
        {"threads_num", required_argument, 0, 0},
        {"seed", required_argument, 0, 0},
        {"array_size", required_argument, 0, 0},
        {0, 0, 0, 0}
    };

    int option_index = 0;
    while (1) {
        int c = getopt_long(argc, argv, "", options, &option_index);
        if (c == -1) break;

        switch (c) {
            case 0:
                if (option_index == 0) {
                    threads_num = atoi(optarg);
                } else if (option_index == 1) {
                    seed = atoi(optarg);
                } else if (option_index == 2) {
                    array_size = atoi(optarg);
                }
                break;
            default:
                printf("Invalid argument\n");
                return 1;
        }
    }

    if (threads_num == 0 || array_size == 0) {
        printf("Usage: %s --threads_num <num> --seed <num> --array_size <num>\n", argv[0]);
        return 1;
    }

    int *array = malloc(sizeof(int) * array_size);
    GenerateArray(array, array_size, seed);

    pthread_t threads[threads_num];
    struct SumArgs args[threads_num];
    int part_size = array_size / threads_num;

    clock_t start_time = clock();

    for (uint32_t i = 0; i < threads_num; i++) {
        args[i].array = array;
        args[i].begin = i * part_size;
        args[i].end = (i == threads_num - 1) ? array_size : (i + 1) * part_size;

        if (pthread_create(&threads[i], NULL, ThreadSum, (void *)&args[i])) {
            printf("Error: pthread_create failed!\n");
            return 1;
        }
    }

    int total_sum = 0;

    for (uint32_t i = 0; i < threads_num; i++) {
        int *sum_result;
        pthread_join(threads[i], (void **)&sum_result);
        total_sum += *sum_result;
        free(sum_result);  
    }

    clock_t end_time = clock();
    double time_taken = ((double)(end_time - start_time)) / CLOCKS_PER_SEC;

    free(array);
    printf("Total: %d\n", total_sum);
    printf("Time taken: %f seconds\n", time_taken);
    return 0;
}
