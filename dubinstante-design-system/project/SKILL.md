---
name: dubinstante-design
description: Use this skill to generate well-branded interfaces and assets for DubInstante (a professional video dubbing studio with rythmo band — desktop Qt 6 app + Android), either for production or throwaway prototypes/mocks/etc. Contains essential design guidelines, colors, type, fonts, assets, and UI kit components for prototyping.
user-invocable: true
---

Read the README.md file within this skill, and explore the other available files.

Key files:
- `README.md` — product context, content fundamentals, visual foundations, iconography
- `colors_and_type.css` — drop-in CSS variables (purple scale, neutrals, type, radii, shadows, spacing, rythmo tokens)
- `assets/logo.png`, `assets/icons/*.svg` — brand + transport/navigation icons from the repo
- `preview/*.html` — small component cards you can copy patterns from
- `ui_kits/studio/` — interactive React recreation of the Studio window

If creating visual artifacts (slides, mocks, throwaway prototypes, etc), copy assets out and create static HTML files for the user to view. Pull in `colors_and_type.css` and follow the light-theme, purple-led, minimalist-pro tone documented in README.md. Use Inter Tight for UI, JetBrains Mono for timecodes/shortcuts.

If working on production code, you can copy assets and read the rules here to become an expert in designing with this brand.

If the user invokes this skill without any other guidance, ask them what they want to build or design (a screen, a slide, a marketing page, a flow), ask some questions, and act as an expert designer who outputs HTML artifacts or production code, depending on the need.
