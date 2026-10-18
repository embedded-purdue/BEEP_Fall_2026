# Week 5 - Serial Communication Protocols
 
This week will be the final of our alarm system updates, this time focusing on an LCD Display and the method for controlling it: I2C!

---
## 5.1 Content Overview

### 5.1.1 Wired Serial Communication Protocols
Several of the peripherals included in the ESP32 package are dedicated to wired serial communication, including UART, SPI, I2C, and USB (which is vastly more complicated, if you're curious how it works take ECE 337). There are many more (such as CAN or FlexRAY), but these 4 are the most common. These protocols provide methods of sending and receiving data to and from off-chip peripherals such as sensors (lasers, gyroscopes, thermometers, etc.), I/O devices (keyboards, mice, etc.), motor controllers, displays, and more. They are called **Serial** because they send one bit at a time, as opposed to a typical parallel, on-chip bus fabric which may send a full byte at a time on 8 separate wires. Some important features to think about when comparing protocols are 

* Duplex Level: Whether the microcontroller and peripheral device(s) can send and receive data from each other at the same time or must wait their turn
* Speed: How many bits per second can be sent on a typical setup.
* Timing: Does the protocol have a **clock** line to tell each side of the bus when to measure bits.
* Multi-Connect Behavior: How many peripheral and master devices can be connected to the same bus.
* Hardware Driver: Can multiple devices initiate transactions, or must there be only one per line.

When thinking about serial protocols it is important to learn some terminology. Typically each set of wires is referred to as a **bus**, which has one or more **masters** (depending on the protocol and application), and one or more **subordinates** (depending on protocol and application). 
>Note that there are many terms for a bus peripheral including satellite, subordinate, or slave (which is being phased out in recent years, but still seen occasionally). 

Bus masters initiate transactions, whether it be sending to or requesting data from a subordinate. Subordinates respond to these requests, either by accepting the data sent to it or providing data when requested. Some protocols, like UART, are **point-to-point**. This means there are only two devices on the bus, directly connected together. 

![uart-connect](images/1/uart-connect.png)

Other protocols, like SPI, allow for a single master to talk to many subordinates.

![spi-connect](images/1/spi-connect.png)

The protocol we will be using today, I2C, allows many masters and many subordinates.

![i2c-connect](images/1/i2c-connect.png)

UART and SPI are both **push-pull** protocols, where the master has to manually hold the wires at high and low voltage, as opposed to I2C, which is **open-drain**. This means that the pull-up resistors you see in the image make both wires default to high voltage, and a bus master just pulls the line low. This is what allows I2C to have multiple bus masters, if two masters tried to drive a line in push-pull they could short between each other's power and ground, which is obviously an issue. Since I2C masters only ever pull low, it isn't an issue.

The other important functional difference is **duplex** behavior. UART and SPI are **full duplex**, meaning you can send and receive data at the same time. UART achieves this by having separate wires for TX and RX, and SPI through MOSI and MISO. I2C is **half duplex**, since there is only one data line (**SDA**), you can only do one of send or receive data at once. 

Besides how the protocols function, one also needs to think about the speed of each protocol. Push-pull protocols are generally faster (pull-up resistors decrease slew rate), with open-drain protocols like I2C being slower. Some typical speeds:

* I2C: 100KHz -> 1MHz (clocked, so this is a true frequency)
* SPI: 2MHz -> 50MHz (clocked, so this is a true frequency)
* UART: ~115Kbps (UART has no clock line, so this is a **baud rate** - bits per second - rather than a clock frequency)

### 5.1.2 I2C Details
For our application, all I2C transactions are initiated by the ESP32, with the display responding. A typical transaction looks like this:

![i2c-bits](images/1/i2c-bits.png)

The bus master pulls **SDA** (the data line) low while **SCL** (the clock line) is high, which is the standard **start bit**. This tells the other devices on the bus that a transaction is coming. The next 7 bits are the **device address**, which is usually unique to each of the satellite devices on the bus. The display we are using today's ID is 0x27, all devices on the bus listen to these first 7 bits to determine whether the transaction is meant for them. The next bit is the Read/Write bit, which determines what type of transaction this is. The next bit is the satellite device **ACK** (acknowledge), which it sends if it matches the ID and correctly received the first byte.  

The rest of the transaction is processed byte by byte, with acks between each. The master can provide however many bits it wants in a write and receive however many it wants in a read, only stopping when the master allows the bus to go high while SCL is high, the **stop** bit. Commonly I2C devices will have a **register map** in their datasheet which tells us where to look for specific data, and transactions are structured like: *Start*, *I2C_ADDR*, *R/W*, *Register Address*, *Data*..., *Stop*. Or a write and read back to back, where you write the register address and then read on the next transaction.

## 5.2 Coding Activity

### 5.2.1 Circuit Setup

![Board Setup](images/2/schematic.png)

>Note: Connect the VCC pin on the LCD to the 5V pin on your ESP32, but do NOT connect anything else to the 5V pin. Also, if you have issues with the I2C lines try connecting a 2K Ohm pull-up resistor from SCL and SDA to the 3V power rail

>**Rewiring alert:** In Week 4, `CODE0` and `CODE1` were on GPIO 21 and 22. Those pins are now needed for I2C (`SDA`/`SCL`), so this week `CODE0` moves to **GPIO 5** and `CODE1` moves to **GPIO 17**. Double check the schematic above and move those two jumper wires before you start - this is the single most common wiring mistake this week.

### 5.2.2 Environment Setup

Before you begin, please remember to create a new project:
1. Press ```ctrl+shift+p``` to open up the command panel
2. Look for ```ESP-IDF: Create New Empty Project```  
<img src="images/2/new-project.png">

3. Enter a folder name in the popup window
<img src="images/2/popup.png">

4. Select a location for the new folder (organize however you like!)

5. Replace the ```main``` folder of your new project with the version provided in this week's github folder.  
<img src="images/2/main-folder.png">

6. Press `ctrl + shift + p` to open the VSCode command panel again, and run **Add VS Code Configuration Folder**.
<img src="images/2/add-config.png">

### 5.2.3 Software

#### Headers

```C
#include "driver/i2c_master.h"
```
Only one new header this week, this one provides the necessary data structures and functions to interact with the i2c peripheral.


#### Globals and Macros

```C
#define I2C_SDA_PIN 21
#define I2C_SCL_PIN 22
```
Pin macros for the clock and data lines of the i2c peripheral.

```C
#define LCD_ADDR             0x27
#define FLAG_BACKLIGHT_ON    0x08
#define FLAG_ENABLE          0x04
#define FLAG_RS_DATA         0x01
#define FLAG_RS_COMMAND      0x00
```  
LCD_ADDR is the default I2C address for our LCD displays, and the rest are binary patterns for certain bit flags on the 1602 LCD, determined by the designers.

```C
volatile bool first_code_press = false;
volatile bool second_code_press = false;
volatile bool code_button_pressed = false;
```
Splitting the code setting button flag into two, for more freedom when displaying on the screen in the main loop. We also add a combined flag for the code buttons so we can update the screen live on every button press.

```C
i2c_master_bus_handle_t bus_handle;
i2c_master_dev_handle_t lcd_handle;
``` 
Handles for the FreeRTOS I2C Bus Master configuration, used to send data over the selected I2C port; as well as the device that is connected to said port, which in our case is the LCD Display.

#### Functions and To Do

In `lcd_send_cmd` and `lcd_send_char` you need to fill in the buffer, which is just an array of 6 bytes, and send the command or character over the I2C bus. The array is 6 bytes long because of the interaction between the I2C chip on the back of the LCD and the LCD itself. The chip only uses 4 of the 8 D0-D7 bits for data (The other 4 are the flags you see macros defined for in helpers.h), which means we have to divide our writes into "nibbles" of 4 bits each. Both of these nibbles require a 3-step transmission process, where you send the data to let it settle, send it again with the enable flag toggled on, and then one final send. This pulses the enable flag and shifts the data you give it onto the display (This display is very odd, similar ones used in ECE 362 do not share this behavior, nor do many others you may come across).

In `lcd_print` you will use `lcd_send_char` to write a string to the display in the same way as printf("Hello World!") does to your terminal. The method we have chosen is a full line rewrite on every call, which means we need to clear whatever was there before. This function requires two while loops: the first to print as many characters as are present in the `str` argument, and the second to fill the rest of the line with whitespace, if any.  
String variables like `str` are functionally just arrays of characters, the same way the buffer in `lcd_send_cmd` is an array of integers, which means we can index specific items like `str[0]`, `str[1]`, etc. Also like other arrays, the variable name itself (be it `buffer` or `str`) stores the address of the first element. So dereferencing the pointer like `*str` is equivalent to `str[0]`. We can then increment the pointer itself like `str = str + 1` and then dereference it again to get `*str = str[1]`. You can keep doing this for the entire length of the string, stepping through one character at a time until you reach the last element, which for C strings declared like `"Hello World!"` is always a null terminator `\0`.  
This leads to an easy method for printing a string: print one character at a time with this pointer incrementing behavior until you detect that the character held at the pointers current location is the null terminator.

In `setup_lcd` you will need to fill in the bus and device config structs, and then call their respective initialization functions. The bus config and subsequent function call is a standard pattern across most embedded systems, but the device config is a software construct of the ESP-IDF and FreeRTOS. Both set internal registers on the ESP32 itself, with the device config changing the speed and i2c address for any transactions using the device's associated handle to abstract away those details from write or read calls. The rest of this function is the device specific initialization sequence for the LCD screen, which was found in an open-source github repo [here](https://github.com/DavidAntliff/esp32-i2c-lcd1602/blob/master/i2c-lcd1602.c).


## 5.3 Helpful Links

#### Documentation
* [ESP32 WROOM 32E Pinout](https://docs.sunfounder.com/projects/umsk/en/latest/07_appendix/esp32_wroom_32e.html)
* [ESP32 Technical Reference Manual](https://documentation.espressif.com/esp32_technical_reference_manual_en.pdf#iomuxgpio)
* [ESP-IDF Docs](https://docs.espressif.com/projects/esp-idf/en/stable/esp32/index.html)

#### Environment Setup
* [IDF Frontend (if you're curious)](https://docs.espressif.com/projects/esp-idf/en/stable/esp32/api-guides/tools/idf-py.html)
* [Dev Container Setup](https://docs.espressif.com/projects/vscode-esp-idf-extension/en/latest/additionalfeatures/docker-container.html)
* [WSL](https://learn.microsoft.com/en-us/windows/wsl/basic-commands)
* [USBIPD](https://github.com/dorssel/usbipd-win)