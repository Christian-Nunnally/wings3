
#include "../../Graphics/Effects/fireEffect.h"
#include "../../Graphics/palettes.h"
#include "../../Graphics/directionMaps.h"
#include "../../Graphics/transformMaps.h"
#include "../../Utility/fastRandom.h"
#include "../../settings.h"

const uint32_t gamma_lut[256] = {
     0,1355,2201,2923,3576,4180,4749,5290,5808,6308,6790,7259,7715,8159,8594,9019,
  9436,9845,10247,10642,11031,11414,11792,12165,12533,12896,13255,13610,13961,14308,14651,14992,
  15329,15662,15993,16321,16646,16968,17288,17605,17920,18232,18543,18851,19156,19460,19762,20062,
  20359,20655,20950,21242,21533,21822,22109,22395,22679,22962,23243,23523,23801,24078,24354,24628,
  24901,25173,25443,25713,25981,26248,26513,26778,27041,27304,27565,27825,28084,28343,28600,28856,
  29111,29365,29619,29871,30122,30373,30623,30872,31120,31367,31613,31858,32103,32347,32590,32832,
  33074,33315,33555,33794,34033,34270,34508,34744,34980,35215,35449,35683,35916,36149,36381,36612,
  36842,37072,37302,37530,37759,37986,38213,38440,38665,38891,39115,39339,39563,39786,40009,40231,
  40452,40673,40894,41114,41333,41552,41770,41988,42206,42423,42639,42855,43071,43286,43501,43715,
  43929,44142,44355,44567,44779,44991,45202,45413,45623,45833,46043,46252,46460,46669,46876,47084,
  47291,47498,47704,47910,48116,48321,48526,48730,48934,49138,49341,49544,49747,49949,50151,50353,
  50554,50755,50955,51155,51355,51555,51754,51953,52152,52350,52548,52745,52943,53140,53336,53533,
  53729,53924,54120,54315,54510,54704,54899,55092,55286,55479,55673,55865,56058,56250,56442,56634,
  56825,57016,57207,57397,57588,57778,57968,58157,58346,58535,58724,58912,59100,59288,59476,59663,
  59851,60038,60224,60411,60597,60783,60968,61154,61339,61524,61709,61893,62077,62261,62445,62629,
  62812,62995,63178,63361,63543,63725,63907,64089,64270,64452,64633,64814,64994,65175,65355,65535,
  };

int lastTimeFireMovedUp = 0;
const int FireMovementFrameSpeed = 50;
Color fireEffect(int pixelIndex, Effect *effect, int flameDecay, int sparks)
{
    static uint16_t heat1[TOTAL_LEDS];
    static uint16_t heat2[TOTAL_LEDS];
    static uint16_t* heatAPointer[TOTAL_LEDS];
    static uint16_t* heatBPointer[TOTAL_LEDS];
    static int cooldown;
    static bool shouldFireMoveUpThisFrame;
    static bool currentHeatBackBuffer;

    if (pixelIndex == 0)
    {
      (*effect->transformMap1) = normalTransformMapX;
      (*effect->transformMap2) = normalTransformMapY;

      if (shouldFireMoveUpThisFrame) 
      {
        shouldFireMoveUpThisFrame = false;
        if (currentHeatBackBuffer)
        {
          *heatAPointer = heat1;
          *heatBPointer = heat2;
        }
        else 
        {
          *heatAPointer = heat2;
          *heatBPointer = heat1;
        }
        currentHeatBackBuffer = !currentHeatBackBuffer;
      }
      cooldown = fastRandomInteger(flameDecay * *(effect->frameTimeDelta));
      lastTimeFireMovedUp += *(effect->frameTimeDelta);
      if (lastTimeFireMovedUp > FireMovementFrameSpeed)
      {
        lastTimeFireMovedUp = 0;
        shouldFireMoveUpThisFrame = true;
      }
    }

    if(cooldown > (*heatAPointer)[pixelIndex]) (*heatAPointer)[pixelIndex] = 0;
    else (*heatAPointer)[pixelIndex] -= cooldown;

    if ((isBottomLed[pixelIndex] != -1) && fastRandomInteger(30000) < sparks * *(effect->frameTimeDelta)) 
    {
      (*heatAPointer)[pixelIndex] = (uint16_t)fastRandomInteger(30000) + 35535;
    }

    if (shouldFireMoveUpThisFrame)
    {
      uint16_t southernLed1 = southernLedMap[pixelIndex];
      if (southernLed1 != TOTAL_LEDS && fastRandomByte() < 100)
      {
        uint16_t southernLed2 = southernLedMap[southernLed1];
        if (southernLed2 != TOTAL_LEDS) (*heatBPointer)[pixelIndex] = (((*heatAPointer)[southernLed1] / 2) + ((*heatAPointer)[southernLed2] / 2));// + ((*heatAPointer)[southernLed2] / 3));
        else (*heatBPointer)[pixelIndex] = (*heatAPointer)[southernLed1];
      }
      else (*heatBPointer)[pixelIndex] = (*heatAPointer)[pixelIndex];
    }
    uint16_t brightness = gamma_lut[(*heatAPointer)[pixelIndex] >> 8];
    return colorFromPalette(((*heatAPointer)[pixelIndex] >> 10) + effect->currentPaletteOffset, brightness);
}