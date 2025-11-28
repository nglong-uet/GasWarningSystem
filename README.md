# GasWarningSystem – STM32-Based Gas Detection & Safety Control  
A real-time gas detection and automatic protection system using STM32F4.  
The system measures gas levels, triggers multi-level alerts, and automatically closes a valve through a servo motor in hazardous situations.

---

## 🎥 Demo (Optional)
## 📽️ Demo Video
[![Demo Video]](https://drive.google.com/file/d/1E5k13etV-Ui94mv5GgUzZK4RsarQ4P6L/view?usp=sharing)


##  Features Overview
### 1. Gas Measurement Using MQ2
- Reads analog signal from **MQ2 gas sensor** using ADC.
- Noise filtering using simple moving average.
- Converts ADC values into gas concentration index.
- Detects sensor disconnection/abnormal values.

### 2. Multi-level Warning System
Defines 3 safety levels:
- **Safe** – Normal LED Indicator  
- **Warning** – LED + UART alert  
- **Hazard** – Alarm + Valve close  
Levels determined by preset gas thresholds.

### 3. Automatic Valve Closure
- Uses **servo motor** to mechanically close the gas valve.
- Triggered immediately when entering Hazard state.
- Servo controlled via PWM output.

### 4. Alarm System (Relay Output)
Relay activates:
- Buzzer  
- Alarm siren  
- Emergency light  
Relay remains ON until gas returns to safe levels.

### 5. LCD I2C Display
Shows:
- Gas level  
- Current warning level  
- Servo position  
- Relay state  
Updates continuously without screen flicker.

### 6. Threshold Configuration Mode
- User can cycle through multiple threshold presets using button.
- Useful for kitchen/home/workshop environments.

### 7. UART Logging for Monitoring
Real-time logs:
- Gas PPM
- Warning level
- Valve action
- Relay status
- ADC state
- Error codes
Allows external monitoring using PuTTY.

### 8. Safety Handling
- Sensor error → enter Safe Mode + error message on LCD.
- Servo lock detection (timeout).
- Software reset on major failure.


## 🛠 Hardware Components
| Component | Purpose |
|----------|---------|
| STM32F4 | Main MCU |
| MQ2 sensor | Gas detection |
| Servo motor | Valve control |
| Relay module | Alarm activation |
| LCD 16x2 I2C | Visual interface |
| Button | Threshold selection |
| UART | Debug output |

## 📡 System Architecture
<img width="611" height="348" alt="image" src="https://github.com/user-attachments/assets/31422aa9-f108-40d7-87c5-ad7cc17fc5c2" />
