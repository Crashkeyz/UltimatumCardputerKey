# Evil Portal & Firmware Access Guide

## Overview

The Cardputer firmware now supports two methods for accessing and updating firmware:

1. **Evil Portal** - A captive portal web interface for wireless access
2. **SD Card Sideload** - Direct firmware updates from SD card

## Evil Portal

### What is Evil Portal?

Evil Portal is a captive portal that creates a WiFi access point on your Cardputer. When users connect to this WiFi, they are automatically redirected to a web interface where they can:

- Browse files on the SD card
- Upload firmware updates
- View device information
- Manage the Cardputer remotely

### How to Use Evil Portal

1. **Start the Portal**
   - Press the `P` key on your Cardputer keyboard
   - The device will create a WiFi access point named: `Cardputer-Portal`
   - No password is required (open network)

2. **Connect to the Portal**
   - On your phone/laptop, connect to the WiFi network: `Cardputer-Portal`
   - Open any web browser
   - You will be automatically redirected to the portal interface
   - If not redirected, navigate to: `http://192.168.4.1`

3. **Portal Features**
   - **File Browser**: View and manage files on the SD card
   - **Firmware Upload**: Upload new `.bin` firmware files
   - **Device Info**: View free memory and SD card status
   - **System Control**: Restart or manage the device

4. **Stop the Portal**
   - Press the `S` key on your Cardputer to stop the portal
   - The WiFi access point will be shut down

### Security Considerations

⚠️ **Important Security Notes:**

- The Evil Portal is an **open** WiFi network (no password)
- Anyone within range can connect
- Do not use in public places or unsecured environments
- Only enable when you need to perform updates
- Disable immediately after use

### Uploading Firmware via Portal

1. Prepare your firmware `.bin` file
2. Start the Evil Portal (press `P`)
3. Connect to `Cardputer-Portal` WiFi
4. Open the web interface
5. Navigate to "Firmware Update" section
6. Click "Choose File" and select your `.bin` file
7. Click "Upload Firmware"
8. Wait for the upload to complete
9. The device will automatically restart with the new firmware

## SD Card Firmware Sideload

### Overview

The SD card sideload feature allows you to update firmware directly from a microSD card without needing a computer connection or WiFi.

### How to Use SD Card Sideload

1. **Prepare the SD Card**
   ```
   - Format your SD card as FAT32
   - Insert the SD card into your Cardputer
   ```

2. **Place Firmware on SD Card**
   ```
   - Copy your firmware `.bin` file to the SD card
   - Place it in: /firmware/autoload.bin
   
   Directory structure:
   SD Card Root/
   └── firmware/
       └── autoload.bin
   ```

3. **Automatic Update on Boot**
   - Restart your Cardputer
   - On boot, the device will:
     - Detect the `autoload.bin` file
     - Display update progress on screen
     - Install the new firmware
     - Rename the file to `autoload.bin.old`
     - Restart with the new firmware

4. **Manual File Management**
   - Press `F` key to view firmware files on SD card
   - The serial monitor will list all files in `/firmware/` directory

### Directory Structure

The firmware sideload system creates the following structure on your SD card:

```
SD Card Root/
├── firmware/
│   ├── README.txt          # Instructions
│   ├── autoload.bin        # Place your firmware here
│   ├── autoload.bin.old    # Previous firmware (after update)
│   ├── backup_*.bin        # Firmware backups (optional)
│   └── status.txt          # Last update status
└── data/                   # Your data files
```

### Firmware Update Process

The update process follows these steps:

1. **Detection** - On boot, checks for `/firmware/autoload.bin`
2. **Validation** - Verifies the file size and format
3. **Display** - Shows update UI with progress bar
4. **Installation** - Writes firmware to flash memory (with progress %)
5. **Verification** - Ensures update completed successfully
6. **Cleanup** - Renames file to `.old` to prevent re-flashing
7. **Reboot** - Restarts device with new firmware

### Status Messages

After an update, check `/firmware/status.txt` for details:

- `SUCCESS: Firmware updated successfully` - Update completed
- `ERROR: Cannot open firmware file` - File access problem
- `ERROR: Not enough space for OTA` - Insufficient flash memory
- `ERROR: <specific error>` - Other update failures

### Troubleshooting

**Update not starting:**
- Ensure file is named exactly: `autoload.bin`
- Check file is in `/firmware/` directory
- Verify SD card is properly inserted
- Format SD card as FAT32

**Update failed:**
- Check firmware file is not corrupted
- Ensure firmware is for ESP32-S3
- Verify sufficient free space in flash
- Check battery level (low power can cause failures)

**File not found:**
- Press `F` key to list firmware directory contents
- Check SD card is mounted (green LED indicator)
- Reinsert SD card and restart

## Keyboard Shortcuts

| Key | Action |
|-----|--------|
| `P` | Start Evil Portal |
| `S` | Stop Evil Portal |
| `F` | List firmware files on SD card |

## Best Practices

### For Evil Portal:

1. **Use in controlled environments only**
2. **Disable when not in use** (press `S`)
3. **Monitor connected clients** via serial console
4. **Test firmware in safe environment** before deployment

### For SD Card Sideload:

1. **Always backup** your current firmware before updating
2. **Use reliable SD cards** (Class 10 or better)
3. **Keep SD card clean** - remove old firmware files
4. **Verify firmware** before copying to SD card
5. **Check battery** - ensure sufficient power during update

## Advanced Features

### Firmware Backup

To backup your current firmware to SD card:
```cpp
// Call in your code
backup_current_firmware();
```

This creates a timestamped backup in `/firmware/backup_<timestamp>.bin`

### Custom Portal Pages

The Evil Portal can be customized by modifying `evil_portal.cpp`:

- Change the HTML template
- Add custom API endpoints
- Modify styling and layout
- Add authentication (recommended)

### Multiple Firmware Versions

Store multiple firmware versions on SD card:
```
/firmware/
├── v1.0.0.bin
├── v1.1.0.bin
├── v2.0.0.bin
└── autoload.bin  (symlink or copy of desired version)
```

## Safety & Recovery

### If Update Fails:

1. Device should reboot to previous firmware automatically
2. If stuck in boot loop:
   - Remove SD card
   - Press and hold reset button
   - Device will boot without SD card
   - Re-flash via USB if needed

### Emergency Recovery:

If device becomes unresponsive:

1. **Remove SD card** to prevent auto-update
2. **Connect via USB-C**
3. **Use esptool.py or PlatformIO** to flash firmware
4. **Check serial output** for error messages

### OTA Partition Protection:

The firmware uses a dual-partition scheme:
- If new firmware fails, device rolls back to previous version
- This protects against bricking from bad firmware

## Examples

### Example 1: Quick Firmware Update

```bash
# On your computer:
1. Copy firmware.bin to SD card as /firmware/autoload.bin
2. Eject SD card safely
3. Insert into Cardputer
4. Restart device
5. Watch progress on screen
6. Device auto-restarts with new firmware
```

### Example 2: Using Evil Portal

```bash
# On Cardputer:
1. Press 'P' key
2. Wait for "Portal Active!" message

# On your phone/laptop:
3. Connect to WiFi: "Cardputer-Portal"
4. Browser opens portal automatically
5. Go to "Firmware Update" section
6. Select firmware.bin file
7. Click "Upload Firmware"
8. Wait for completion and reboot

# On Cardputer:
9. Press 'S' to stop portal when done
```

## FAQ

**Q: Can I use both methods at the same time?**
A: No, use one method at a time. The portal cannot run while checking for SD updates.

**Q: How large can firmware files be?**
A: Maximum firmware size is limited by the partition scheme (~3.75MB per partition). Most firmware should be under 2MB.

**Q: Can I access the portal from multiple devices?**
A: Yes, multiple devices can connect to the Evil Portal simultaneously.

**Q: What happens if I remove the SD card during update?**
A: The update will fail gracefully and the device will reboot to the previous firmware. Never remove SD card during an active update.

**Q: Can I use the portal to upload other files besides firmware?**
A: Yes! The portal's file browser allows uploading any file type to the SD card.

**Q: How do I know if an update is in progress?**
A: The screen shows real-time progress percentage and status messages.

**Q: Can I customize the portal SSID?**
A: Yes, edit `AP_SSID` in `lib/evil_portal.cpp` and rebuild firmware.

## Support

For issues or questions:
- Check the serial monitor output for detailed logs
- Review `/firmware/status.txt` on SD card
- Consult the main README.md for general troubleshooting
- Report bugs on the GitHub repository

---

**Remember**: Always test firmware updates in a safe environment before deploying to production devices!
