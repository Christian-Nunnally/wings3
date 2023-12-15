#include <iostream>
#include "../tests/testLeds.h"
#include "../src/IO/leds.h"
#include "../src/Graphics/effectController.h"
#include "../src/Utility/time.h"
#include <chrono>
#include <cstdint>
//#include "../src/microphone.h"

using namespace std::chrono;

int main() {
    setupEffects();
    int i = 0;
    while(true)
    {
        setTime(duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count());
        incrementEffectFrame();
        renderLeds();
        std::cout << "status,";
        std::cout << "frame,";
        std::cout << i++;
        std::cout << "\n";
    }
    return 0;
}