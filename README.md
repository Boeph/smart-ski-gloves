# Smart Ski Gloves

EngHacks 2026 prototype (Jan–Mar 2026): ski gloves that control your phone by pinching your fingers, and trigger an SOS shortcut when you fall.

An Arduino UNO R4 Minima acts as a USB HID keyboard and sends shortcuts to the connected device.

## Controls

| Input | Pin | Action |
| --- | --- | --- |
| Force sensor (finger pinch) | A1 | 1 press: play/pause, 2 presses: next track, 3 presses: previous track |
| Piezo sensor (palm impact) | A0 | Fall detected: SOS shortcut |
| Button | D2 / D3 | Volume up / down (hold to repeat) |
| Button | D4 | Hold 0.8 s for Siri |

The force sensor reading is converted to grams (10k pulldown, 5 V) and counts as a press above a 200 g threshold, found through testing. Presses are grouped into a gesture once there's a 500 ms gap, so more gestures can be mapped to each finger.

Heat from palm hand warmers is spread to the fingertips with conductive copper fabric.

## Build

Open `glove.ino` in the Arduino IDE, select **Arduino UNO R4 Minima**, and upload.
