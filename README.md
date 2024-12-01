# RFID Gym Member Management System

A robust RFID-based gym member management system built using the STM32F446RE microcontroller and the STM32 HAL API. This project enables gym members to check in using RFID cards, with real-time member data displayed on an LCD screen. It incorporates an MFRC522 RFID module, an SPI-based LCD, an SD card for data storage, a potentiometer for ADC input, and a buzzer for card acceptance/rejection feedback using DAC mode.

---

## Features
- **RFID Card Authentication:** Members use RFID cards for seamless check-ins.
- **LCD Data Display:** Real-time display of member details such as name, membership status, and remaining sessions.
- **Membership Validation:** Access control based on membership conditions (e.g., active/inactive status, session count).
- **Data Storage:** Member information is stored and retrieved from an SD card for easy management.
- **Buzzer Feedback:** A buzzer provides auditory feedback (via DAC mode) to indicate card acceptance or rejection.
- **ADC Integration:** A potentiometer connected via ADC for additional input functionality or calibration.
- **Interrupt-driven Design:** Minimized use of polling by utilizing interrupts for efficient operation.

---

## Components
### Hardware
- **Microcontroller:** STM32F446RE Nucleo Board
- **RFID Reader:** MFRC522 RFID Module
- **Display:** SPI-based LCD
- **Storage:** SD Card Module (SPI)
- **Input:** Potentiometer for ADC input
- **Feedback Device:** Buzzer (operated in DAC mode)
- **Shield Board:** Compatible board for easy hardware connections

### Software
- **Development Environment:** STM32CubeIDE
- **Firmware API:** STM32 HAL (Hardware Abstraction Layer)
- **Communication Protocols:** SPI for LCD and SD Card, UART/Serial for debugging, and interrupt-based workflows for optimized performance.

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
   - **Buzzer Feedback:** The buzzer emits a distinct tone for acceptance and rejection.

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
| **Buzzer**          | DAC Pin (e.g., PA4) | DAC mode for sound generation  |
| **Debugging**       | UART (TX, RX)       | Serial communication           |

---

## Development Steps

1. **Hardware Setup:**
   - Connect the STM32F446RE to the MFRC522 RFID module, SPI-based LCD, SD card module, potentiometer, and buzzer as per the pin configuration.

2. **Software Setup:**
   - Use STM32CubeMX to generate the HAL-based initialization code.
   - Configure peripherals: SPI for the LCD, RFID, and SD card; ADC for the potentiometer; DAC for the buzzer; and UART for debugging.

3. **Interrupt Configuration:**
   - Enable interrupts for RFID scanning and other critical tasks to minimize polling.

4. **RFID Module Integration:**
   - Implement SPI-based communication to read card IDs using the MFRC522.

5. **SD Card Data Handling:**
   - Use SPI to read/write member data stored in CSV format.

6. **LCD Integration:**
   - Use SPI to display member details and validation messages.

7. **Buzzer Feedback:**
   - Configure the DAC to produce distinct tones for card acceptance and rejection.

8. **Validation Logic:**
   - Implement membership conditions to determine access.

9. **Testing and Calibration:**
   - Test the entire system, including hardware and software, and calibrate the potentiometer if required.

---

## Member Data Format
The member data is stored on the SD card in a structured format (e.g., CSV):
