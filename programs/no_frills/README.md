No Frills
=========

A program that tries to be useful, without having too many features.

Modes:
------

* Normal: Medium intensity light
* High: Full intensity light
* Strobe: Flash with full intensity in variable frequency of about 10 Hz
* Nightlight: Turn off light after 10 seconds and only keep the green LED on,
    unless the acceleration sensor picks up movement
* Manual Nightlight: Same as above, but using clicks instead of movement

Switching between nodes:
------------------------
There are short clicks (< 300 ms), long clicks (< 700 ms) and permanent
button-pressing. Generally, to turn the light on or off, a short click is used.
To switch modes, a long click is used. When the button is held for longer than
the duration of a long click, the next-higher mode is selected for the duration
of the button-press only.

Here is how it works in detail:

* Hexbright off
    * Short click: Turn on
    * Long click: Go into nightlight mode
    * Permanent: Turn on until button is no longer held
* Hexbright is on
    * Short click: Turn off
    * Long click: Go into full intensity mode
    * Permanent: Go into full intensity mode until button is no longer held
* Hexbright is in full intensity mode
    * Short click: Turn off
    * Long click: Go back to normal intensity
    * Permanent: Strobe until button is no longer held
* Hexbright in nightlight mode
    * Short click: Turn off
    * Long click: Go into manual nightlight mode
    * Permanent: Turn off light until button is no longer held
* Hexbright in permanent nightlight mode
    * Short click: Turn off
    * Long click: Go back into (automatic) nightlight mode

Battery status:
---------------
When turned on, the red LED will blink for half a second every 15 seconds if
the battery is running low.
During charging the green LED will blink and change to perma-on when fully
charged. To avoid confusion with the nightlight mode, the charging indicator is
only enabled when the Hexbright is turned off/not in use.
