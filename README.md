# ThermoGuard: Secure GSM-Based Intelligent Temperature Monitoring System

## Overview

ThermoGuard is an ARM7 LPC2148 based Embedded C project that continuously monitors temperature using an LM35 sensor and sends SMS alerts through a GSM module whenever the temperature exceeds a predefined threshold.

The system supports secure remote configuration using password-protected SMS commands and stores user settings in EEPROM, allowing the configuration to persist after power cycles.

---

## Features

- Real-time temperature monitoring using LM35 sensor
- GSM based SMS alert generation
- Password protected remote control
- Remote temperature setpoint configuration
- Remote mobile number update
- EEPROM storage using I2C
- LCD based status display
- Keypad based local configuration
- UART communication with GSM module
- External interrupt based password access
- Persistent configuration after power reset

---

## Hardware Used

- LPC2148 (ARM7TDMI-S)
- LM35 Temperature Sensor
- GSM Module (SIM800/SIM900)
- 16x2 LCD
- 4x4 Matrix Keypad
- AT24Cxx EEPROM
- Power Supply

---

## Software Used

- Embedded C
- Keil uVision
- Proteus
- Flash Magic

---

## Communication Protocols

- UART
- I2C
- ADC
- External Interrupts

---

## Project Structure

```
Code/
│── main.c
│── gsm.c
│── uart.c
│── adc.c
│── i2c.c
│── lcd.c
│── keypad.c
│── interrupt.c
│── local_settings.c
│── *.h
```

---

## SMS Commands

| Command | Description |
|----------|-------------|
| 1107T250$ | Update temperature setpoint |
| 1107M9876543210$ | Update mobile number |
| 1107I$ | Get current temperature |
| 1107S$ | Get system status |

---

## Working

1. Initializes LCD, UART, ADC, I2C, Keypad and GSM module.
2. Reads stored configuration from EEPROM.
3. Continuously monitors temperature.
4. Sends SMS alert when temperature exceeds the threshold.
5. Receives authenticated SMS commands.
6. Updates EEPROM settings.
7. Supports secure local configuration using keypad password.

---

## Future Improvements

- RTC based logging
- SD Card data storage
- IoT Cloud Integration
- Mobile Application
- CAN Bus Support
- FreeRTOS implementation

---

