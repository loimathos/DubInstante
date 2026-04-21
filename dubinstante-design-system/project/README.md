# DubInstante Design System

A visual system for **DubInstante** — a professional video dubbing studio with a scrolling *rythmo band* (rythmo = the vertical-scrolling lyric-style text that dubbing actors read from, in sync with the video).

This design system reimagines the existing Qt 6 / C++ desktop app with a cleaner, more minimalist professional GUI — **light theme, purple-led**.

---

## Sources

- **Codebase:** [loinstante/DubInstante](https://github.com/loinstante/DubInstante) (master branch)
  - `src/gui/MainWindow.cpp` — current layout (menu bar, video frame, rythmo overlay, controls bar, track panels)
  - `src/gui/RythmoWidget.cpp` / `RythmoOverlay.cpp` — scrolling text band, 60 FPS
  - `src/core/RythmoManager.cpp` — time→cursor math, char duration
  - `docs/en/README.md` — architecture + data flow reference
  - `CHANGELOG.md` — feature evolution (v0.0 → v0.11)
- **Logo:** `DubInstante.png` from repo root → `assets/logo.png`
- **Icons:** `resources/icons/*.svg` → `assets/icons/*.svg` (play, pause, stop, record, volume, folder_open, arrows)
- **Related products (not yet explored):** [loinstante/DubWritter](https://github.com/loinstante/DubWritter), [loinstante/loinstante_website](https://github.com/loinstante/loinstante_website), [loinstante/InstanTexte](https://github.com/loinstante/InstanTexte)

Repo context was read-only via GitHub; if you want deeper coverage of the Android surface or the companion apps (DubWritter), re-scope the repo list.

---

## Product snapshot

DubInstante is a desktop (Qt 6) + Android (Kotlin/Compose) app for:
1. **Opening** a video
2. **Writing** dubbing dialogue on a scrolling *rythmo* band that slides in sync with the video
3. **Recording** voice — up to **4 simultaneous tracks** as of v0.10
4. **Exporting** via FFmpeg

The rythmo band is the defining UI element — it overlays the lower third of the video and scrolls left-to-right as playback advances, with a fixed vertical "cursor" line where the actor should be speaking *now*.

Key surfaces:
- **Studio (desktop main window)** — video player, rythmo overlay, playback controls, track panels, menu bar
- **Track settings dialog** — per-track font, text color, background color, size, presets
- **Project I/O** — `.dbi` binary / `.zip` archive save+load
- **Android app** — compact mobile-first variant

---

## Index — what's in this folder

- `README.md` — this file
- `SKILL.md` — skill entrypoint for Claude Code / Skills
- `colors_and_type.css` — CSS variables (color scales, type, radii, shadows, spacing, rythmo tokens)
- `assets/` — logo + imported SVG icons
- `preview/` — small HTML cards that populate the Design System tab
- `ui_kits/studio/` — HTML + JSX reimagining of the DubInstante desktop Studio
- `SKILL.md` — entrypoint for Claude Skills / Claude Code

---

## CONTENT FUNDAMENTALS

DubInstante is **bilingual (French + English)**, produced by a small open-source team. Copy tone observed from the README, CHANGELOG, menus, and error strings:

- **Primary language in UI:** French (`Bande Rythmo`, `Piste 1`, `Vitesse Défilement`, `Activer 2e piste`, `Personnaliser`, `Paramètre global`, `Sauvegarder`, `Chargez une vidéo avant d'enregistrer.`)
- **Secondary:** English menu items (`Files`, `Application`, `Account`, `Expert mode`, `Fullscreen mode`)
- **Bilingual is deliberate** — README and docs ship side-by-side FR/EN. When labeling features, accept both.
- **Tone:** direct, utilitarian, professional-tool. No marketing fluff. No exclamation marks in UI. Buttons are verbs (`STOP`, `REC`, `Login`, `Sauvegarder`, `Exporter la bande rythmo`). Menus are nouns (`Bande Rythmo`, `Application`, `Account`).
- **Case:**
  - Menu items & buttons: **Sentence case** in French (`Sauvegarder le projet`, `Changer raccourcis`, `Activer 2e piste`), **Title Case** in English headings (`Fullscreen Recording`)
  - **Record button** is the exception: `REC` / `STOP` in **ALL CAPS** — loud and immediate for the one-shot action
  - Micro-labels (tab headers, field captions): Title Case or uppercase tracked
- **Pronouns:** No "we"/"you" addressed to user in UI. Error strings imperative: `Chargez une vidéo avant d'enregistrer.` ("Load a video before recording.") — never passive, never apologetic.
- **Emoji:** Used **only in README/docs/changelog** for section markers (🎬 📝 ⚡ 🎙️). **Never** in the product UI. Do not add emoji to app surfaces.
- **Vibe:** Pro audio/video tool. Think Reaper, DaVinci Resolve, Logic Pro — terse labels, tabular timecodes, every pixel earns its place. The user is a working dubbing artist, not a consumer.
- **Specific examples to mirror:**
  - `00:00 / 00:00` — monospace, tabular, separator-spaced
  - `1 bande rythmo` / `2 bandes rythmo` — pluralization handled inline
  - `REC` (idle) / `STOP` (recording) — state written into the label itself
  - Error: `Le fichier est corrompu ou d'une version incompatible.` — factual, no blame

---

## VISUAL FOUNDATIONS

### Colors
- **Brand primary: purple** (`--purple-500 #7c56f5`). Used for active cursor line on the rythmo band, primary buttons, selection, focus rings, and the brand mark wash.
- **Neutrals:** cool-gray scale. `--neutral-50` page bg, `--neutral-0` surface, `--neutral-900` primary text. The source app is dark-chrome; this redesign inverts to **light theme** with the video player area staying near-black (it's essentially always a video).
- **Semantic:** green success, amber warning, **red `#e2344d` for REC / errors** (consistent with the original `record.svg` red-dot metaphor), blue info.
- **Imagery vibe:** cool, neutral. No warm tones. Gradients are used sparingly and only within the brand mark or a single hero accent — never as section backgrounds.

### Type
- **Inter Tight** for everything (UI + display). Tight tracking at display sizes, normal at body. 400/500/600/700.
- **JetBrains Mono** for anything numeric that needs alignment: timecodes, track indices, shortcuts, export progress. `font-variant-numeric: tabular-nums` always.
- Micro labels are 11px uppercase with +0.04em tracking (column headers, tab labels).

### Spacing
4px base scale (1, 2, 3, 4, 5, 6, 8, 10, 12, 16). Dense — this is a pro app with many controls on one screen. Toolbar row is 40–44px, control buttons 32–36px, rythmo band ~96–120px tall over the video.

### Backgrounds
- Page: flat `--bg` (neutral-50).
- Cards/panels: flat white with 1px `--border` hairlines. No drop shadows on panels in the main grid — shadows reserved for floating elements (menus, dialogs, popovers).
- **Video area:** always black (`#000`). Rythmo overlay sits on top as a translucent `rgba(18,18,23,0.72)` band with a subtle purple cursor line.
- **No full-bleed imagery, no repeating textures, no grain.** The content IS the video.
- One gradient exists: the brand mark itself (purple→blue, matching logo). Reuse only in brand contexts.

### Animation
- **Easing:** `cubic-bezier(0.2, 0.8, 0.2, 1)` (`--ease-out`) for most UI; `ease-in-out` for reversible state changes.
- **Duration:** 120ms (fast, hovers), 180ms (default), 260ms (panel slide-in). Never longer.
- **No bounces, no springs.** This is a pro tool — motion is informational, not expressive.
- **Rythmo scroll** is linear at 60 FPS (product mechanic, not decoration).
- **Record button** gets a subtle 1.2s pulse ring when active — the only "alive" motion in the UI.

### Hover / press states
- **Hover:** background shifts to `--surface-hover` (neutral-100) on ghost buttons; on primary buttons, to `--accent-hover` (purple-600). No lift / no shadow bloom.
- **Press:** background goes to `--accent-press` (purple-700) + scale(0.98) for primary buttons only.
- **Focus:** 3px purple ring at 24% alpha (`--ring-focus`). Always visible for keyboard users.
- **Disabled:** opacity 0.5, no pointer.

### Borders
- 1px hairline `--border` (neutral-200) on all separations.
- Inputs: 1px `--border-strong` (neutral-300) baseline, `--border-focus` (purple-500) on focus with ring.
- Dividers inside dark surfaces (rythmo band): `rgba(255,255,255,0.12)`.

### Shadows
- Four-tier elevation (`xs` / `sm` / `md` / `lg`) — soft, neutral, never colored.
- `--shadow-accent` (purple-tinted) used **only** on the primary CTA at rest if it's the main action on-screen (e.g., big Export button).
- `--ring-focus` for focus state.

### Capsules vs protection gradients
- **No protection gradients** anywhere. Contrast is solved by layout, not by fading overlays.
- **Capsules:** the rythmo band is a rounded rectangle with 12px radius. Pill buttons (radius-full) only for status chips (e.g., `● REC`, `4 TRACKS`).

### Transparency & blur
- Video-overlay rythmo: 72% alpha black, **no blur** (blur would waste GPU on a frame-syncing widget).
- Menus/popovers: opaque white with `--shadow-lg`.
- Use of blur is effectively zero — pro tools avoid it.

### Corner radii
- 4px inputs and micro-chips
- 6px small buttons
- 10px cards, panels, primary buttons
- 14px large panels / dialogs
- 20px hero cards
- 999px status pills

### Cards
- 1px border, 10–14px radius, flat white background, no shadow at rest, `--shadow-sm` when floating (dialogs, menus, popovers). Interior padding: 16–24px.

### Layout rules
- Top menu bar is fixed at 40px.
- Bottom track panel stack is fixed-height per panel (~56px) with a max of 4 visible.
- Video/rythmo area fills the remainder (min 400px).
- Everything else is flex or grid — no absolute positioning except the rythmo overlay on the video.

### Iconography
See `ICONOGRAPHY` section below.

---

## ICONOGRAPHY

DubInstante ships **10 hand-authored stroke SVG icons** in `resources/icons/` (imported to `assets/icons/`):

`play, pause, stop, record, volume, folder_open, arrow_up, arrow_down, arrow_left, arrow_right`

These are the **canonical product icons**. They match Google's Material-style stroke language (1.5–2px strokes, 24px viewbox). `record.svg` is the one filled icon — a red dot with surrounding ring, matching the universal REC convention.

**Rules for this design system:**
1. **Use repo icons first** for anything they cover (transport controls, file open, arrows, volume).
2. **For everything else** (menu, settings, close, mic, download, check, etc.) — fall back to **Lucide** (CDN: `https://unpkg.com/lucide@latest`), matching the 1.5–2px stroke / 24px viewbox style. Flag substitutions in context.
3. **No emoji in UI.** Emoji in docs only (changelog/README section headers).
4. **No unicode glyph icons** inside interactive controls. `−` and `+` appear in the track-count widget from the source code — these are typographic labels, not icons; keep them as-is.
5. **Never hand-roll new SVG icons** in this system. If Lucide doesn't cover it, ask for the real asset.

Icon coloring follows `currentColor` — inherit from text context. Active/selected icons take `--accent`.

---

## UI kits in this system

- **`ui_kits/studio/`** — the desktop Studio window (video + rythmo + controls + track panels), reimagined in light theme with purple accent. React/JSX components, clickable interactive prototype.

(Companion products — DubWritter, InstanTexte, marketing site, Android app — are *not* included in this pass. They live in sibling repos; scope them in a follow-up.)
