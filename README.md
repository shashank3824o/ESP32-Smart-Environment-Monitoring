# ESP32-Smart-Environment-Monitoring
IoT-based environment monitoring using ESP32, sensors, Python and Blynk
Tip: In Word, select from the first line beginning with # ESP32... to the final line. Copy it, then paste into the README editor on GitHub. Do not copy this instruction line.

# ESP32 Smart Environmental Monitoring System

 

A completed IoT-based environmental monitoring project built with an ESP32. The system reads temperature and relative humidity from a DHT11 sensor, displays the readings on an OLED screen, and sends monitoring data over Wi-Fi to a Blynk dashboard.

 

## Project Status

 

**Working prototype completed:** ESP32, DHT11 readings, OLED display, Wi-Fi connectivity, and Blynk Console integration are working.

 

## Features

 

- Measures temperature using a DHT11 sensor.

- Measures relative humidity using a DHT11 sensor.

- Displays live readings on an OLED screen.

- Connects the ESP32 to Wi-Fi.

- Sends sensor data to Blynk for remote monitoring.

- Includes LED blinking and PWM brightness-control experiments.

 

## Hardware Components

 

| Component | Purpose |

|---|---|

| ESP32 development board | Main controller and Wi-Fi connectivity |

| DHT11 sensor | Temperature and humidity measurements |

| I2C OLED display | Displays readings locally |

| LED | GPIO output and PWM experiments |

| Current-limiting resistor | Protects the LED |

| Breadboard | Circuit prototyping |

| Jumper wires | Component connections |

| USB cable | Programming and power |

 

## Software and Tools

 

- Arduino IDE

- ESP32 Arduino board package

- Adafruit DHT Sensor Library

- Adafruit Unified Sensor Library

- Adafruit SSD1306 and Adafruit GFX libraries (if using an SSD1306 OLED)

- Blynk IoT Console

 

## How It Works

 

1. The ESP32 starts and connects to the configured Wi-Fi network.

2. The DHT11 measures temperature and relative humidity.

3. The ESP32 shows the readings on the OLED display.

4. The ESP32 sends configured sensor values to Blynk over Wi-Fi.

5. The user monitors the data from the Blynk Console/dashboard.

 

## Wiring Reference

 

**Always verify the labels on your exact modules before wiring.** Pin layouts can vary between boards and sensor modules.

 

### DHT11 Sensor

 

| DHT11 pin | ESP32 connection |

|---|---|

| VCC / `+` | 3V3, if supported by the module |

| GND / `-` | GND |

| DATA / OUT / `S` | GPIO 4 (if this matches your sketch) |

 

### I2C OLED Display

 

For a compatible I2C OLED using the common ESP32 I2C pins:

 

| OLED pin | ESP32 connection |

|---|---|

| GND | GND |

| VDD / VCC | 3V3, if required by the module |

| SCK / SCL | GPIO 22 |

| SDA | GPIO 21 |

 

Confirm that your display is I2C-compatible and check its voltage requirements. Some OLED modules use SPI or a different controller/interface.

 

### LED Test Circuit

 

- LED anode (long leg) connects to GPIO 18 through a suitable current-limiting resistor.

- LED cathode (short leg) connects to GND.

 

## Setup and Run

 

1. Install the Arduino IDE.

2. Install ESP32 board support and select the correct board and serial port.

3. Install the libraries required by the sketch using Library Manager.

4. Open the `.ino` project file in Arduino IDE.

5. Confirm the pin definitions match the actual wiring.

6. Configure Wi-Fi and Blynk settings without exposing secrets in the source code.

7. Connect the ESP32 over USB and upload the sketch.

8. Open Serial Monitor at the baud rate configured in the sketch (the project setup used `115200`).

9. Check that the OLED readings update and that the Blynk dashboard receives data.

 

## Example Serial Output

 

The following values are illustrative; real readings depend on the environment.

 

```text

Temperature: 28.0 C

Humidity: 65.0 %

```

 

## Secure Configuration

 

**Do not upload real Wi-Fi passwords, Blynk Auth Tokens, API keys, or other secrets to a public GitHub repository.** Use placeholders or keep secrets in a local configuration file excluded from Git.

 

Example placeholders only:

 

```cpp

const char* ssid = "YOUR_WIFI_NAME";

const char* password = "YOUR_WIFI_PASSWORD";

#define BLYNK_AUTH_TOKEN "YOUR_BLYNK_AUTH_TOKEN"

```

 

If real credentials were already published, change the exposed password or revoke/regenerate the exposed token.

 

## Repository Structure

 

```text

ESP32-Smart-Environmental-Monitoring/

├── ESP32_Smart_Environmental_Monitoring.ino

├── README.md

├── circuit_diagram.png

└── images/

    ├── hardware_setup.jpg

    ├── oled_output.jpg

    └── blynk_dashboard.jpg

```

 

Replace the example sketch filename and add image files only if they exist in your repository.

 

## Future Improvements

 

- Add an air-quality sensor such as MQ-135 and validate/calibrate its readings.

- Add a light-intensity sensor.

- Improve error handling and Wi-Fi reconnection behavior.

- Add historical charts, alerts, and additional Blynk dashboard widgets.

- Document the final circuit and measured results.

 

## Safety Notes

 

- Disconnect USB power before changing wiring.

- Use a current-limiting resistor with a bare LED.

- Confirm voltage requirements and pinouts before powering modules.

- These readings are for learning and prototyping, not a substitute for calibrated safety or industrial instruments.

- Keep Wi-Fi credentials and Blynk tokens private.

 

## Author

 

**Shashank Singh**  

B.Tech - Electronics and Communication Engineering

 

---

 

Built as a hands-on learning project in embedded systems, sensor interfacing, and IoT monitoring.
