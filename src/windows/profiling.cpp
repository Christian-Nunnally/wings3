#include "profiling.h"
#include <time.h>
#include "../common/settings.h"
#include "../common/IO/tracing.h"

clock_t start;

void startProfile()
{
    start = clock();
}

void stopProfile()
{
    D_emitMetric(METRIC_NAME_ID_PROFILE_TIME, (float)(clock() - start));
}