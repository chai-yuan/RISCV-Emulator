#ifndef PARSE_ARGS_H
#define PARSE_ARGS_H

#include <getopt.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char    *bin;
    char    *machine;
    uint64_t step;
} Config;

typedef struct {
    const char *name;    // 长选项名称
    int         has_arg; // 是否需要参数
    int        *flag;    // 标志位
    int         val;     // 选项枚举值
    const char *help;    // 帮助信息
} OptionInfo;

static OptionInfo options[] = {{"bin", required_argument, 0, 'b', "Specify the binary path"},
                               {"machine", required_argument, 0, 'm', "Specify the machine type"},
                               {"step", required_argument, 0, 's', "Specify the step"},
                               {"help", no_argument, 0, 'h', "Display this help message"},
                               {0, 0, 0, 0, 0}};

static Config default_config = {
    .bin     = "test.bin",
    .machine = "qemu",
    .step    = -1,
};

void print_help(const char *program_name) {
    printf("Usage: %s [OPTIONS]\n", program_name);
    printf("Options:\n");

    // 遍历选项数组，自动生成帮助信息
    for (OptionInfo *opt = options; opt->name != 0; opt++) {
        printf("  --%-15s %s\n", opt->name, opt->help);
    }
}

int parse_args(int argc, char *argv[], Config *config) {
    int opt;
    int option_index = 0;

    // 将 OptionInfo 转换为 getopt_long 需要的 struct option 数组
    struct option long_options[sizeof(options) / sizeof(OptionInfo)];
    for (int i = 0; options[i].name != 0; i++) {
        long_options[i].name    = options[i].name;
        long_options[i].has_arg = options[i].has_arg;
        long_options[i].flag    = options[i].flag;
        long_options[i].val     = options[i].val;
    }

    // 配置默认设置
    memcpy(config, &default_config, sizeof(Config));

    while ((opt = getopt_long(argc, argv, "", long_options, &option_index)) != -1) {
        switch (opt) {
        case 'b':
            config->bin = optarg;
            break;
        case 'm':
            config->machine = optarg;
            break;
        case 's':
            sscanf(optarg, "%lu", &config->step);
            break;
        case 'h':
            print_help(argv[0]);
            exit(0);
        default:
            print_help(argv[0]);
            exit(1);
        }
    }

    return 0;
}

#endif
