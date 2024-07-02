#include "cli_parser.h"
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void handle_option_a(const char *arg) {
    printf("Option 'a' with argument: %s\n", arg);
}

void handle_option_b(const char *arg) {
    printf("Option 'b' with argument: %s\n", arg);
}

void handle_option_c(const char *arg) {
    printf("Option 'c' with argument: %s\n", arg);
}

// 定义选项数组
const struct option options[] = {{"option_a", required_argument, NULL, 'a'},
                                 {"option_b", required_argument, NULL, 'b'},
                                 {"option_c", no_argument, NULL, 'c'},
                                 {NULL, 0, NULL, 0}};

// 解析命令行参数
void parse_arguments(int argc, char **argv) {
    int opt;
    while ((opt = getopt_long(argc, argv, "a:b:c", options, NULL)) != -1) {
        switch (opt) {
        case 'a':
            handle_option_a(optarg);
            break;
        case 'b':
            handle_option_b(optarg);
            break;
        case 'c':
            handle_option_c(NULL);
            break;
        default:
            fprintf(
                stderr,
                "Usage: %s [--option_a ARG] [--option_b ARG] [--option_c]\n",
                argv[0]);
            exit(EXIT_FAILURE);
        }
    }
}
