# Reaction-Time Tester

## Project brief

Build a game that waits a random amount of time, signals “go,” and measures how quickly the player reacts. The kit provides five pushbuttons, 25 single-color LEDs, and two bare RGB LEDs.

## Required behavior

- Provide a clear ready state.
- Use a countdown or other unambiguous start signal.
- Measure reaction time in milliseconds.
- Handle an early press as a false start.
- Give feedback using LEDs and/or RGB LEDs.
- Allow the player to try again without resetting the board.

## Hardware

- ESP32
- At least one button; using all five is encouraged
- At least one LED; a row of LEDs or both RGB LEDs is encouraged
- One resistor for every bare LED channel
- Breadboard and jumper wires

The 5-button/25-LED/2-RGB-LED combination is available in the kit lists supplied with this course. A bare 4-pin RGB LED needs a separate resistor for each color channel.

## Design questions

- How will you prevent switch bounce from counting as several presses?
- What should happen if the player presses before the signal?
- How will you show the best score?
- What timing approach will you use instead of delaying through the reaction window?

## Suggested pin starting point

Buttons: GPIO 13, 14, 16, 17, 18. LEDs: GPIO 19, 21, 22, 23, 25. RGB channels: GPIO 26, 27, 32, 33, 4, 5. Verify your own wiring before flashing.

## Acceptance test

Run five trials. The tester must detect a false start, record five valid reaction times, and reset for a new round without reflashing.

