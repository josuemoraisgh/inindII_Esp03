#include <Arduino.h>
#include "InIndKit.h"

void setup()
{
    InIndKit.setup(115200);
}

void loop()
{
    InIndKit.loop();
}