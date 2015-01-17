#include "debug.h"

void dbgPrint(int lvl, int src, const char *fmt, ...) {
#ifdef _DEBUG
    if(lvl <= VERBOSE_LVL)
    {
    va_list arg;
    int done;

    struct timeval tv;
    gettimeofday(&tv, NULL);

    va_start(arg, fmt);
    printf("[%i.%i] DEBUG - ", tv.tv_sec, tv.tv_usec);
    switch(src){
        case DBG_CPU:
            printf("%sCPU: ", KGRN);
        break;

        case DBG_VID:
            printf("%sVID: ", KBLU);
        break;

        case DBG_APP:
            printf("%sAPP: ", KMAG);
            break;

        default:
            printf("UNKNOWN: ");
        break;
    }
    vfprintf(stdout, fmt, arg);
    va_end(arg);
    printf("%s\n", KRST);
    }
    else {
    return;
    }
#endif
}
