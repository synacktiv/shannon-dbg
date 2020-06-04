#ifndef DBG_H
#define DBG_H

#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>

#define err(f_, ...)                 \
	printf("[\033[31;1m!\033[0m] "); \
	printf(f_, ##__VA_ARGS__);       \
	fflush(stdout);
#define ok(f_, ...)                  \
	printf("[\033[32;1m+\033[0m] "); \
	printf(f_, ##__VA_ARGS__);       \
	fflush(stdout);
#define info(f_, ...)                \
	printf("[\033[34;1m-\033[0m] "); \
	printf(f_, ##__VA_ARGS__);       \
	fflush(stdout);
#define warn(f_, ...)                \
	printf("[\033[33;1mw\033[0m] "); \
	printf(f_, ##__VA_ARGS__);       \
	fflush(stdout);


#endif