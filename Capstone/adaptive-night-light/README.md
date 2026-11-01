# Adaptive Night Light

## Project brief

Build a light that responds to external brightness. Use the two photoresistors to sense light, the potentiometer as a user-adjustable brightness or sensitivity control, and the RGB LED to show the lamp state (warm lighting or cool lighting or vairable lighting if you feel crazy).

## Required behavior

- Read at least one photoresistor through an ADC voltage divider.
- Use the potentiometer as a meaningful user control.
- Change LED brightness or color as the environment changes.
- Avoid visible flicker.
- Include a clearly defined dark/bright behavior.

## Hardware

- ESP32
- One or two photoresistors
- 10 kΩ fixed resistor for each photoresistor divider (from the resistor assortment)
- 10 kΩ potentiometer
- Bare 4-pin RGB LED
- Three current-limiting resistors for the RGB channels

For a divider, connect the photoresistor and fixed resistor between 3.3 V and ground, then read their midpoint with an ADC pin. Do not assume the voltage rises in darkness; it depends on which component is on which side of the divider.

## Design questions

- Is your light brighter in darkness, or does it indicate the measured brightness?
- How will you calibrate the sensor for your room?
- Should the potentiometer set maximum brightness, the threshold, or color?
- How will you keep ADC noise from making the output shimmer?

## Suggested pin starting point

Photoresistors: ADC1 GPIO 34 and 35. Potentiometer: ADC1 GPIO 32. RGB channels: GPIO 25, 26, and 27. Verify the RGB LED pin order and polarity from its datasheet or a low-current test.

## Acceptance test

Demonstrate stable output in bright and dim conditions, adjust the potentiometer to show its intended effect, and explain your resistor/divider choice.

