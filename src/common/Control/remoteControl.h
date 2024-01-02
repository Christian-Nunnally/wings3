#ifndef REMOTE_CONTROL_H
#define REMOTE_CONTROL_H

#include "../commonHeaders.h"

typedef struct
{
    uint8_t operationCode;
    int16_t value;
    uint8_t flags;
}
RemoteControlCommand;

void processRemoteInput();

#endif