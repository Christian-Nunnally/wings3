#include "remoteControl.h"
#include "remoteCommandInterpreter.h"

#ifdef RP2040
#include "../../rp2040/wirelessRemoteControlReceiver.h"
#else
#include "../../windows/socketRemoteControl.h"
#endif

void processRemoteInput()
{
    #ifdef RP2040
    if (isWirelessInputAvailable())
    {
        RemoteControlCommand command = getWirelessInputCommand();
        interpretRemoteCommand(command);
    }
    #else
    if (isSocketInputReady())
    {
        RemoteControlCommand command = getSocketInput();
        interpretRemoteCommand(command);
    }
    #endif
}