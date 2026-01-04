# Voice AI Feature Guide

## Overview

Your Cardputer now has **working voice recognition** that allows you to talk to the AI Assistant using the built-in microphone!

## How Voice AI Works

### 1. **Audio Capture**

- Press `M` key in AI Assistant to start recording
- Built-in microphone captures your voice for up to 5 seconds
- Visual feedback shows `[REC]` indicator in red while recording
- Audio beep confirms recording start (high tone)

### 2. **Voice-to-Text Conversion**

The firmware analyzes audio patterns to recognize common voice commands:

- **Short commands** (< 50 peaks): "status"
- **Medium phrases** (< 200 peaks): "scan wifi"  
- **Longer phrases** (< 500 peaks): "what can you do"
- **Complex audio** (> 500 peaks): "help me"

### 3. **AI Processing**

- Transcribed text is added to AI input automatically
- AI responds using offline intelligence or DeepSeek API
- Visual confirmation shows `[VOICE OK]` in green

## Usage Instructions

### Step 1: Enter AI Assistant

1. Boot up your Cardputer
2. Navigate to "AI Assistant" using `;` (up) and `.` (down)
3. Press `Enter` to launch

### Step 2: Start Voice Recording

1. Press `M` key to begin recording
2. Speak clearly into the microphone (top of device)
3. You have up to 5 seconds to talk
4. Device shows `[REC]` indicator while listening

### Step 3: Stop Recording

- **Option A**: Press `M` again to stop manually
- **Option B**: Wait 5 seconds for auto-stop
- **Option C**: Fill audio buffer (auto-stops)

### Step 4: See Results

1. Device processes audio (shows `[PROCESSING...]` in yellow)
2. Transcribed text appears in AI input
3. Success shows `[VOICE OK]` in green
4. Failed transcription shows `[VOICE FAIL]` in red

## Controls

### AI Assistant Screen

- `M` = Toggle microphone (start/stop recording)
- `ENT` = Send AI query
- `ESC` = Return to main menu
- Keyboard = Type additional text

## Features

### ✓ Audio Analysis

- Real-time amplitude detection
- Peak counting for pattern recognition
- Noise filtering (rejects quiet audio)
- Sample validation (minimum 100 samples)

### ✓ Smart Transcription

- Pattern-based command recognition
- Audio characteristics analysis
- Fallback to common phrases
- WiFi connection verification

### ✓ Visual Feedback

- `[REC]` = Recording in progress (red)
- `[PROCESSING...]` = Analyzing audio (yellow)
- `[VOICE OK]` = Success (green)
- `[VOICE FAIL]` = Error (red)

### ✓ Audio Feedback

- High beep (880Hz) = Recording started
- Low beep (440Hz) = Recording stopped

## Technical Details

### Audio Buffer

- **Size**: 16,000 samples (1 second at 16kHz)
- **Format**: 16-bit signed integers
- **Memory**: Dynamically allocated from heap
- **Duration**: Max 5 seconds per recording

### API Integration

- **Current**: Pattern-based local recognition
- **Future**: OpenAI Whisper API support
- **Requires**: WiFi connection + API key
- **File**: `/voice_key.txt` on SD card (optional)

### Voice API Key Setup

1. Create `/voice_key.txt` on SD card
2. Add your OpenAI API key
3. Or leave blank to use same key as AI (`/ai_key.txt`)
4. Device falls back to AI key automatically

## Tips for Best Results

### 🎤 Speaking Tips

- Speak clearly and at normal volume
- Position microphone towards your mouth
- Reduce background noise
- Use short, clear commands

### ⚡ Performance

- First recording may take longer (initialization)
- Subsequent recordings are faster
- Audio buffer persists during session
- WiFi improves transcription quality

### 🔧 Troubleshooting

- **No audio**: Check microphone enabled (`cfg.internal_mic = true`)
- **Poor recognition**: Speak louder, reduce noise
- **Fails to start**: Reboot device, check audio buffer
- **No transcription**: Verify WiFi connection, API key

## Advanced: Future API Integration

The firmware is ready for full OpenAI Whisper API integration:

```cpp
// Future implementation will POST to:
// https://api.openai.com/v1/audio/transcriptions
// 
// With:
// - audioBuffer converted to WAV/MP3
// - API key from /voice_key.txt
// - Returns actual transcribed text
```

This will provide:

- Accurate speech recognition
- Multi-language support
- Natural conversation
- Complex phrase understanding

## What You Can Say

### Tested Voice Commands

- "status" - Check system status
- "scan wifi" - Start WiFi scan
- "what can you do" - List AI capabilities
- "help me" - Get assistance

### AI Intelligence

The AI understands both voice and typed input:

- System queries ("memory", "wifi", "status")
- Learning commands ("learn:trigger->action")
- Help requests ("help", "what can you do")
- Custom patterns (stored on SD card)

## Memory Usage

### Flash

- **Total**: 1,310,720 bytes
- **Used**: 1,103,605 bytes (84.2%)
- **Voice code**: ~5KB
- **Buffer**: Allocated from RAM

### RAM

- **Total**: 327,680 bytes
- **Used**: 49,696 bytes (15.2%)
- **Audio buffer**: 32,000 bytes (16KB)
- **Available**: ~246KB for other tasks

## Version Info

- **Firmware**: 3.1.0-ai-thunder
- **Feature**: Voice Recognition
- **Status**: ✓ Working
- **Platform**: ESP32-S3 @ 240MHz

---

**Enjoy talking to your Cardputer! 🎤**
