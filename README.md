# UltimatumCardputerKey - Thunder Castle Edition 🌩️

**Professional Firmware for M5Stack Cardputer ADV (ESP32-S3)**

## 💎 PREMIUM SOFTWARE - $5.00 USD

This is a **paid, proprietary firmware** developed to fund ongoing development and support.

### Purchase & Download

- **Price**: $5.00 USD (one-time payment)
- **Includes**: Lifetime updates for v3.x, full source code access, priority support
- **Purchase via**: [Gumroad](https://YOUR_GUMROAD_LINK) | [Ko-fi](https://YOUR_KOFI_LINK) | [PayPal](mailto:YOUR_EMAIL)

### 🎁 Complimentary Access

- Existing friends and followers (pre-Jan 3, 2026) receive complimentary licenses
- Contact via GitHub Issues with your username for verification

### ⚖️ Licensing

- **License**: Proprietary (see LICENSE file)
- Personal use only - no redistribution
- Modifications allowed for personal use
- Commercial use requires separate agreement

---

## 🚀 Features

### Thunder Castle Edition v3.1.0-ai-thunder

**Advanced AI System:**

- ✅ **Smart Offline AI** - Context-aware responses without internet
- ✅ **DeepSeek API Integration** - Full conversational AI with API key
- ✅ **Background Learning** - Experiments every 30s, discovers hardware capabilities
- ✅ **Pattern Learning** - Teach custom commands (learn:trigger->action)
- ✅ **Voice Recognition** - Microphone input with speech-to-text

**Professional Security Tools:**

- ✅ WiFi Scanner with WPA/WPA2/WPA3 detection
- ✅ Security Auditor for network analysis
- ✅ Auto-connect WiFi with password saving
- ✅ Network signal strength monitoring

**Thunder Castle Interface:**

- ✅ "HACK THE PLANET" graffiti splash screen
- ✅ Retro hacker aesthetic with 3-stage boot
- ✅ Optimized navigation (; . , / keys)
- ✅ Large, readable text (headers=3, body=2)

**Hardware Optimization:**

- ✅ Optimized partition scheme for 16MB flash
- ✅ SD card support for external data storage
- ✅ Reduced flash memory usage (85.8%)
- ✅ OTA update capability

## 📋 Requirements

### Hardware

- **Device**: M5Stack Cardputer ADV
- **MCU**: ESP32-S3-WROOM-1-N16R8
- **Flash**: 16MB
- **PSRAM**: 8MB
- **Display**: 1.14" LCD (240x135)
- **Keyboard**: Full QWERTY matrix
- **SD Card**: microSD card (FAT32 formatted, recommended)

### Software

- PlatformIO (for building and flashing)
- DeepSeek API key (free at <https://platform.deepseek.com>) - for full AI features
- USB-C cable for programming

---

## 🛠️ Installation

### 1. Purchase & License Verification

After purchasing, you'll receive:

- Access to this repository (added as collaborator)
- License key confirmation
- Priority support channel

### 2. Setup Development Environment

```bash
# Install PlatformIO
pip install platformio

# Clone repository (after purchase/access granted)
git clone https://github.com/Crashkeyz/UltimatumCardputerKey.git
cd UltimatumCardputerKey

# Build firmware
pio run

# Flash to device (connect via USB-C)
pio run --target upload
```

### 3. Configure SD Card

**📖 For detailed SD card setup instructions, see [docs/SD_CARD_SETUP.md](docs/SD_CARD_SETUP.md)**

1. Format microSD card as **FAT32**
2. Create these files in root directory:
   - `ai_key.txt` - Your DeepSeek API key
   - `wifi_saved.txt` - Auto-created after first WiFi connect
   - `patterns.txt` - Auto-created when you teach patterns
   - `ai_log.txt` - Auto-created by background learning

Example `ai_key.txt`:

```
sk-your-deepseek-api-key-here
```

### 4. First Boot

1. Insert SD card into Cardputer
2. Power on device
3. Watch 3-stage boot sequence (Thunder Castle → Loading → Devil screen)
4. Splash: "HACK THE PLANET" in graffiti style
5. Main menu appears

---

## 🎮 Usage

### Navigation Controls

- **;** - Up
- **.** - Down
- **,** - Left
- **/** - Right
- **Enter** - Select
- **ESC** - Back to menu
- **M** - Toggle microphone recording

### Main Menu Options

1. **WiFi Scanner**
   - Scan nearby networks
   - Shows SSID, RSSI, security type
   - Select network to connect
   - Auto-saves password for next boot

2. **Security Auditor**
   - Network security analysis
   - Checks encryption types
   - Signal strength monitoring

3. **AI Assistant**
   - Smart offline responses (no WiFi needed!)
   - Full AI with DeepSeek API (WiFi required)
   - Voice input via microphone
   - Pattern learning system
   - Background hardware experiments

4. **About**
   - Firmware version info
   - System statistics

### AI Commands

**Offline AI (works without WiFi/API):**

- `hello` / `hi` - Friendly greeting
- `status` - System info (WiFi, RAM, API status)
- `cpu` / `speed` - Processor details
- `memory` / `ram` - RAM usage stats
- `scan wifi` - Launch WiFi scanner
- `security` - Open Security Auditor
- `discover` - Show background learning results
- `hardware` / `gpio` - Hardware capabilities
- `mic` / `voice` - Microphone info
- `joke` - Get a fun response
- `what can you do` - List all commands
- `help` - Command reference

**Pattern Learning:**

```
learn:trigger->action
Example: learn:w->wifi
Then type: w (executes WiFi scanner)
```

**Voice Input:**

1. Press **M** to start recording
2. Speak your question/command
3. Press **M** again to stop
4. AI transcribes and responds

**Background Learning:**

- Runs automatically every 30 seconds
- Experiments with GPIO pins, I2C devices, WiFi, memory
- Logs discoveries to SD card `/ai_log.txt`
- Ask "discover" to see latest findings

---

## 💻 Development

### Project Structure

```
├── src/
│   └── main.cpp           # Main firmware code
├── drivers/
│   ├── cardputer_adv_driver.cpp
│   └── cardputer_adv_driver.h
├── lib/
│   └── utilities.cpp      # Helper functions
├── docs/
│   ├── README.md
│   └── SD_CARD_SETUP.md
├── platformio.ini         # Build configuration
└── LICENSE                # Proprietary license
```

### Building from Source (License Holders Only)

```bash
# Build only
pio run

# Build and upload
pio run --target upload

# Clean build
pio run --target clean

# Monitor serial output
pio device monitor
```

### Using VS Code + PlatformIO Extension

1. Install PlatformIO IDE extension
2. Open project folder
3. Click "Build" (✓) in toolbar
4. Click "Upload" (→) to flash
5. Click "Serial Monitor" (🔌) to view output

---

## 📊 Memory Usage

- **Flash**: 85.8% (1,124,053 bytes / 1,310,720 bytes)
- **RAM**: 15.2% (49,720 bytes / 327,680 bytes)
- **SD Card**: Unlimited external storage

**Optimized partition scheme:**

- App partitions: 2 × 3.75MB (for OTA updates)
- SPIFFS: ~8.5MB (for file storage)
- NVS: 20KB (for settings)

---

## 🐛 Troubleshooting

### SD Card Not Detected

- Ensure FAT32 format (not exFAT or NTFS)
- Check card is fully inserted
- Try different SD card (max 32GB recommended)
- Firmware retries 5 times with 500ms delays

### WiFi Connection Issues

- Check password in WiFi Scanner
- Signal strength must be > -90 dBm
- WPA3 may require specific AP support

### API Not Working

- Verify DeepSeek API key in `/ai_key.txt`
- Check WiFi connection (`status` command)
- Free API keys have rate limits (check platform.deepseek.com)

### Voice Recognition Issues

- Press M to start, M to stop, then wait 3 seconds
- Speak clearly near microphone
- Check mic enabled: `mic` command

### Build Errors

- Clean build: `pio run --target clean`
- Update PlatformIO: `pio upgrade`
- Check USB cable supports data (not charge-only)

---

## 🤝 Support & Community

### For License Holders

- **Priority Support**: GitHub Issues (licensed users only)
- **Updates**: Automatic via git pull
- **Feature Requests**: Submit via Issues with [FEATURE] tag

### Getting Help

1. Check troubleshooting section above
2. Review [docs/SD_CARD_SETUP.md](docs/SD_CARD_SETUP.md)
3. Open GitHub Issue with:
   - Firmware version
   - Error message / serial output
   - Steps to reproduce

---

## 📜 License & Terms

This project is **proprietary software**. See [LICENSE](LICENSE) file for full terms.

**Summary:**

- ✅ Personal use after purchase
- ✅ Modify for personal use
- ✅ Use on your own devices
- ❌ No redistribution of source or binaries
- ❌ No commercial use without permission
- ❌ No public derivative works

**Copyright © 2026 Crashkeyz. All rights reserved.**

---

## 🙏 Acknowledgments

- M5Stack for Cardputer hardware and libraries
- DeepSeek for AI API
- PlatformIO for development platform
- ESP32 community

---

## 📞 Contact

- **Purchase**: [Add your payment link]
- **Support**: GitHub Issues (licensed users)
- **Email**: [Your email]
- **GitHub**: [@Crashkeyz](https://github.com/Crashkeyz)

**🌩️ HACK THE PLANET! 🌩️**

- ✓ Checks for secure boot status
- ✓ Checks flash encryption
- ✓ Validates WiFi password strength (min 8 chars)
- ✓ Checks Bluetooth pairing PIN presence
- ✓ **Passwords are never displayed** - only masked output (e.g., `******** (8 chars)`)
- ✓ All results logged to Serial Monitor for detailed analysis

## Troubleshooting

### "Out of Memory" Error When Flashing

If you encounter memory errors during flashing:

1. Make sure you're using the `copilot/optimize-flashing-to-sd-card` branch
2. The custom partition scheme (`default_16MB.csv`) is now optimized for your device
3. Insert an SD card to store data externally instead of in flash
4. Clean and rebuild: `pio run --target clean && pio run`

### SD Card Not Detected

If the SD card isn't mounting:

1. Ensure the SD card is properly inserted
2. Try formatting the card as FAT32
3. Check the serial monitor for initialization messages
4. Some SD cards may need lower SPI speeds (already set to 25MHz)

### Partition Scheme Issues

The firmware uses a custom 16MB partition scheme:

- Located in: `default_16MB.csv`
- Configured in: `platformio.ini`
- Provides optimal space allocation for OTA updates and storage

## License

See LICENSE file for details.
