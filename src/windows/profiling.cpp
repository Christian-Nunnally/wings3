#include "profiling.h"
#include "tracing.h"

clock_t start;

void startProfile()
{
    start = clock();
}

void stopProfile()
{
    D_emitFloatMetric(METRIC_NAME_ID_PROFILE_TIME, ((double)(clock() - start)) / CLOCKS_PER_SEC * 1000);
}