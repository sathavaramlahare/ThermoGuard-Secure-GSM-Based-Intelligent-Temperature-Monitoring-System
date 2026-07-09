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

- LPC2148
- LM35 Temperature Sensor
- GSM Module (M660A) 
- 16x2 LCD
- 4x4 Matrix Keypad
- AT24LC256 EEPROM
- Power Supply

---

## Software Used

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

## Flow Chart
┌──────────────────────────────────────────────────────────────┐
│                          START                               │
└─────────────────────────────┬────────────────────────────────┘
                              │
                              ▼
┌──────────────────────────────────────────────────────────────┐
│                 Power ON / System Reset                      │
└─────────────────────────────┬────────────────────────────────┘
                              │
                              ▼
┌──────────────────────────────────────────────────────────────┐
│ Initialize Hardware Peripherals                             │
│-------------------------------------------------------------│
│ LCD │ UART │ ADC │ I2C │ GSM │ Keypad │ External Interrupt  │
└─────────────────────────────┬────────────────────────────────┘
                              │
                              ▼
┌──────────────────────────────────────────────────────────────┐
│ Read EEPROM Initialization Flag (Address 0x40)              │
└─────────────────────────────┬────────────────────────────────┘
                              │
                              ▼
                 ┌────────────────────────────┐
                 │ Is Flag = "1107@s" ?       │
                 └─────────────┬──────────────┘
                               │
               ┌───────────────┴───────────────┐
               │                               │
              NO                              YES
               │                               │
               ▼                               ▼
┌──────────────────────────────┐   ┌──────────────────────────────┐
│ Store Default Configuration  │   │ Load Saved Configuration     │
│------------------------------│   │------------------------------│
│ • Temperature Setpoint       │   │ • Temperature Setpoint       │
│ • Mobile Number              │   │ • Mobile Number              │
│ • Password                   │   │ • Password                   │
│ • Store Initialization Flag  │   └──────────────────────────────┘
└───────────────┬──────────────┘
                │
                └───────────────┬───────────────────────────────┐
                                ▼
┌──────────────────────────────────────────────────────────────┐
│ Initialize GSM Module                                       │
│ AT → ATE0 → CMGF → CMGD → CNMI                              │
└─────────────────────────────┬────────────────────────────────┘
                              │
                              ▼
┌──────────────────────────────────────────────────────────────┐
│                 Enter Infinite Monitoring Loop               │
└─────────────────────────────┬────────────────────────────────┘
                              │
                              ▼

═══════════════════════════════════════════════════════════════════════════════
                     MAIN LOOP (Runs Continuously)
═══════════════════════════════════════════════════════════════════════════════

┌──────────────────────────────┐
│ Temperature Monitoring       │
└──────────────┬───────────────┘
               │
               ▼
┌──────────────────────────────┐
│ Read ADC Temperature         │
└──────────────┬───────────────┘
               │
               ▼
┌──────────────────────────────┐
│ Convert ADC to Temperature   │
└──────────────┬───────────────┘
               │
               ▼
┌──────────────────────────────┐
│ Compare with Setpoint        │
└──────────────┬───────────────┘
               │
        ┌──────┴───────┐
        │              │
      YES             NO
        │              │
        ▼              │
┌──────────────────────────────┐
│ Send Temperature Alert SMS   │
└──────────────────────────────┘



┌──────────────────────────────┐
│ External Interrupt (EINT0)   │
└──────────────┬───────────────┘
               │
               ▼
┌──────────────────────────────┐
│ Enter Password               │
└──────────────┬───────────────┘
               │
        ┌──────┴───────┐
        │              │
    Correct          Wrong
        │              │
        ▼              ▼
┌──────────────────┐   ┌────────────────────────────┐
│ Open Local Menu  │   │ Send Unauthorized SMS      │
└────────┬─────────┘   └────────────────────────────┘
         │
         ▼
┌──────────────────────────────┐
│ Enter New Temperature        │
│ Setpoint                     │
└──────────────┬───────────────┘
               │
               ▼
┌──────────────────────────────┐
│ Update EEPROM                │
└──────────────┬───────────────┘
               │
               ▼
┌──────────────────────────────┐
│ Send Confirmation SMS        │
└──────────────────────────────┘



┌──────────────────────────────┐
│ SMS Notification             │
│ (UART Interrupt)             │
└──────────────┬───────────────┘
               │
               ▼
┌──────────────────────────────┐
│ New SMS Received             │
└──────────────┬───────────────┘
               │
               ▼
┌──────────────────────────────┐
│ Read SMS                     │
└──────────────┬───────────────┘
               │
               ▼
┌──────────────────────────────┐
│ Verify Mobile Number         │
└──────────────┬───────────────┘
               │
        ┌──────┴───────┐
        │              │
      Valid         Invalid
        │              │
        ▼              ▼
┌──────────────────┐   ┌───────────────────────────┐
│ Check Syntax     │   │ Send Unauthorized Alert   │
└────────┬─────────┘   └───────────────────────────┘
         │
    ┌────┴─────┐
    │          │
 Invalid     Valid
    │          │
    ▼          ▼
┌────────────┐  ┌────────────────────────────────────────────┐
│ Error LCD  │  │ Execute SMS Command                        │
└────────────┘  │--------------------------------------------│
                │ T → Update Temperature Setpoint            │
                │ M → Update Authorized Mobile Number        │
                │ I → Send Current Temperature               │
                │ S → Send Current System Status             │
                └──────────────────────┬─────────────────────┘
                                       │
                                       ▼
                        ┌──────────────────────────────┐
                        │ Update EEPROM (If Required)  │
                        └──────────────┬───────────────┘
                                       │
                                       ▼
                        ┌──────────────────────────────┐
                        │ Send Response SMS            │
                        └──────────────────────────────┘


                              │
                              ▼
┌──────────────────────────────────────────────────────────────┐
│              Continue Monitoring (Repeat Forever)            │
└──────────────────────────────────────────────────────────────┘
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

1. On power-up, the LPC2148 initializes the LCD, UART, ADC, I2C, Keypad, External Interrupt, and GSM module using AT commands.

2. The system reads the stored temperature setpoint, authorized mobile number, and password from EEPROM. During the first boot, default values are written into EEPROM.

3. The LM35 temperature sensor is continuously monitored using the ADC, and the measured temperature is compared with the stored setpoint.

4. If the measured temperature exceeds the setpoint, the system automatically sends an SMS alert to the authorized mobile number via the GSM module.

5. Incoming SMS notifications are detected through UART interrupts. The controller reads the received SMS, extracts the message, and deletes it from GSM memory after processing.

6. Before executing any command, the sender's mobile number and SMS syntax are verified to ensure that only authorized users can access the system.

7. The system supports secure SMS commands to update the temperature setpoint, change the authorized mobile number, request the current temperature, and retrieve the system status. All configuration changes are stored permanently in EEPROM.

8. Local configuration is protected using a keypad-based password. After successful authentication, users can update the temperature setpoint locally, and the system sends SMS notifications for configuration updates and unauthorized access attempts.

---
**Applications**
1. Industrial Temperature Monitoring
2. Smart Home Automation
3. Laboratories
4. Hospitals & Medical Storage
5. Agriculture & Greenhouses
6. Electrical Panels & Server Rooms
7. Remote Industrial Automation

