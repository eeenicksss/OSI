#include <ctype.h>
#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <getopt.h>
#include <signal.h>

#include "find_min_max.h"
#include "utils.h"

int *child_pids; // Массив для хранения идентификаторов дочерних процессов
int pnum = -1; // Число процессов

void kill_all_children(int signum) {
    for (int i = 0; i < pnum; i++) {
        kill(child_pids[i], SIGKILL); // Убиваем каждый дочерний процесс
    }
}

int main(int argc, char **argv) {
    int seed = -1;
    int array_size = -1;
    int timeout = 0; // Таймаут, по умолчанию 0 (без таймаута)
    bool with_files = false;

    while (true) {
        int current_optind = optind ? optind : 1;

        static struct option options[] = {{"seed", required_argument, 0, 0},
                                          {"array_size", required_argument, 0, 0},
                                          {"pnum", required_argument, 0, 0},
                                          {"timeout", required_argument, 0, 0},
                                          {"by_files", no_argument, 0, 'f'},
                                          {0, 0, 0, 0}};

        int option_index = 0;
        int c = getopt_long(argc, argv, "f", options, &option_index);

        if (c == -1) break;

        switch (c) {
            case 0:
                switch (option_index) {
                    case 0:
                        seed = atoi(optarg);
                        if (seed <= 0) {
                            printf("Seed should be a positive number\n");
                            return 1;
                        }
                        break;
                    case 1:
                        array_size = atoi(optarg);
                        if (array_size <= 0) {
                            printf("Array size should be a positive number\n");
                            return 1;
                        }
                        break;
                    case 2:
                        pnum = atoi(optarg);
                        if (pnum <= 0) {
                            printf("Number of processes should be a positive number\n");
                            return 1;
                        }
                        break;
                    case 3:
                        timeout = atoi(optarg);
                        if (timeout <= 0) {
                            printf("Timeout should be a positive number\n");
                            return 1;
                        }
                        break;
                    case 4:
                        with_files = true;
                        break;

                    default:
                        printf("Index %d is out of options\n", option_index);
                }
                break;
            case 'f':
                with_files = true;
                break;

            case '?':
                break;

            default:
                printf("getopt returned character code 0%o?\n", c);
        }
    }

    if (optind < argc) {
        printf("Has at least one no option argument\n");
        return 1;
    }

    if (seed == -1 || array_size == -1 || pnum == -1) {
        printf("Usage: %s --seed \"num\" --array_size \"num\" --pnum \"num\" [--timeout \"num\"]\n", argv[0]);
        return 1;
    }

    int *array = malloc(sizeof(int) * array_size);
    GenerateArray(array, array_size, seed);
    int active_child_processes = 0;

    struct timeval start_time;
    gettimeofday(&start_time, NULL);

    int pipefd[2 * pnum];
    int part_size = array_size / pnum;

    if (!with_files) {
        for (int i = 0; i < pnum; i++) {
            pipe(pipefd + 2 * i);
        }
    }

    child_pids = malloc(sizeof(int) * pnum); // Храним PID дочерних процессов

    for (int i = 0; i < pnum; i++) {
        pid_t child_pid = fork();
        if (child_pid >= 0) {
            active_child_processes += 1;
            if (child_pid == 0) {
                int begin = i * part_size;
                int end = (i == pnum - 1) ? array_size : (i + 1) * part_size;
                struct MinMax min_max = GetMinMax(array, begin, end);

                if (with_files) {
                    char file_name[255];
                    snprintf(file_name, sizeof(file_name), "min_max_%d.txt", i);
                    FILE *file = fopen(file_name, "w");
                    fwrite(&min_max, sizeof(struct MinMax), 1, file);
                    fclose(file);
                } else {
                    close(pipefd[2 * i]);
                    write(pipefd[2 * i + 1], &min_max, sizeof(struct MinMax));
                    close(pipefd[2 * i + 1]);
                }
                return 0;
            } else {
                child_pids[i] = child_pid;
            }
        } else {
            printf("Fork failed!\n");
            return 1;
        }
    }

    if (timeout > 0) {
        signal(SIGALRM, kill_all_children); // Устанавливаем обработчик SIGALRM
        alarm(timeout); // Запускаем таймер
    }

    while (active_child_processes > 0) {
        pid_t child_pid = waitpid(-1, NULL, WNOHANG);
        if (child_pid > 0) {
            active_child_processes--;
        }
    }

    struct MinMax min_max;
    min_max.min = INT_MAX;
    min_max.max = INT_MIN;

    for (int i = 0; i < pnum; i++) {
        struct MinMax local_min_max;
        if (with_files) {
            char file_name[255];
            snprintf(file_name, sizeof(file_name), "min_max_%d.txt", i);
            FILE *file = fopen(file_name, "r");
            fread(&local_min_max, sizeof(struct MinMax), 1, file);
            fclose(file);
        } else {
            close(pipefd[2 * i + 1]);
            read(pipefd[2 * i], &local_min_max, sizeof(struct MinMax));
            close(pipefd[2 * i]);
        }

        if (local_min_max.min < min_max.min) min_max.min = local_min_max.min;
        if (local_min_max.max > min_max.max) min_max.max = local_min_max.max;
    }

    struct timeval finish_time;
    gettimeofday(&finish_time, NULL);

    double elapsed_time = (finish_time.tv_sec - start_time.tv_sec) * 1000.0;
    elapsed_time += (finish_time.tv_usec - start_time.tv_usec) / 1000.0;

    free(array);
    free(child_pids);

    printf("Min: %d\n", min_max.min);
    printf("Max: %d\n", min_max.max);
    printf("Elapsed time: %fms\n", elapsed_time);
    return 0;
}
