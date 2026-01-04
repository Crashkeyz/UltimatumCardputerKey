# AI Assistant - Thunder Castle Edition

## Overview

The AI Assistant feature adds intelligent interaction capabilities to your Cardputer ADV. It can respond to queries via cloud AI and learn custom keyboard shortcuts/actions.

## Features

- **Cloud AI Integration**: Connect to AI APIs for intelligent responses
- **Pattern Learning**: Teach the device custom shortcuts
- **Offline Fallback**: Basic responses when WiFi unavailable
- **Persistent Memory**: Learned patterns saved to SD card

## Setup

### 1. SD Card Files

Create these files on your SD card:

#### `/ai_key.txt`

```
your-api-key-here
```

Add your DeepSeek API key (or compatible OpenAI-format API key).

- Get a DeepSeek API key at: <https://platform.deepseek.com/>
- DeepSeek is more affordable than OpenAI (~$0.001 per request)

#### `/patterns.txt` (optional - created automatically)

```
wifi->wifi
scan->scan
status->about
```

Format: `trigger->action`

### 2. WiFi Connection

The AI Assistant requires WiFi for API calls:

1. Go to **WiFi Scanner**
2. Press **S** to scan networks
3. Press **1-3** to select your network
4. Enter password and press **Enter**

## Usage

### Basic Queries

1. Select **AI Assistant** from main menu
2. Type your question using the keyboard
3. Press **Enter** to send
4. Wait for response (shows "Processing...")
5. Press **ESC** to return to menu

### Learning Patterns

Teach the device custom shortcuts:

```
learn:wifi->wifi
```

This creates a shortcut where typing "wifi" opens WiFi Scanner.

**Format**: `learn:trigger->action`

**Built-in Actions**:

- `wifi` - Opens WiFi Scanner
- `scan` - Opens Security Audit
- (More coming soon)

### Offline Mode

Without WiFi, the AI Assistant provides:

- Helpful suggestions based on keywords
- Access to learned patterns
- Instructions for WiFi setup

## Examples

### Cloud AI Queries

```
what is the weather
calculate 25 * 4
explain esp32
```

### Learning Commands

```
learn:w->wifi
learn:s->scan
learn:hello->wifi
```

### Using Learned Patterns

After learning, just type the trigger:

```
w
```

This will execute the learned action (e.g., open WiFi Scanner).

## API Configuration

### DeepSeek API (Recommended)

- **Cost**: ~$0.001 per request (100x cheaper than GPT-4)
- **Model**: deepseek-chat
- **Speed**: Fast responses
- **Sign up**: <https://platform.deepseek.com/>

### Custom API

To use a different API, edit `main.cpp`:

```cpp
// Line ~826 in callAIAPI()
http.begin("https://your-api-endpoint.com/v1/chat/completions");
doc["model"] = "your-model-name";
```

## Display

- **Header**: "AI ASSIST" (red, size 3)
- **Query**: Cyan text with cursor
- **Response**: Green text (up to 3 lines)
- **Processing**: Yellow "Processing..." indicator

## Keyboard Controls

- **Any key**: Type query (max 100 chars)
- **Backspace**: Delete character
- **Enter**: Send query
- **ESC**: Exit to main menu

## Limitations

- Max query length: 100 characters
- Max response display: ~111 characters (word wrapped)
- Max learned patterns: 10
- Requires WiFi for AI API
- API key must be on SD card

## Troubleshooting

### "No API key" Error

1. Check SD card is inserted
2. Create `/ai_key.txt` on SD card root
3. Paste your API key (single line)
4. Restart device

### "Connect WiFi" Message

1. Go to WiFi Scanner
2. Connect to your network
3. Return to AI Assistant

### "API error" Response

- Check API key is valid
- Verify WiFi connection
- Check Serial Monitor (115200 baud) for details
- Ensure API endpoint is accessible

### SD Card Not Detected

- Verify SD card is formatted as FAT32
- Check card is fully inserted
- Try a different SD card (some cards incompatible)
- Watch boot messages on Serial Monitor

## Serial Monitoring

Connect via USB at 115200 baud to see:

```
=== SKELETOR AI BOOT ===
SD Card: OK
Loaded 2 patterns from SD
AI Key: Loaded
```

During AI queries, you'll see:

```
AI API Request:
{"model":"deepseek-chat","messages":[...]}
AI API Response:
{"choices":[{"message":{"content":"..."}}]}
```

## Advanced: Pattern Actions

Current supported actions:

- `wifi` - Opens WiFi Scanner
- `scan` - Opens Security Auditor

To add more actions, edit `processAIQuery()` in `main.cpp`:

```cpp
} else if (patterns[i].action == "your_action") {
    // Your code here
}
```

## Memory Usage

- RAM: 15.1% (49,632 bytes)
- Flash: 83.6% (1,095,353 bytes)
- AI features add ~5% flash overhead

## Libraries Required

- HTTPClient (ESP32 built-in)
- ArduinoJson@7.4.2
- WiFiClientSecure (for HTTPS)

## Version

Firmware: `3.1.0-ai-thunder`

## Credits

- Thunder Castle Edition by @YourName
- DeepSeek AI integration
- Pattern learning system
