"""
environmental_chamber_logger.py

Reads temp / NTC raw ADC / CO2 lines from the environmental chamber Arduino
over serial and appends each one, timestamped, to a CSV log.

Expects one CSV line per loop from environmental_chamber_controller.ino, as:
    temp,ntcRaw,co2
"""

import csv
from datetime import datetime

import serial

SERIAL_PORT = 'COM3'   # Windows-style port. On Mac/Linux this looks more like
                        # '/dev/tty.usbmodemXXXX' or '/dev/ttyACM0' -- check
                        # Arduino IDE's Tools > Port menu and update this.
BAUD_RATE = 115200      # must match Serial.begin() in the .ino
OUTPUT_CSV = 'KowshikLogs.csv'


def main():
    ser = serial.Serial(SERIAL_PORT, BAUD_RATE)

    with open(OUTPUT_CSV, 'a', newline='') as f:
        writer = csv.writer(f)
        if f.tell() == 0:
            writer.writerow(['timestamp', 'temp_c', 'ntc_raw', 'co2_ppm'])

        while True:
            raw_line = ser.readline().decode('utf-8', errors='replace').strip()
            if not raw_line:
                continue

            print(raw_line)

            fields = raw_line.split(',')
            if len(fields) != 3:
                continue  # partial/garbled line -- skip rather than crash

            try:
                temp = float(fields[0])
                ntc_raw = int(fields[1])
                co2 = float(fields[2])
            except ValueError:
                continue  # didn't parse cleanly -- skip this line

            timestamp = datetime.now().isoformat(timespec='seconds')
            writer.writerow([timestamp, temp, ntc_raw, co2])
            f.flush()


if __name__ == '__main__':
    main()
