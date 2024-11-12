#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <getopt.h> // для getopt_long

int mod;
long long factorial_result = 1;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

struct FactorialPart {
    int start;
    int end;
};

// Функция для вычисления части факториала
void *compute_partial_factorial(void *args) {
    struct FactorialPart *range = (struct FactorialPart *)args;
    long long partial_result = 1;

    for (int i = range->start; i <= range->end; i++) {
        partial_result = (partial_result * i) % mod;
    }

    pthread_mutex_lock(&mutex);
    factorial_result = (factorial_result * partial_result) % mod;
    pthread_mutex_unlock(&mutex);

    return NULL;
}

int main(int argc, char **argv) {
    int k = 0;
    int pnum = 0;

    // Параметры для getopt_long
    const struct option long_options[] = {
        {"pnum", required_argument, 0,  'p' },
        {"mod",  required_argument, 0,  'm' },
        {0,      0,                 0,  0   }
    };

    // Парсинг аргументов с использованием getopt_long
    int opt;
    while ((opt = getopt_long(argc, argv, "k:", long_options, NULL)) != -1) {
        switch (opt) {
            case 'k':
                k = atoi(optarg);
                break;
            case 'p':
                pnum = atoi(optarg);
                break;
            case 'm':
                mod = atoi(optarg);
                break;
            default:
                printf("Usage: %s -k <number> --pnum=<threads_num> --mod=<mod_value>\n", argv[0]);
                return 1;
        }
    }

    // Проверка корректности ввода
    if (k == 0 || pnum == 0 || mod == 0) {
        printf("Invalid arguments. Ensure all parameters are specified correctly.\n");
        printf("Usage: %s -k <number> --pnum=<threads_num> --mod=<mod_value>\n", argv[0]);
        return 1;
    }

    pthread_t threads[pnum];
    struct FactorialPart parts[pnum];
    int range_size = k / pnum;

    // Разделение задачи на части для каждого потока
    for (int i = 0; i < pnum; i++) {
        parts[i].start = i * range_size + 1;
        parts[i].end = (i == pnum - 1) ? k : (i + 1) * range_size;

        if (pthread_create(&threads[i], NULL, compute_partial_factorial, &parts[i]) != 0) {
            perror("Error creating thread");
            return 1;
        }
    }

    // Ожидание завершения потоков
    for (int i = 0; i < pnum; i++) {
        if (pthread_join(threads[i], NULL) != 0) {
            perror("Error joining thread");
            return 1;
        }
    }

    pthread_mutex_destroy(&mutex);

    printf("Factorial of %d modulo %d is: %lld\n", k, mod, factorial_result);
    return 0;
}
