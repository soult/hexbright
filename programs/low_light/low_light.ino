/*
This is free and unencumbered software released into the public domain.

Anyone is free to copy, modify, publish, use, compile, sell, or
distribute this software, either in source code form or as a compiled
binary, for any purpose, commercial or non-commercial, and by any
means.

In jurisdictions that recognize copyright laws, the author or authors
of this software dedicate any and all copyright interest in the
software to the public domain. We make this dedication for the benefit
of the public at large and to the detriment of our heirs and
successors. We intend this dedication to be an overt act of
relinquishment in perpetuity of all present and future rights to this
software under copyright law.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.

For more information, please refer to <http://unlicense.org/>
*/

#include <print_power.h>

#define BUILD_HACK
#include <hexbright.h>

#define BUTTON_SHORT 300           // Duration of short button press in ms

#define LOW_LEVEL 50               // Low intensity
#define NORMAL_LEVEL 350           // Normal intensity
#define HIGH_LEVEL MAX_LEVEL       // High intensity

#define LOW_POWER_INTERVAL 15000   // Time between low power warnings in ms

#define MODE_OFF 0
#define MODE_LOW 1
#define MODE_NORMAL 2
#define MODE_HIGH 3

hexbright hb;

int mode = 0;

void setup() {
  hb = hexbright();
  hb.init_hardware();
}

void shutdown() {
  mode = MODE_OFF,
  hb.set_light(CURRENT_LEVEL, OFF_LEVEL, NOW);
}

void loop() {
  hb.update();
  print_charge(GLED);
  
  switch(mode) {
    case MODE_OFF:
      if(hb.button_pressed()) {
        mode = MODE_LOW;
        hb.set_light(CURRENT_LEVEL, LOW_LEVEL, NOW);
      } else if(hb.button_just_released()) {
        mode = MODE_NORMAL;
        hb.set_light(CURRENT_LEVEL, NORMAL_LEVEL, NOW);
      }
      break;
    case MODE_LOW:
      if(hb.button_just_released()) {
        if(hb.button_pressed_time() < BUTTON_SHORT) {
          mode = MODE_NORMAL;
          hb.set_light(CURRENT_LEVEL, NORMAL_LEVEL, NOW);
        } else {
          shutdown();
        }
      }
      break;
    case MODE_NORMAL:
      if(hb.button_just_released()) {
        if(hb.button_released_time() < 2*BUTTON_SHORT) {
          mode = MODE_HIGH;
          hb.set_light(CURRENT_LEVEL, HIGH_LEVEL, NOW);          
        } else {
          shutdown();
        }
      }
      break;
    case MODE_HIGH:
      if(hb.button_just_released()) {
        shutdown();
      }
      break;
  }

  if(mode != MODE_OFF && hb.low_voltage_state() && hb.get_led_state(RLED) == LED_OFF) {
    hb.set_led(RLED, 500, LOW_POWER_INTERVAL);
  }
}
