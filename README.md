# Environmental Chamber Controller

Arduino + Python system for CO2 and temperature control of a live-imaging spheroid chamber. Keeps chamber CO2 under a set threshold and temperature near 37°C during time-lapse microscopy of hydrogel-embedded spheroids, and logs both to CSV for later analysis.

## Hardware

| Component | Role |
|---|---|
| Arduino Uno R3 | control hub |
| K30 CO2 sensor (0–10% range) | CO2 monitoring, serial (UART) |
| NTC 10k thermistor + fixed resistor | chamber temperature, analog voltage divider |
| 2x dual H-bridge motor driver (L298N-style) | PWM enable control for the valve and heaters |
| NC solenoid valve | CO2 flush |
| 3x heating foil | chamber heating (all 3 switch together) |

## Pinout

| Arduino pin | Wire | Connects to |
|---|---|---|
| D13 | yellow | K30 Rx |
| D12 | orange | K30 Tx |
| D11 | teal 1 | shared heat-enable bus → motor ctrl 1 ENB, motor ctrl 2 ENA, motor ctrl 2 ENB |
| D9 | brown | motor ctrl 1 ENA → CO2 valve |
| D8 | teal 2 | NTC divider supply rail |
| A0 | blue | NTC divider read |
| 5V | red | common logic reference (both controllers) |
| GND | black | common ground (both controllers + 12V supply return) |

D7 (alarm) and D10 (originally wired for a DHT11) exist in the code/board but are not currently connected to anything.

## How it works

- **CO2 control** — the valve (motor ctrl 1, OUT1) opens for a 2-second pulse whenever the K30 reading crosses the threshold set in `environmental_chamber_controller.ino`, and stays closed otherwise.
- **Temperature control** — all 3 heaters are wired to the same PWM line (D11) and switch on/off together based on the NTC reading vs. 37°C.
- **Logging** — the Arduino streams one `temp,ntcRaw,co2` line per second over serial; `environmental_chamber_logger.py` timestamps and appends each line to a CSV.

## Repo contents

- `environmental_chamber_controller.ino` — Arduino sketch (sensor reads + valve/heater control)
- `environmental_chamber_logger.py` — serial logger (Arduino → timestamped CSV)
- `README.md` — this file

## Setup

### Arduino

1. Install the `kSeries` library (for the K30 sensor) via Library Manager or from its source repo.
2. In the sketch, fill in `R_FIXED`, `NTC_R25`, and `NTC_BETA` with your actual resistor and thermistor values — these currently ship as placeholders, and temperature readings aren't trustworthy until they're real numbers.
3. Upload, then open Serial Monitor at 115200 baud and confirm `temp,ntcRaw,co2` lines are coming through before connecting the logger.

### Python

```bash
pip install pyserial
```

Edit `SERIAL_PORT` in `environmental_chamber_logger.py` to match your system — Windows looks like `COM3`; Mac/Linux looks like `/dev/tty.usbmodemXXXX` or `/dev/ttyACM0` (check the Arduino IDE's **Tools → Port** menu for the exact name).

```bash
python environmental_chamber_logger.py
```

Appends timestamped rows (`timestamp, temp_c, ntc_raw, co2_ppm`) to `KowshikLogs.csv` until stopped with Ctrl+C. Safe to stop and restart — it appends rather than overwriting, and only writes the header once.

For analysis afterward, the CSV loads directly with `pandas.read_csv('KowshikLogs.csv')`.

## Known open items

- NTC calibration constants (`R_FIXED`, `NTC_R25`, `NTC_BETA`) are placeholders — see Setup above.
- D8 is held `HIGH` permanently as the divider's supply rather than pulsed only before each reading — worth confirming that's the intended behavior.
- The CO2 check applies a ×10 multiplier to the raw K30 ppm reading before comparing to the 5000 threshold — double check that the intended trigger point is 5000 ppm and not 500.
- Alarm (D7) and D10 are present in the code but unused in the current build.
