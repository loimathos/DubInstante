# Changelog

All notable changes to DUBSync will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [1.1.0] - 2026-01-21

### Added
- **Dual Track Recording**: Second independent audio input for simultaneous two-track dubbing
- **Track 2 Controls**: Dedicated microphone selection, gain control, and volume slider for second track
- **Track 2 Toggle**: "Activer Piste 2" checkbox to enable/disable second track recording
- **Dual Rythmo Display**: Second rythmo band overlay for complex dubbing workflows
- **Multi-Track Export**: FFmpeg export now merges both audio tracks with the original video

### Changed
- Refactored audio recording architecture to support multiple `AudioRecorderManager` instances
- Enhanced UI layout with collapsible Track 2 section
- Improved rythmo overlay system with transparent dual-band rendering

### Technical
- Added `AudioRecorderManager2` for second track audio capture
- Implemented `RythmoOverlay` component for dual rythmo display
- Extended `Exporter` to handle multi-track audio merging
- Updated file paths for second track (`temp_dub_2.wav`, `recorded_audio_2.wav`)

---

## [1.0.0] - 2026-01-12

### Initial Release
- **Single Track Recording**: Professional audio dubbing with one microphone input
- **Rythmo Band**: Interactive scrolling text synchronized with video playback
- **Video Playback**: Hardware-accelerated player with frame-by-frame navigation
- **Audio Controls**: Microphone selection, gain adjustment, and volume control
- **Speed Control**: Variable playback speed (1% to 400%) for practice
- **Export Functionality**: FFmpeg-based video/audio merging
- **Modern UI**: Clean, professional interface with Qt 6 styling
- **Keyboard Shortcuts**: Space (play/pause), Esc (insert & play), arrows (frame navigation)

### Features
- OpenGL-accelerated video rendering
- Direct text input on rythmo band
- Real-time audio monitoring
- WAV recording with configurable gain
- Interactive timeline scrubbing
- Cross-platform support (Windows, Linux)

### Core Infrastructure
- **Complete Dubbing Foundation**: Built the entire professional dubbing system from scratch
- **PlayerController Engine**: Full multimedia playback architecture with Qt 6 Multimedia
- **Audio Recording Pipeline**: High-quality WAV capture system with device management
- **Rythmo Synchronization**: Real-time text band synchronized with video timeline
- **Export Pipeline**: FFmpeg integration for video/audio merging
- **OpenGL Rendering**: Hardware-accelerated video display system
- **UI Framework**: Modern, responsive interface with custom Qt styling

---


**Legend:**
- 🎉 Added: New features
- ✨ Changed: Changes to existing functionality
- 🐛 Fixed: Bug fixes
- 🚀 Deprecated: Soon-to-be removed features
- ❌ Removed: Removed features
- 🔒 Security: Security improvements
