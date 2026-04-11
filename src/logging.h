#pragma once
#include <stdio.h>

#ifdef DEBUG
    #define LOG(fmt, ...) \
        fprintf(stdout, "[LOG] " fmt "\n", ##__VA_ARGS__)
#else
    #define LOG(fmt, ...) ((void)0)
#endif
