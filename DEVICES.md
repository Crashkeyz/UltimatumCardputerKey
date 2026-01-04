# Supported Devices

## Thunder Castle Firmware - Multi-Device Support

This firmware now supports multiple ESP32-based devices with automatic hardware detection.

---

## ✅ Currently Supported Devices

### M5Stack Family

#### 1. M5Stack Cardputer (Primary) 🎯

- **MCU**: ESP32-S3 QFN56
- **Display**: 240x135 ST7789
- **Input**: Full QWERTY keyboard
- **Features**: Microphone, SD card, battery
- **Status**: ✅ Fully Supported

#### 2. M5Stack Core2

- **MCU**: ESP32 WROVER-B
- **Display**: 320x240 ILI9342C
- **Input**: Touch screen
- **Features**: Microphone, SD card, vibration motor
- **Status**: ✅ Supported (touch UI adapted)

#### 3. M5Stack StickC Plus

- **MCU**: ESP32-PICO-D4
- **Display**: 135x240 ST7735
- **Input**: 2 buttons
- **Features**: IMU, IR transmitter, battery
- **Status**: ✅ Supported (button-only navigation)

#### 4. M5Stack Atom Matrix

- **MCU**: ESP32-PICO-D4
- **Display**: 5x5 RGB LED matrix
- **Input**: 1 button
- **Features**: IMU, minimal form factor
- **Status**: ⚠️ Beta (LED matrix UI)

---

### LilyGO Family

#### 5. LilyGO T-Embed (Origin Device) 🌟

- **MCU**: ESP32-S3
- **Display**: 170x320 ST7789
- **Input**: Rotary encoder + button
- **Features**: SD card, battery, encoder
- **Status**: ✅ Fully Supported (original Skeletor code)

#### 6. LilyGO T-Display S3

- **MCU**: ESP32-S3
- **Display**: 170x320 ST7789
- **Input**: 2 buttons
- **Features**: USB-C, battery connector
- **Status**: ✅ Supported

#### 7. LilyGO T-QT Pro

- **MCU**: ESP32-S3
- **Display**: 128x128 GC9107
- **Input**: 1 button
- **Features**: Ultra-compact, USB-C
- **Status**: ✅ Supported

#### 8. LilyGO T-Dongle S3

- **MCU**: ESP32-S3
- **Display**: 80x160 ST7735
- **Input**: 1 button
- **Features**: USB-A form factor
- **Status**: ✅ Supported

#### 9. LilyGO T-Watch S3

- **MCU**: ESP32-S3
- **Display**: 240x240 round AMOLED
- **Input**: Touch + buttons
- **Features**: Watch form, sensors, battery
- **Status**: ✅ Supported (watch UI)

---

## 🎨 Device-Specific Adaptations

### Navigation Schemes

| Device | Up | Down | Left | Right | Select | Back |
|--------|-----|------|------|-------|--------|------|
| Cardputer | ; | . | , | / | Enter | ESC |
| T-Embed | Encoder↑ | Encoder↓ | Encoder← | Encoder→ | Click | Long Press |
| Core2 | Touch↑ | Touch↓ | Touch← | Touch→ | Tap | Swipe Down |
| StickC+ | Btn A | Btn A | Btn B | Btn B | Btn A Long | Btn B Long |
| T-Display S3 | Btn1 | Btn1 | Btn2 | Btn2 | Btn1 Long | Btn2 Long |

### Display Layouts

- **Large screens** (320x240+): Full menus with 5+ items visible
- **Medium screens** (240x135): Optimized 3-item display
- **Small screens** (170x135): Condensed 2-item scrolling
- **Tiny screens** (128x128): Icon-based navigation
- **LED Matrix** (5x5): Symbol-based status display

### Feature Availability

| Feature | Cardputer | T-Embed | Core2 | StickC+ | T-Display | Others |
|---------|-----------|---------|-------|---------|-----------|--------|
| WiFi Scanner | ✅ | ✅ | ✅ | ✅ | ✅ | ✅ |
| Security Audit | ✅ | ✅ | ✅ | ✅ | ✅ | ✅ |
| AI Assistant | ✅ | ✅ | ✅ | ⚠️ Limited | ✅ | ⚠️ |
| Voice Input | ✅ | ❌ | ✅ | ❌ | ❌ | ❌ |
| Pattern Learning | ✅ | ✅ | ✅ | ✅ | ✅ | ✅ |
| Background Learning | ✅ | ✅ | ✅ | ✅ | ✅ | ✅ |
| SD Card | ✅ | ✅ | ✅ | ❌ | ❌ | Varies |

---

## 🛠️ Building for Different Devices

### PlatformIO Environments

The `platformio.ini` now includes multiple environments:

```bash
# Build for M5Stack Cardputer (default)
pio run -e m5stack-cardputer

# Build for LilyGO T-Embed
pio run -e lilygo-t-embed

# Build for M5Stack Core2
pio run -e m5stack-core2

# Build for LilyGO T-Display S3
pio run -e lilygo-t-display-s3

# Build for M5Stack StickC Plus
pio run -e m5stack-stickc-plus

# Build for LilyGO T-QT Pro
pio run -e lilygo-t-qt

# Build all devices
pio run
```

### Automatic Hardware Detection

The firmware automatically detects the device type at boot:

- Checks display resolution
- Detects available input methods
- Probes for peripherals (mic, SD, encoder)
- Adapts UI and features accordingly

---

## 📦 Pre-built Binaries

### Download by Device

Coming soon! Pre-built firmware files for each device:

```
/releases/v3.1.0/
├── cardputer-thunder-v3.1.0.bin
├── t-embed-thunder-v3.1.0.bin
├── core2-thunder-v3.1.0.bin
├── t-display-s3-thunder-v3.1.0.bin
├── stickc-plus-thunder-v3.1.0.bin
└── README.md
```

---

## 🎯 Device Selection Guide

### Best for Hacking: M5Stack Cardputer

- Full keyboard for fast input
- Large screen
- All features work
- **Recommended** for main use

### Best Portable: LilyGO T-Dongle S3

- USB-A stick form factor
- Plug directly into computer
- Quick WiFi scanning on-the-go

### Best for Wearable: LilyGO T-Watch S3

- Always-on wrist access
- Quick security checks
- Vibration alerts

### Best for Original Experience: LilyGO T-Embed

- Rotary encoder navigation
- Skeletor Edition origins
- SD card support

### Best Budget: LilyGO T-QT Pro

- Cheapest option (~$10)
- All core features work
- Ultra-compact

---

## 🔧 Adding New Devices

Want to port to a new device? Here's what you need:

### Hardware Requirements

- ✅ ESP32 or ESP32-S3
- ✅ At least 4MB flash
- ✅ Display (any size, any driver)
- ✅ At least 1 input button/touch

### Optional but Recommended

- 8MB+ flash for full features
- PSRAM for smoother operation
- SD card for data storage
- WiFi/Bluetooth (built into ESP32)

### Porting Process

1. Add new environment to `platformio.ini`
2. Define pins in `src/hal/device_config.h`
3. Implement input driver in `src/hal/input_driver.cpp`
4. Test and submit PR!

---

## 💰 Licensing for Multiple Devices

### Single Purchase Covers

- ✅ Unlimited personal devices you own
- ✅ All supported device types
- ✅ Mix and match (Cardputer + T-Embed + Core2, etc.)

### Commercial License Required For

- ❌ Reselling devices with pre-installed firmware
- ❌ Corporate/business deployments (>5 devices)
- ❌ Offering as a service

Contact for commercial licensing.

---

## 📊 Device Comparison Table

| Device | Price | Screen | Input | WiFi | Battery | Mic | SD Card | Best For |
|--------|-------|--------|-------|------|---------|-----|---------|----------|
| Cardputer | $60 | 240x135 | QWERTY | ✅ | ✅ | ✅ | ✅ | **Daily Driver** |
| T-Embed | $30 | 170x320 | Encoder | ✅ | ✅ | ❌ | ✅ | Retro Feel |
| Core2 | $50 | 320x240 | Touch | ✅ | ✅ | ✅ | ✅ | Touch UI |
| StickC+ | $20 | 135x240 | 2 Buttons | ✅ | ✅ | ❌ | ❌ | Pocket Tool |
| T-Display S3 | $15 | 170x320 | 2 Buttons | ✅ | Optional | ❌ | ❌ | Budget Choice |
| T-QT Pro | $10 | 128x128 | 1 Button | ✅ | Optional | ❌ | ❌ | Ultra Compact |
| T-Dongle S3 | $12 | 80x160 | 1 Button | ✅ | ❌ | ❌ | ❌ | USB Stick |
| T-Watch S3 | $70 | 240x240 | Touch+Btn | ✅ | ✅ | ❌ | ❌ | Wearable |

---

## 🎉 Purchase Options

### Individual Device License: $5

- Works on one device type
- Example: Cardputer-only license

### Universal License: $15 (Save $25!)

- **All supported devices**
- Mix and match unlimited
- Future device support included
- **Recommended for collectors**

### Developer License: $50

- All devices + source access
- Port to new devices
- Contribute back to project
- Commercial use (up to 10 devices)

---

## 🚀 Coming Soon

### Planned Device Support

- [ ] LilyGO T-HMI
- [ ] M5Stack Fire
- [ ] M5Stack Paper
- [ ] ESP32-S3-Box
- [ ] Seeed XIAO ESP32-S3 Sense

Vote for your device in GitHub Issues!

---

**Questions?** Open an issue or check the documentation.

**Want a device ported?** Sponsor development and get it prioritized!
