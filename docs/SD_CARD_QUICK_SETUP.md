# Thunder Castle Edition - SD Card Quick Setup

## Required SD Card Files

### 1. AI API Key (`/ai_key.txt`)

```
sk-xxxxxxxxxxxxxxxxxxxxx
```

- Get DeepSeek API key: <https://platform.deepseek.com/>
- Single line, no spaces
- Cost: ~$0.001 per query (very cheap)

### 2. Credentials (`/creds.txt`) - Optional

```
wifi_ssid=YourNetwork
wifi_pass=YourPassword
bt_name=DeviceName
bt_pin=0000
```

### 3. Learned Patterns (`/patterns.txt`) - Auto-created

```
wifi->wifi
scan->scan
```

Created automatically when using "learn:" commands

## SD Card Setup Steps

1. **Format Card**
   - Use FAT32 format
   - 8GB-32GB recommended
   - Class 10 or faster

2. **Create Files**
   - On computer, create `ai_key.txt`
   - Paste your DeepSeek API key
   - Save and eject safely

3. **Insert Card**
   - Power off Cardputer
   - Insert card (label facing display)
   - Push until it clicks
   - Power on

4. **Verify**
   - Open Serial Monitor (115200 baud)
   - Look for: `SD Card: OK` and `AI Key: Loaded`

## Troubleshooting

| Problem | Solution |
|---------|----------|
| SD Card: FAIL | Reinsert card, try different card, format as FAT32 |
| AI Key: Missing | Check filename is `ai_key.txt`, place in root directory |
| Card not detected | Use 8-32GB card, format with SD Card Formatter tool |

## Hardware Pins

```
SCK:  GPIO40
MISO: GPIO39
MOSI: GPIO14
CS:   GPIO12
```

## File Locations

```
/ai_key.txt     → DeepSeek API key
/creds.txt      → WiFi/BT credentials  
/patterns.txt   → Learned shortcuts
```

## Getting DeepSeek API Key

1. Go to <https://platform.deepseek.com/>
2. Sign up for free account
3. Create API key
4. Copy key to `ai_key.txt`
5. Add $5 credit (lasts months at ~$0.001/query)

## Testing

After setup, try:

1. Go to **AI Assistant** in menu
2. Type: `hello`
3. Press Enter
4. Should see AI response (if WiFi connected)

If no WiFi, learn a pattern:

```
learn:test->wifi
```

Then type `test` to trigger WiFi Scanner

## Security

- SD card contains API keys and passwords
- Remove when not in use
- Don't share between devices
- Keep backup of patterns.txt
