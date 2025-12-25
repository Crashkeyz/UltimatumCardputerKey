# Documentation

This directory contains documentation and setup instructions for the UltimatumCardputerKey firmware.

## Flashing to M5Stack Cardputer

### Hardware Requirements
- M5Stack Cardputer (ESP32-S3 based device)
- USB-C cable

### Flashing Steps

1. **Install PlatformIO** (if not already installed):
   ```bash
   pip install platformio
   ```

2. **Connect the Cardputer** via USB-C cable to your computer

3. **Flash the firmware**:
   ```bash
   cd /path/to/UltimatumCardputerKey
   pio run --target upload
   ```

4. **Monitor serial output** (optional):
   ```bash
   pio device monitor
   ```

### Troubleshooting

- **Device not detected**: 
  - Make sure USB drivers are installed
  - Try a different USB cable (some cables are power-only)
  - Press the reset button on the Cardputer
  
- **Upload failed**: 
  - Hold the BOOT button while connecting USB
  - Check that no other program is using the serial port
  
- **No output on serial monitor**:
  - Check that monitor_speed matches (115200 baud)
  - Verify USB CDC is enabled in platformio.ini

### OTA Upload (Optional)

For over-the-air updates (requires initial flash via USB):
```bash
pio run --target upload --upload-port Cardputer-OTA.local
```