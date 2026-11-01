# Week 8 - Capstone Project

Firstly, congratulations on making it to the end of the course. It has been a pleasure to work with all of you and teach all of you.

This week is a capstone project week (I have mentinoed this in-person during Week 5 Lab). You guys can either choose **one** project (or make your own project), design the circuit, write the firmware, and demonstrate a working prototype. Unlike earlier weeks, the starter code is intentionally small: it gives you a project shell and a few hardware setup hints, but the behavior and user experience are yours to design.

## Choose one project

1. [Reaction-Time Tester](reaction-time-tester/README.md)
2. [Adaptive Night Light](adaptive-night-light/README.md)
3. [Servo Joystick Controller](servo-joystick-controller/README.md)

Each project folder contains a short brief and an ESP-IDF starter `main.c`.

## Expectations

- Build a safe circuit on a breadboard before connecting USB power. (I do not want more fried ESP32 Boards)
- Use a common ground and keep ESP32 GPIO signals at **3.3 V**.
- Do not drive an LED directly from a GPIO without a current-limiting resistor.
- Do not power a servo from an ESP32 GPIO. Use a suitable 5 V supply rail and connect grounds.
- Please ensure your wiring is correct **5V** and **GND** NEVER connect to eachother directly.
- Keep a wiring diagram and explain your design choices to me verbally when asked.
- Your program should be reliable across repeated demonstrations, not just one successful run.

## Minimum deliverables

- Working hardware prototype
- Source code that builds and has appropriate comments explaining your code and what it does.
- Short README describing the controls, inputs, outputs, and testing.
- A two-minute demonstration in-person during Week 8.

## Capstone grading checklist

- **Functionality:** the core behavior works repeatedly (10 pts)
- **Hardware:** correct polarity, resistors, power, and grounding (5 pts)
- **Software:** clear state/logic, sensible timing, and no busy-waiting where a delay or event would work (10 pts)
- **Explanation & Communication:** clear explanation of project, conncetions, and engineering decisions made. (5 pts)

**Total:** 30% of your grade

## Suggested setup

Create a new ESP-IDF project, then copy the selected project's `firmware/main/main.c` into its `main` folder. Add the `CMakeLists.txt` from the same folder if needed. The pin numbers in the starter files are suggestions; change them if your wiring or board layout requires it.

