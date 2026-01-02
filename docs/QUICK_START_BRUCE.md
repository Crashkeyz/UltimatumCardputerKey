# Quick Start: Bruce Firmware Sideload

## Visual Overview

```
┌─────────────────────────────────────────────────────────────┐
│  Your Computer                                               │
│  ┌──────────────┐    ┌──────────────┐    ┌──────────────┐  │
│  │   Build      │ -> │  Install     │ -> │   SD Card    │  │
│  │   Firmware   │    │   Script     │    │   Ready!     │  │
│  └──────────────┘    └──────────────┘    └──────────────┘  │
└─────────────────────────────────────────────────────────────┘
                              |
                              v
                    ┌──────────────────┐
                    │  Insert SD Card  │
                    │  into Cardputer  │
                    └──────────────────┘
                              |
                              v
┌─────────────────────────────────────────────────────────────┐
│  M5Stack Cardputer                                           │
│  ┌──────────────┐    ┌──────────────┐    ┌──────────────┐  │
│  │    Boot      │ -> │   Select     │ -> │  Ultimatum   │  │
│  │    Bruce     │    │  Ultimatum   │    │   Running!   │  │
│  └──────────────┘    └──────────────┘    └──────────────┘  │
└─────────────────────────────────────────────────────────────┘
```

## TL;DR - Fast Installation

### Prerequisites
- M5Stack Cardputer with Bruce Firmware installed
- microSD card (FAT32 formatted)
- Built firmware (see below)

### 3-Step Installation

1. **Build or Download Firmware**
   ```bash
   pio run -e m5stack-cardputer
   ```
   Or download from [Releases](https://github.com/Crashkeyz/UltimatumCardputerKey/releases)

2. **Run Installation Script**
   
   **Linux/macOS:**
   ```bash
   ./install-bruce-sideload.sh /path/to/sdcard
   ```
   
   **Windows:**
   ```cmd
   install-bruce-sideload.bat E:
   ```
   (Replace `E:` with your SD card drive letter)

3. **Load in Bruce Firmware**
   - Insert SD card into Cardputer
   - Boot Bruce Firmware
   - Navigate to Sideload/Apps menu
   - Select "Ultimatum Cardputer"

## Manual Installation (No Scripts)

If you prefer to do it manually:

1. Create directory on SD card: `SD:/firmware/ultimatum/`
2. Copy these files to that directory:
   - `.pio/build/m5stack-cardputer/firmware.bin`
   - `bruce_manifest.json` (rename to `manifest.json`)
   - `default_16MB.csv`
3. Insert SD card and boot Bruce Firmware
4. Load from sideload menu

## Directory Structure on SD Card

```
SD:/
└── firmware/
    └── ultimatum/
        ├── firmware.bin
        ├── manifest.json
        └── default_16MB.csv
```

## Troubleshooting

**Problem:** Firmware not showing in Bruce menu
- **Solution:** Check folder is at `SD:/firmware/ultimatum/` exactly

**Problem:** Build failed
- **Solution:** Run `pio run --target clean` then rebuild

**Problem:** SD card not detected
- **Solution:** Format as FAT32, try different card

## Full Documentation

For detailed instructions, see [BRUCE_FIRMWARE_SIDELOAD.md](BRUCE_FIRMWARE_SIDELOAD.md)

## Support

Issues? Visit: https://github.com/Crashkeyz/UltimatumCardputerKey/issues
