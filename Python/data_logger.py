import requests
import csv
import time
from datetime import datetime
from pathlib import Path

# Replace this with your ESP32's actual IP address
ESP32_URL = "http://172.17.250.36/data"

# CSV file will be created in the same folder as this script
CSV_FILE = Path(__file__).with_name("sensor_data1.csv")

def log_sensor_data():
    file_exists = CSV_FILE.exists()

    with open(CSV_FILE, "a", newline="", encoding="utf-8") as file:
        writer = csv.writer(file)

        if not file_exists or CSV_FILE.stat().st_size == 0:
            writer.writerow([
                "Timestamp",
                "Temperature_C",
                "Humidity_Percent",
                "MQ135_Raw"
            ])

        print("ESP32 data logging started. Press Ctrl+C to stop.")

        while True:
            try:
                response = requests.get(ESP32_URL, timeout=5)
                response.raise_for_status()
                data = response.json()

                timestamp = datetime.now().strftime("%Y-%m-%d %H:%M:%S")

                temperature = data.get("temperature", "")
                humidity = data.get("humidity", "")
                mq135 = data.get("mq135", "")

                writer.writerow([
                    timestamp,
                    temperature,
                    humidity,
                    mq135
                ])

                file.flush()

                print(
                    f"{timestamp} | "
                    f"Temperature: {temperature} °C | "
                    f"Humidity: {humidity}% | "
                    f"MQ135 raw: {mq135}"
                )

            except (requests.RequestException, ValueError) as error:
                print("Could not read ESP32 data:", error)

            time.sleep(5)


if __name__ == "__main__":
    try:
        log_sensor_data()
    except KeyboardInterrupt:
        print("\nLogging stopped. Data saved to:", CSV_FILE)

