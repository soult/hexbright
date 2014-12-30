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

// Duration of short button press in ms
#define BUTTON_SHORT 300

// Low intensity
#define LOW_LEVEL 50
// Normal intensity
#define NORMAL_LEVEL 350
// High intensity
#define HIGH_LEVEL MAX_LEVEL

// Sensitivity for the strobe "shake"
#define STROBE_ACCL_THRESH 150
// Minimum strobe interval in ms (83 ms = 12 Hz)
#define STROBE_LOWER 83
// Maximum strobe interval in ms (125 ms = 8 Hz)
#define STROBE_UPPER 125
// Duration of strobe in ms
#define STROBE_DURATION 20

// Time between low power warnings in ms
#define LOW_POWER_INTERVAL 15000

// Do not modify code below unless you know what you are doing

#define LIGHT_AXIS 1
#define ACCL_SAMPLE_RATE 8

#define MODE_OFF 0
#define MODE_LOW 1
#define MODE_NORMAL 2
#define MODE_HIGH 3
#define MODE_STROBE_PENDING 4
#define MODE_STROBE 5

hexbright hb;

int mode = 0;
unsigned long timer = 0;

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
      if(timer == 0) {
        timer = millis() + ACCL_SAMPLE_RATE * 4;
      } else if(timer < millis()) {
        timer = millis() + ACCL_SAMPLE_RATE;
        int minmax[] = {hb.vector(0)[1], hb.vector(0)[LIGHT_AXIS]};
        for(short i = 1; i < 4; i++) {
          if(hb.vector(i)[LIGHT_AXIS] < minmax[0]) {
            minmax[0] = hb.vector(i)[LIGHT_AXIS];
          } else if(hb.vector(i)[LIGHT_AXIS] > minmax[1]) {
            minmax[1] = hb.vector(i)[LIGHT_AXIS];
          }
        }
        if(minmax[1] - minmax[0] > STROBE_ACCL_THRESH) {
          timer = millis() + 2 * BUTTON_SHORT;
          mode = MODE_STROBE_PENDING;
        }
      }
      break;
    case MODE_NORMAL:
      if(hb.button_pressed()) {
        if(hb.button_pressed_time() > BUTTON_SHORT) {
          mode = MODE_HIGH;
          hb.set_light(CURRENT_LEVEL, HIGH_LEVEL, NOW);
        }
      } else if(hb.button_just_released()) {
        if(hb.button_pressed_time() < BUTTON_SHORT) {
          shutdown();
        }
      }
      break;
    case MODE_HIGH:
      if(hb.button_just_released()) {
        mode = MODE_NORMAL;
        hb.set_light(CURRENT_LEVEL, NORMAL_LEVEL, NOW);
      }
      break;
    case MODE_STROBE_PENDING:
      if(hb.button_just_released()) {
        mode = MODE_STROBE;
      } else if(timer < millis()) {
        mode = MODE_LOW;
      }
      break;
    case MODE_STROBE:
      if(timer < millis()) {
        timer = millis() + random(STROBE_LOWER, STROBE_UPPER);
        hb.set_light(MAX_LEVEL, 0, STROBE_DURATION);
      }
      if(hb.button_just_released()) {
        shutdown();
      }
      break;
  }

  if(mode != MODE_OFF && hb.low_voltage_state() && hb.get_led_state(RLED) == LED_OFF) {
    hb.set_led(RLED, 500, LOW_POWER_INTERVAL);
  }
}
