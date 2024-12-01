# RFID Gym Member Management System

A robust RFID-based gym member management system built using the STM32F446RE microcontroller. This project utilizes RFID cards to streamline member check-ins and access validation, displaying real-time member data on an LCD screen. It incorporates an MFRC522 RFID module, an SPI-based LCD, an SD card for data storage, and a potentiometer for ADC input.

---

## Features
- **RFID Card Authentication:** Members use RFID cards for seamless check-ins.
- **LCD Data Display:** Real-time display of member details such as name, membership status, and remaining sessions.
- **Membership Validation:** Access control based on membership conditions (e.g., active/inactive status, session count).
- **Data Storage:** Member information is stored and retrieved from an SD card for easy management.
- **ADC Integration:** A potentiometer connected via ADC for additional input functionality or calibration.

---

## Components
### Hardware
- **Microcontroller:** STM32F446RE Nucleo Board
- **RFID Reader:** MFRC522 RFID Module
- **Display:** SPI-based LCD
- **Storage:** SD Card Module (SPI)
- **Input:** Potentiometer for ADC input
- **Shield Board:** mbed-compatible shield for ease of connections

### Software
- **Development Environment:** mbed OS (ARM's IoT Device Platform)
- **Communication Protocols:** SPI for LCD and SD Card, UART/Serial for debugging, I2C for potential expansion.

---

## System Overview

### Workflow
1. **RFID Scanning:**
   - Members scan their RFID cards on the MFRC522 reader.
   - The system reads the card's unique ID.

2. **Data Retrieval:**
   - The ID is matched against records stored on the SD card.
   - Relevant member information (e.g., name, membership status) is fetched.

3. **Validation:**
   - The system checks predefined conditions (e.g., membership validity, remaining sessions).
   - If valid, access is granted, and details are displayed on the LCD.

4. **LCD Output:**
   - Displays member name, status (active/inactive), and remaining sessions.

5. **Potentiometer Input (Optional):**
   - Used for calibration or additional ADC-based functionality.

---

## Pin Configuration
| **Component**      | **STM32F446RE Pin** | **Details**                    |
|---------------------|---------------------|---------------------------------|
| **MFRC522 (RFID)**  | SPI (MOSI, MISO, SCK, NSS) | SPI interface for RFID module |
| **LCD**             | SPI (MOSI, MISO, SCK, NSS) | SPI interface for LCD screen  |
| **SD Card Module**  | SPI (MOSI, MISO, SCK, NSS) | SPI interface for data storage|
| **Potentiometer**   | ADC Pin (e.g., A0)  | Analog input                   |
| **Debugging**       | UART (TX, RX)       | Serial communication           |

---

## Software Features
- **RFID Communication:** Reads unique card IDs via the MFRC522.
- **Data Parsing and Storage:** Reads and writes member data to/from the SD card.
- **LCD Control:** Displays member details using SPI commands.
- **Validation Logic:** Implements conditional checks for membership status and session limits.
- **ADC Integration:** Reads potentiometer values for calibration or dynamic adjustments.

---

## Setup and Usage

### Prerequisites
- STM32CubeIDE or mbed online IDE for programming.
- Required hardware components assembled as per the pin configuration table.

### Steps to Run
1. Clone the repository and open the project in your development environment.
2. Connect the hardware components to the STM32F446RE board as described in the pin configuration.
3. Flash the firmware to the STM32F446RE board.
4. Insert the SD card with pre-loaded member data (CSV format recommended).
5. Power on the system. Scan an RFID card to test functionality.

---

## Member Data Format
The member data is stored on the SD card in a structured format (e.g., CSV):
