#ifndef REMOTE_CONTROL_H
#define REMOTE_CONTROL_H

#include "../commonHeaders.h"

typedef struct
{
    uint8_t operationCode;
    uint8_t operationType;
    int16_t value;
}
RemoteControlCommand;

void processRemoteInput();

#endif