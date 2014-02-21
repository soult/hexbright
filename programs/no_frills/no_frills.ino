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
#define BUTTON_LONG 700            // Duration of long button press in ms

#define NORMAL_LEVEL 350           // Normal intensity
#define HIGH_LEVEL MAX_LEVEL       // High intensity

#define STROBE_LOWER 83            // Minimum strobe interval in ms (83 ms = 12 Hz)
#define STROBE_UPPER 125           // Maximum strobe interval in ms (125 ms = 8 Hz)
#define STROBE_DURATION 20         // Duration of strobe in ms

#define NIGHTLIGHT_TOLERANCE 25    // Acceleration sensor tolerance
#define NIGHTLIGHT_IDLE 10000      // Switch to "idle" after that time in ms
#define NIGHTLIGHT_DIM 2500        // Duration of dimming phase in ms

#define LOW_POWER_INTERVAL 15000   // Time between low power warnings in ms

#define MODE_OFF 0
#define MODE_ON 1
#define MODE_HIGH 2
#define MODE_STROBE 3
#define MODE_NIGHTLIGHT 4
#define MODE_NIGHTLIGHT_MANUAL 5

hexbright hb;

int mode = 0;
BOOL pressed = false;
unsigned long timer = 0;

void setup() {
  hb = hexbright();
  hb.init_hardware();
}

void loop() {
  hb.update();
  int old_mode = mode;
  
  switch(mode) {
    case MODE_OFF:
      if(hb.button_pressed()) {
        mode = MODE_ON;
        pressed = true;
      } else if(hb.button_just_released()) {
        mode = MODE_ON;
      } else {
        print_charge(GLED);
      }
      break;
    case MODE_ON:
      if(hb.button_pressed() && !pressed) {
        if(hb.button_pressed_time() > BUTTON_SHORT) {
          mode = MODE_HIGH;
          pressed = true;
        }
      } else if(hb.button_just_released()) {
        if(pressed) {
          pressed = false;
          if(hb.button_pressed_time() > BUTTON_LONG) {
            mode = MODE_OFF;
          } else if(hb.button_pressed_time() > BUTTON_SHORT) {
            mode = MODE_NIGHTLIGHT;
          }
        } else {
          mode = MODE_OFF;
        }
      }
      break;
    case MODE_HIGH:
      if(hb.button_pressed() && !pressed) {
        if(hb.button_pressed_time() > BUTTON_LONG) {
          mode = MODE_STROBE;
          pressed = true;
        }
      } else if(hb.button_just_released()) {
        if(pressed) {
          if(hb.button_pressed_time() > BUTTON_LONG) {
            mode = MODE_ON;
          }
          pressed = false;
        } else {
          if(hb.button_pressed_time() < BUTTON_SHORT) {
            mode = MODE_OFF;
          } else if(hb.button_pressed_time() < BUTTON_LONG) {
            mode = MODE_ON;
          }
        }
      }
      break;
    case MODE_STROBE:
      if(!hb.button_pressed()) {
        mode = MODE_HIGH;
        pressed = false;
      } else if(timer < millis()) {
        timer = millis() + random(STROBE_LOWER, STROBE_UPPER);
        hb.set_light(MAX_LEVEL, 0, STROBE_DURATION);
      }
      break;
    case MODE_NIGHTLIGHT:
      if(hb.button_pressed()) {
        hb.set_light(CURRENT_LEVEL, 0, NOW);
      } else if(hb.button_just_released()) {
        if(hb.button_pressed_time() < BUTTON_SHORT) {
          mode = MODE_OFF;
        } else if(hb.button_pressed_time() < BUTTON_LONG) {
          mode = MODE_NIGHTLIGHT_MANUAL;
        } else {
          hb.set_light(CURRENT_LEVEL, NORMAL_LEVEL, NOW);
        }
      } else {
        if(hb.moved(NIGHTLIGHT_TOLERANCE)) {
          timer = millis() + NIGHTLIGHT_IDLE;
        }
        if(timer < millis()) {
          if(hb.get_led_state(GLED) == LED_OFF) {
            hb.set_light(CURRENT_LEVEL, 0, NIGHTLIGHT_DIM);
          }
          hb.set_led(GLED, 100);
        } else {
          if(hb.get_led_state(GLED) != LED_OFF) {
            hb.set_light(CURRENT_LEVEL, NORMAL_LEVEL, NOW);
            hb.set_led(GLED, 0, 0);
          }
        }
      }
      break;
    case MODE_NIGHTLIGHT_MANUAL:
      if(hb.button_just_released()) {
        if(hb.button_pressed_time() < BUTTON_SHORT) {
          mode = MODE_OFF;
        } else {
          mode = MODE_NIGHTLIGHT;
        }
      } else {
        hb.set_led(GLED, 100);
      }
      break;
  }
       
  if(old_mode != mode) {
    switch(mode) {
      case MODE_OFF:
        hb.set_light(CURRENT_LEVEL, OFF_LEVEL, NOW);
        break;
      case MODE_ON:
        hb.set_light(CURRENT_LEVEL, NORMAL_LEVEL, NOW);
        break;
      case MODE_HIGH:
        hb.set_light(CURRENT_LEVEL, HIGH_LEVEL, NOW);
        break;
      case MODE_STROBE:
        timer = millis();
        randomSeed(timer);
        break;
      case MODE_NIGHTLIGHT:
        timer = millis() + NIGHTLIGHT_IDLE;
        hb.set_light(CURRENT_LEVEL, NORMAL_LEVEL, 0);
        break;
      case MODE_NIGHTLIGHT_MANUAL:
        hb.set_light(CURRENT_LEVEL, 0, NIGHTLIGHT_DIM);
        hb.set_led(GLED, 100);
        break;
    }
  }

  if(hb.low_voltage_state() && hb.get_led_state(RLED) == LED_OFF) {
    hb.set_led(RLED, 500, LOW_POWER_INTERVAL);
  }
}
