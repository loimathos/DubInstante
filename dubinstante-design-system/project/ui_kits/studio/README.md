# Studio UI Kit

Reimagined **DubInstante Studio** desktop GUI — the core professional dubbing surface.

## What's here

- `index.html` — interactive prototype (Space to play/pause, click transport, add/remove tracks up to 4)
- `StudioApp.jsx` — root screen; owns playback + recording + track state
- `TopBar.jsx` — brand + menu bar + account
- `VideoStage.jsx` — dark 16:9 video area with embedded rythmo overlay
- `RythmoOverlay.jsx` — stack of scrolling Rythmo bands + purple cursor line
- `TransportBar.jsx` — play / stop / scrub / timecode / REC
- `TrackPanel.jsx` — one audio track row (device, live meter, gain, mute)
- `TrackStack.jsx` — track counter (1–4), speed %, Texte Blanc toggle, panel list
- `styles.jsx` — shared module styles

## Screens reproduced
1. **Idle** — video loaded, rythmo visible, paused
2. **Playing** — rythmo scrolling in sync (click ▶)
3. **Recording** — REC button active, pulsing ring, meters animate
4. **Multi-track** — use `+` in Bandes Rythmo counter to stack up to 4

## What's simplified vs production
- Video is a static dark placeholder (no QMediaPlayer)
- Meters are RNG-driven during record
- No save/load dialogs (would require file pickers)
- Track settings dialog (font / color / preset) not yet rebuilt — ask to expand
