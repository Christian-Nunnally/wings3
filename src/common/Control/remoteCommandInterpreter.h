#ifndef REMOTE_COMMAND_INTERPRETER_H
#define REMOTE_COMMAND_INTERPRETER_H

#include "../commonHeaders.h"
#include "remoteControl.h"

#define REMOTE_OPERATION_CODE_NO_OP 0
#define REMOTE_OPERATION_CODE_CONTROL_LEDS 1
#define REMOTE_OPERATION_CODE_CONTROL_MICROPHONE 2
#define REMOTE_OPERATION_CODE_CONTROL_SPEED 3
#define REMOTE_OPERATION_CODE_CONTROL_BRIGHTNESS 4
#define REMOTE_OPERATION_CODE_CONTROL_MOVEMENT_DETECTION 5
#define REMOTE_OPERATION_CODE_CONTROL_STEP_DETECTION 6
#define REMOTE_OPERATION_CODE_CONTROL_TEMPORAL_FADE 7
#define REMOTE_OPERATION_CODE_CONTROL_SECONDARY_EFFECT 8
#define REMOTE_OPERATION_CODE_CONTROL_BACKGROUND_EFFECT 9
#define REMOTE_OPERATION_CODE_CONTROL_RANDOM_EFFECT_CHANGE 10
#define REMOTE_OPERATION_CODE_CONTROL_PRESET 11
#define REMOTE_OPERATION_CODE_EFFECT_TRIGGER 12

#define REMOTE_OPERATION_TYPE_NOT_APPLICABLE 0
#define REMOTE_OPERATION_TYPE_GET 1
#define REMOTE_OPERATION_TYPE_SET 2
#define REMOTE_OPERATION_TYPE_INCREMENT 3
#define REMOTE_OPERATION_TYPE_DECREMENT 4
#define REMOTE_OPERATION_TYPE_ENABLE 5
#define REMOTE_OPERATION_TYPE_DISABLE 6

void interpretRemoteCommand(RemoteControlCommand command);

#endif