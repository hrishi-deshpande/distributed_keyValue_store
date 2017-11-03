#include "genericApi.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

void die(const char* fmt, ...) {
	va_list args;

	va_start(args, fmt);
	vfprintf(stderr, fmt, args);
	fputc('\n', stderr);
	exit (-1);
}
