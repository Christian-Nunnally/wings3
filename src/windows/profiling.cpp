#include "profiling.h"
#include <time.h>
#include "../common/settings.h"
#include "../common/IO/tracing.h"

clock_t start;

int numberOfProfilesLeftToTake = 0;
int sumOfProfileSamplesTaken = 0;
int totalProfileSamplesTaken = 0;

void startProfile(int numberOfSamples)
{
    if (numberOfProfilesLeftToTake == 0)
    {
        numberOfProfilesLeftToTake = numberOfSamples;
    }
    start = clock();
}

void stopProfile()
{
    sumOfProfileSamplesTaken += (int)(clock() - start);
    totalProfileSamplesTaken++;
    numberOfProfilesLeftToTake--;
    if (numberOfProfilesLeftToTake <= 0)
    {
        D_emitMetric(METRIC_NAME_ID_PROFILE_TIME, sumOfProfileSamplesTaken / totalProfileSamplesTaken);
        totalProfileSamplesTaken = 0;
        sumOfProfileSamplesTaken = 0;
        numberOfProfilesLeftToTake = 0;
    }
}