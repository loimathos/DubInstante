# DUBSync - Logiciel de Doublage Vidéo

DUBSync est un logiciel de doublage vidéo conçu pour être performant et intuitif. Il permet de lire une vidéo, d'écrire des textes de doublage sur une bande rythmo, et d'enregistrer la voix en synchronisation.

## Architecture

- **RythmoWidget** : Bande rythmo synchronisée avec la vidéo. Supporte l'édition directe du texte et la navigation temporelle.
- **AudioRecorderManager** : Gère la capture audio multipiste et la sélection des périphériques d'entrée.
- **Exporter** : Utilise FFmpeg pour fusionner la vidéo originale avec le doublage enregistré.
- **PlayerController** : Coeur de la lecture multimédia (Qt 6 Multimedia).
- **VideoWidget** : Rendu vidéo accéléré par OpenGL.

## Prérequis

- **Qt 6.5+** (Modules : `Widgets`, `Multimedia`, `OpenGLWidgets`)
- **FFmpeg** : Requis pour l'exportation finale (`sudo apt install ffmpeg` sur Linux).
- **Codecs (GStreamer)** : Pour la lecture des fichiers MP4 sur Linux.
    ```bash
    sudo apt install gstreamer1.0-libav gstreamer1.0-plugins-good gstreamer1.0-plugins-bad gstreamer1.0-plugins-ugly
    ```

## Installation & Build

### Windows
Le projet est automatiquement compilé pour Windows via GitHub Actions.
1. Allez sur l'onglet **Actions** de ce dépôt.
2. Téléchargez le dernier artefact **DUBSync-Windows**.

### Linux (Compilation manuelle)
1. Installez les dépendances :
   ```bash
   sudo apt install qt6-multimedia-dev libqt6opengl6-dev ffmpeg
   ```
2. Build :
   ```bash
   mkdir build && cd build
   cmake ..
   make -j$(nproc)
   ./DUBSync
   ```

## Utilisation & Raccourcis

- **Espace** : Play / Pause.
- **Echap** : Insère un espace sur la bande rythmo et lance la lecture.
- **Flèches Gauche/Droite** : Navigation image par image.
- **Édition directe** : Tapez directement sur la bande rythmo pour ajouter du texte.
- **Enregistrement** : Sélectionnez votre micro, cliquez sur "RECORD", puis sauvegardez le résultat.

## Pipeline CI/CD
Le projet utilise GitHub Actions pour garantir la stabilité et fournir des builds Windows prêts à l'emploi. Chaque commit déclenche une vérification de compilation et génère un binaire téléchargeable.
