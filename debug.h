#ifndef DEBUG_H
#define DEBUG_H

#include <sys/time.h>
#include <stdio.h>
#include <stdarg.h>

#define KNRM  "\x1B[0m"
#define KRED  "\x1B[31m"
#define KGRN  "\x1B[32m"
#define KYEL  "\x1B[33m"
#define KBLU  "\x1B[34m"
#define KMAG  "\x1B[35m"
#define KCYN  "\x1B[36m"
#define KWHT  "\x1B[37m"
#define KRST  "\033[0m"

#define VERBOSE_LVL 0

#define DBG_LV0 0 //Normal.
#define DBG_LV1 1 //More in depth.
#define DBG_LV2 2 //More in depth.
#define DBG_LV3 3 //Opcodes.

#define DBG_CPU 0
#define DBG_VID 1
#define DBG_APP 2

void dbgPrint(int lvl, int src, const char *fmt, ...);

#endif
