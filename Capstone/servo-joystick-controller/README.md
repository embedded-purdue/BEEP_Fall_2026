# Servo Joystick Controller

## Project brief

Use the joystick module to control the position of an SG90 servo. Add a useful interaction or visual feedback so this is a controller, not only a pass-through demo.

## Required behavior

- Read the joystick's X and/or Y axis with the ADC.
- Map the input to a safe servo angle.
- Include a dead zone or smoothing so the servo does not jitter at rest.
- Use the joystick switch or another control to change modes, set a limit, or trigger an action.
- Explain the servo's power and grounding arrangement.

## Hardware

- ESP32
- Joystick module
- SG90 servo
- External or suitable 5 V supply rail for the servo
- Common ground between the servo supply and ESP32

The servo signal is a 3.3 V GPIO signal, but the servo power must not come from a GPIO pin. Disconnect power before changing wiring.

## Design questions

- What are the useful minimum and maximum angles for your mechanism?
- How will you calibrate the joystick center?
- What does the joystick push switch do?
- How often should you update the servo?

## Suggested pin starting point

Joystick X: ADC1 GPIO 36. Joystick Y: ADC1 GPIO 39. Joystick switch: GPIO 32. Servo signal: GPIO 18. Use the servo's red wire for supply, brown/black for ground, and orange/yellow for signal, but confirm your part's wiring.

## Acceptance test

The servo must follow the joystick smoothly across its usable range, remain stable when released, and respond to the push switch or chosen mode control.

