/**
 * @file MainWindow.cpp
 * @brief Implementation of the MainWindow class.
 */

#include "MainWindow.h"

// Core includes
#include "AudioRecorder.h"
#include "ExportService.h"
#include "PlaybackEngine.h"
#include "RythmoManager.h"
#include "SaveManager.h"

// GUI includes
#include "ClickableSlider.h"
#include "RythmoOverlay.h"
#include "TrackPanel.h"
#include "TrackSettingsDialog.h"
#include "VideoWidget.h"

// Utils includes
#include "TimeFormatter.h"

#include <QDir>
#include <QEvent>
#include <QFile>
#include <QFileDialog>
#include <QFrame>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QResizeEvent>
#include <QStandardPaths>
#include <QStatusBar>
#include <QVBoxLayout>

#include <QFutureWatcher>
#include <QProgressDialog>
#include <QtConcurrent>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
      // Initialize Core services
      ,
      m_playbackEngine(new PlaybackEngine(this)),
      m_rythmoManager(new RythmoManager(this)),
      m_exportService(new ExportService(this)),
      m_saveManager(new SaveManager(this))
      // Initialize state
      ,
      m_trackCount(0), m_previousVolume(100), m_isRecording(false),
      m_isFullscreenRecording(false), m_lastRecordedDurationMs(0),
      m_recordingStartTimeMs(0) {
  loadStylesheet();
  setupUi();
  createMenus();
  setupConnections();
  setupShortcuts();

  // Connect video sink
  m_playbackEngine->setVideoSink(m_videoWidget->videoSink());

  // Create initial track (always start with 1)
  setTrackCount(1);

  // Window configuration
  setWindowTitle("DubInstante - Studio");
  resize(900, 600);
  setMinimumSize(800, 500);
}

// =============================================================================
// UI Setup
// =============================================================================

void MainWindow::loadStylesheet() {
  QFile styleFile(":/resources/style.qss");
  if (styleFile.open(QFile::ReadOnly)) {
    QString styleSheet = QLatin1String(styleFile.readAll());
    setStyleSheet(styleSheet);
  }
}

void MainWindow::setupUi() {
  QWidget *centralWidget = new QWidget(this);
  setCentralWidget(centralWidget);

  QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);
  mainLayout->setContentsMargins(5, 5, 5, 5);
  mainLayout->setSpacing(5);

  // =========================================================================
  // Video Area with Overlay
  // =========================================================================

  m_videoFrame = new QFrame(this);
  m_videoFrame->setObjectName("videoFrame");
  m_videoFrame->setFrameStyle(QFrame::NoFrame);
  m_videoFrame->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

  m_videoWidget = new VideoWidget(m_videoFrame);
  m_videoWidget->show();

  m_rythmoOverlay = new RythmoOverlay(m_videoFrame);
  m_rythmoOverlay->show();

  QVBoxLayout *playerContainerLayout = new QVBoxLayout();
  playerContainerLayout->setContentsMargins(0, 0, 0, 0);
  playerContainerLayout->setSpacing(0);
  playerContainerLayout->addWidget(m_videoFrame, 1);

  mainLayout->addLayout(playerContainerLayout, 1);

  // Watch for resize events
  m_videoFrame->installEventFilter(this);

  // Fullscreen container (hidden until recording starts)
  m_fullscreenContainer =
      new QWidget(nullptr, Qt::Window | Qt::FramelessWindowHint);
  m_fullscreenContainer->setObjectName("fullscreenContainer");
  m_fullscreenContainer->setStyleSheet("background-color: black;");
  m_fullscreenContainer->hide();

  // =========================================================================
  // Position Slider
  // =========================================================================

  m_positionSlider = new ClickableSlider(Qt::Horizontal, this);
  m_positionSlider->setRange(0, 0);
  mainLayout->addWidget(m_positionSlider);

  // =========================================================================
  // Playback Controls
  // =========================================================================

  QHBoxLayout *controlsLayout = new QHBoxLayout();
  controlsLayout->setSpacing(10);

  m_playPauseButton =
      new QPushButton(QIcon(":/resources/icons/play.svg"), "", this);
  m_playPauseButton->setFixedSize(36, 36);
  m_playPauseButton->setIconSize(QSize(24, 24));
  controlsLayout->addWidget(m_playPauseButton);

  m_stopButton = new QPushButton(QIcon(":/resources/icons/stop.svg"), "", this);
  m_stopButton->setFixedSize(36, 36);
  m_stopButton->setIconSize(QSize(24, 24));
  controlsLayout->addWidget(m_stopButton);

  m_timeLabel = new QLabel("00:00 / 00:00", this);
  m_timeLabel->setStyleSheet(
      "color: #666; font-family: monospace; font-weight: bold;");
  controlsLayout->addWidget(m_timeLabel);

  controlsLayout->addStretch();

  // Volume controls
  m_volumeButton =
      new QPushButton(QIcon(":/resources/icons/arrow_up.svg"), "", this);
  m_volumeButton->setFixedSize(24, 24);
  m_volumeButton->setFlat(true);
  controlsLayout->addWidget(m_volumeButton);

  m_volumeSlider = new ClickableSlider(Qt::Horizontal, this);
  m_volumeSlider->setRange(0, 100);
  m_volumeSlider->setValue(100);
  m_volumeSlider->setFixedWidth(100);
  controlsLayout->addWidget(m_volumeSlider);

  m_volumeSpinBox = new QSpinBox(this);
  m_volumeSpinBox->setRange(0, 100);
  m_volumeSpinBox->setValue(100);
  m_volumeSpinBox->setFixedWidth(90);
  m_volumeSpinBox->setAlignment(Qt::AlignRight);
  m_volumeSpinBox->setSuffix("%");
  controlsLayout->addWidget(m_volumeSpinBox);

  m_recordButton =
      new QPushButton(QIcon(":/resources/icons/record.svg"), "REC", this);
  m_recordButton->setObjectName("recordButton");
  m_recordButton->setCheckable(true);
  m_recordButton->setFixedSize(90, 36);
  m_recordButton->setIconSize(QSize(16, 16));
  m_recordButton->setCursor(Qt::PointingHandCursor);
  controlsLayout->addWidget(m_recordButton);

  mainLayout->addLayout(controlsLayout);

  // =========================================================================
  // Bottom Controls (Tracks + Settings)
  // =========================================================================

  QHBoxLayout *bottomControlsLayout = new QHBoxLayout();

  // Tracks column (dynamic)
  m_tracksLayout = new QVBoxLayout();
  m_tracksLayout->setSpacing(5);

  bottomControlsLayout->addLayout(m_tracksLayout);
  bottomControlsLayout->addStretch();

  // Speed controls column
  QVBoxLayout *speedLayout = new QVBoxLayout();
  speedLayout->setSpacing(2);
  speedLayout->addWidget(new QLabel("Vitesse Défilement:", this));

  m_speedSpinBox = new QSpinBox(this);
  m_speedSpinBox->setRange(1, 400);
  m_speedSpinBox->setValue(100);
  m_speedSpinBox->setSuffix("%");
  m_speedSpinBox->setFixedWidth(90);
  m_speedSpinBox->setAlignment(Qt::AlignRight);
  m_speedSpinBox->setSingleStep(10);
  speedLayout->addWidget(m_speedSpinBox);

  m_textColorCheck = new QCheckBox("Texte Blanc", this);
  speedLayout->addWidget(m_textColorCheck);

  bottomControlsLayout->addLayout(speedLayout);

  bottomControlsLayout->addSpacing(20);

  m_exportProgressBar = new QProgressBar(this);
  m_exportProgressBar->setVisible(false);
  bottomControlsLayout->addWidget(m_exportProgressBar);

  mainLayout->addLayout(bottomControlsLayout);

  // Initial sync
  m_rythmoOverlay->setSpeed(m_speedSpinBox->value());
  m_rythmoManager->setSpeed(m_speedSpinBox->value());
}

void MainWindow::createMenus() {
  QMenuBar *menuBar = new QMenuBar(this);
  setMenuBar(menuBar);

  // === Files Menu ===
  QMenu *filesMenu = menuBar->addMenu(tr("Files"));

  m_actionOpenMp4 = new QAction(tr("Open MP4"), this);
  connect(m_actionOpenMp4, &QAction::triggered, this, &MainWindow::onOpenFile);
  filesMenu->addAction(m_actionOpenMp4);

  m_actionLoadProject = new QAction(tr("Open save file"), this);
  connect(m_actionLoadProject, &QAction::triggered, this,
          &MainWindow::onLoadProject);
  filesMenu->addAction(m_actionLoadProject);

  m_actionSaveProject = new QAction(tr("Save .dbi / .zip"), this);
  connect(m_actionSaveProject, &QAction::triggered, this,
          &MainWindow::onSaveProject);
  filesMenu->addAction(m_actionSaveProject);

  // === Application Menu ===
  QMenu *appMenu = menuBar->addMenu(tr("Application"));

  m_actionExpertMode = new QAction(tr("Expert mode"), this);
  m_actionExpertMode->setCheckable(true);
  appMenu->addAction(m_actionExpertMode);
  m_actionFullscreen = new QAction(tr("Fullscreen mode"), this);
  m_actionFullscreen->setCheckable(true);
  appMenu->addAction(m_actionFullscreen);

  m_actionShortcuts = new QAction(tr("Changer raccourcis"), this);
  connect(m_actionShortcuts, &QAction::triggered, this,
          &MainWindow::showShortcutsPopup);
  appMenu->addAction(m_actionShortcuts);

  m_actionGlobalSettings = new QAction(tr("Paramètre global"), this);
  appMenu->addAction(m_actionGlobalSettings);

  // === Bande Rythmo Menu ===
  QMenu *rythmoMenu = menuBar->addMenu(tr("Bande Rythmo"));

  // Track count selector: [ - ] N bande(s) rythmo [ + ]
  QWidget *trackCountWidget = new QWidget(this);
  QHBoxLayout *trackCountLayout = new QHBoxLayout(trackCountWidget);
  trackCountLayout->setContentsMargins(8, 4, 8, 4);
  trackCountLayout->setSpacing(6);

  QPushButton *btnMinus = new QPushButton("−", trackCountWidget);
  btnMinus->setFixedSize(28, 28);
  btnMinus->setCursor(Qt::PointingHandCursor);
  trackCountLayout->addWidget(btnMinus);

  m_trackCountLabel = new QLabel("1 bande rythmo", trackCountWidget);
  m_trackCountLabel->setAlignment(Qt::AlignCenter);
  m_trackCountLabel->setMinimumWidth(120);
  trackCountLayout->addWidget(m_trackCountLabel);

  QPushButton *btnPlus = new QPushButton("+", trackCountWidget);
  btnPlus->setFixedSize(28, 28);
  btnPlus->setCursor(Qt::PointingHandCursor);
  trackCountLayout->addWidget(btnPlus);

  connect(btnMinus, &QPushButton::clicked, this, [this]() {
    if (!m_isRecording) {
      setTrackCount(m_trackCount - 1);
    }
  });

  connect(btnPlus, &QPushButton::clicked, this, [this]() {
    if (!m_isRecording) {
      setTrackCount(m_trackCount + 1);
    }
  });

  QWidgetAction *trackCountAction = new QWidgetAction(this);
  trackCountAction->setDefaultWidget(trackCountWidget);
  rythmoMenu->addAction(trackCountAction);

  rythmoMenu->addSeparator();

  m_actionPersonalizeRythmo = new QAction(tr("Personnaliser"), this);
  rythmoMenu->addAction(m_actionPersonalizeRythmo);

  m_actionExportRythmo = new QAction(tr("Exporter la bande rythmo"), this);
  m_actionExportRythmo->setCheckable(true);
  rythmoMenu->addAction(m_actionExportRythmo);

  // === Account Menu (Right aligned) ===
  QMenuBar *rightMenuBar = new QMenuBar(menuBar);
  rightMenuBar->setObjectName("rightMenuBar");
  QMenu *accountMenu = rightMenuBar->addMenu(tr("Account"));
  menuBar->setCornerWidget(rightMenuBar, Qt::TopRightCorner);

  QWidget *accountWidget = new QWidget(this);
  QVBoxLayout *accountLayout = new QVBoxLayout(accountWidget);
  accountLayout->setContentsMargins(10, 10, 10, 10);

  QLineEdit *emailEdit = new QLineEdit(accountWidget);
  emailEdit->setPlaceholderText(tr("Email"));

  QLineEdit *passwordEdit = new QLineEdit(accountWidget);
  passwordEdit->setPlaceholderText(tr("Password"));
  passwordEdit->setEchoMode(QLineEdit::Password);

  QPushButton *loginBtn = new QPushButton(tr("Login"), accountWidget);

  accountLayout->addWidget(emailEdit);
  accountLayout->addWidget(passwordEdit);
  accountLayout->addWidget(loginBtn);

  QWidgetAction *accountDropdownAction = new QWidgetAction(this);
  accountDropdownAction->setDefaultWidget(accountWidget);
  accountMenu->addAction(accountDropdownAction);
}

void MainWindow::setupConnections() {
  // =========================================================================
  // Playback Controls
  // =========================================================================

  connect(m_playPauseButton, &QPushButton::clicked, this, [this]() {
    if (m_playbackEngine->playbackState() == QMediaPlayer::PlayingState) {
      m_playbackEngine->pause();
    } else {
      m_playbackEngine->play();
    }
  });

  connect(m_stopButton, &QPushButton::clicked, this, [this]() {
    m_playbackEngine->stop();
    if (m_isRecording) {
      toggleRecording();
    }
  });

  // =========================================================================
  // PlaybackEngine -> UI
  // =========================================================================

  connect(m_playbackEngine, &PlaybackEngine::positionChanged, this,
          &MainWindow::onPositionChanged);
  connect(m_playbackEngine, &PlaybackEngine::durationChanged, this,
          &MainWindow::onDurationChanged);
  connect(m_playbackEngine, &PlaybackEngine::playbackStateChanged, this,
          &MainWindow::onPlaybackStateChanged);
  connect(m_playbackEngine, &PlaybackEngine::errorOccurred, this,
          &MainWindow::onError);

  // PlaybackEngine -> RythmoManager -> RythmoOverlay
  connect(m_playbackEngine, &PlaybackEngine::positionChanged, m_rythmoManager,
          &RythmoManager::sync);
  connect(m_playbackEngine, &PlaybackEngine::positionChanged, m_rythmoOverlay,
          &RythmoOverlay::sync);
  connect(m_playbackEngine, &PlaybackEngine::playbackStateChanged, this,
          [this](QMediaPlayer::PlaybackState state) {
            m_rythmoOverlay->setPlaying(state == QMediaPlayer::PlayingState);
          });

  // =========================================================================
  // Position Slider
  // =========================================================================

  connect(m_positionSlider, &QSlider::sliderMoved, m_playbackEngine,
          &PlaybackEngine::seek);

  // Frame stepping configuration
  connect(m_playbackEngine, &PlaybackEngine::metaDataChanged, this, [this]() {
    qreal fps = m_playbackEngine->videoFrameRate();
    if (fps > 0) {
      int frameDurationMs = static_cast<int>(1000.0 / fps);
      m_positionSlider->setSingleStep(frameDurationMs);
      m_positionSlider->setPageStep(frameDurationMs * 10);
    }
  });

  // =========================================================================
  // Volume Controls
  // =========================================================================

  connect(m_volumeSlider, &QSlider::valueChanged, this, [this](int value) {
    m_playbackEngine->setVolume(static_cast<float>(value) / 100.0f);
    if (m_volumeSpinBox->value() != value) {
      m_volumeSpinBox->blockSignals(true);
      m_volumeSpinBox->setValue(value);
      m_volumeSpinBox->blockSignals(false);
    }
    if (value > 0) {
      m_previousVolume = value;
    }
  });

  connect(m_volumeSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), this,
          [this](int value) {
            m_playbackEngine->setVolume(static_cast<float>(value) / 100.0f);
            if (m_volumeSlider->value() != value) {
              m_volumeSlider->blockSignals(true);
              m_volumeSlider->setValue(value);
              m_volumeSlider->blockSignals(false);
            }
          });

  connect(m_volumeButton, &QPushButton::clicked, this, [this]() {
    if (m_volumeSlider->value() > 0) {
      m_previousVolume = m_volumeSlider->value();
      m_volumeSlider->setValue(0);
    } else {
      m_volumeSlider->setValue(m_previousVolume);
    }
  });

  connect(m_playbackEngine, &PlaybackEngine::volumeChanged, this,
          [this](float volume) {
            int val = static_cast<int>(volume * 100);
            if (m_volumeSlider->value() != val) {
              m_volumeSlider->blockSignals(true);
              m_volumeSlider->setValue(val);
              m_volumeSlider->blockSignals(false);
            }
            if (m_volumeSpinBox->value() != val) {
              m_volumeSpinBox->blockSignals(true);
              m_volumeSpinBox->setValue(val);
              m_volumeSpinBox->blockSignals(false);
            }
          });

  // =========================================================================
  // Speed & Display Settings
  // =========================================================================

  connect(m_speedSpinBox, QOverload<int>::of(&QSpinBox::valueChanged),
          m_rythmoOverlay, &RythmoOverlay::setSpeed);
  connect(m_speedSpinBox, QOverload<int>::of(&QSpinBox::valueChanged),
          m_rythmoManager, &RythmoManager::setSpeed);

  connect(m_textColorCheck, &QCheckBox::toggled, this, [this](bool checked) {
    QColor color = checked ? QColor(Qt::white) : QColor(34, 34, 34);
    for (int i = 0; i < m_trackCount; ++i) {
      RythmoTrackStyle style = m_rythmoManager->trackStyle(i);
      style.textColor = color;
      m_rythmoManager->setTrackStyle(i, style);
    }
  });

  connect(m_actionPersonalizeRythmo, &QAction::triggered, this, [this]() {
    TrackSettingsDialog *dialog =
        new TrackSettingsDialog(m_rythmoManager, m_trackCount, this);
    dialog->setAttribute(Qt::WA_DeleteOnClose);
    dialog->show();
  });

  // =========================================================================
  // Recording
  // =========================================================================

  // Update overlay styles when manager styles change
  connect(m_rythmoManager, &RythmoManager::trackStyleChanged, this,
          [this](int trackIndex, const RythmoTrackStyle &style) {
            RythmoWidget *w = m_rythmoOverlay->track(trackIndex);
            if (w) {
              w->setTrackStyle(style);
            }
          });

  // Recording
  connect(m_recordButton, &QPushButton::clicked, this,
          &MainWindow::toggleRecording);

  // =========================================================================
  // Export
  // =========================================================================

  connect(m_exportService, &ExportService::progressChanged, this,
          &MainWindow::onExportProgress);
  connect(m_exportService, &ExportService::exportFinished, this,
          &MainWindow::onExportFinished);
}

// =============================================================================
// Dynamic Track Management
// =============================================================================

void MainWindow::setTrackCount(int count) {
  count = qBound(1, count, MAX_TRACKS);
  if (count == m_trackCount)
    return;

  QString tempDir =
      QStandardPaths::writableLocation(QStandardPaths::TempLocation);

  // Add tracks if needed
  while (m_trackCount < count) {
    int idx = m_trackCount;

    // Create AudioRecorder
    AudioRecorder *recorder = new AudioRecorder(this);
    m_audioRecorders.append(recorder);
    connect(recorder, &AudioRecorder::errorOccurred, this,
            &MainWindow::onError);

    // Create TrackPanel
    TrackPanel *panel = new TrackPanel(
        QString("Piste %1").arg(idx + 1), recorder, this);
    m_trackPanels.append(panel);
    m_tracksLayout->addWidget(panel);

    // Setup temp audio path
    m_tempAudioPaths.append(
        tempDir + QString("/temp_dub_%1.wav").arg(idx + 1));

    // Initialize RythmoManager text for this track
    m_rythmoManager->setText(idx, "");

    m_trackCount++;
  }

  // Remove tracks if needed
  while (m_trackCount > count) {
    int idx = m_trackCount - 1;

    // Remove TrackPanel
    TrackPanel *panel = m_trackPanels.takeLast();
    m_tracksLayout->removeWidget(panel);
    panel->deleteLater();

    // Remove AudioRecorder
    AudioRecorder *recorder = m_audioRecorders.takeLast();
    recorder->deleteLater();

    // Remove temp path
    m_tempAudioPaths.removeLast();

    m_trackCount--;
  }

  // Update RythmoOverlay
  m_rythmoOverlay->setTrackCount(count);

  // Connect signals for all current tracks
  // (reconnecting is safe because we use lambdas with captured index)
  for (int i = 0; i < m_trackCount; ++i) {
    connectTrack(i);
  }

  // Update label
  if (m_trackCountLabel) {
    m_trackCountLabel->setText(
        QString("%1 bande%2 rythmo")
            .arg(m_trackCount)
            .arg(m_trackCount > 1 ? "s" : ""));
  }
}

void MainWindow::connectTrack(int index) {
  RythmoWidget *widget = m_rythmoOverlay->track(index);
  if (!widget)
    return;

  // Disconnect any existing connections on this widget to prevent duplicates
  disconnect(widget, nullptr, this, nullptr);
  disconnect(widget, nullptr, m_playbackEngine, nullptr);

  // Seek and play
  connect(widget, &RythmoWidget::seekRequested, m_playbackEngine,
          &PlaybackEngine::seek);
  connect(widget, &RythmoWidget::playRequested, m_playbackEngine,
          &PlaybackEngine::play);

  // Text editing: RythmoWidget -> RythmoManager
  connect(widget, &RythmoWidget::characterTyped, this,
          [this, index](const QString &character) {
            m_rythmoManager->insertCharacter(index, character);
            RythmoWidget *w = m_rythmoOverlay->track(index);
            if (w)
              w->setText(m_rythmoManager->text(index));
          });

  connect(widget, &RythmoWidget::backspacePressed, this, [this, index]() {
    m_rythmoManager->deleteCharacter(index, true);
    RythmoWidget *w = m_rythmoOverlay->track(index);
    if (w)
      w->setText(m_rythmoManager->text(index));
  });

  connect(widget, &RythmoWidget::deletePressed, this, [this, index]() {
    m_rythmoManager->deleteCharacter(index, false);
    RythmoWidget *w = m_rythmoOverlay->track(index);
    if (w)
      w->setText(m_rythmoManager->text(index));
  });

  // Navigation (frame stepping via RythmoWidget arrow keys)
  qreal fps = m_playbackEngine->videoFrameRate();
  int frameStep = (fps > 0) ? static_cast<int>(1000.0 / fps) : 40;
  connect(widget, &RythmoWidget::navigationRequested, this,
          [this, frameStep](bool forward) {
            qint64 delta = forward ? frameStep : -frameStep;
            m_playbackEngine->seek(m_playbackEngine->position() + delta);
          });

  // Text changed: RythmoWidget -> RythmoManager
  connect(widget, &RythmoWidget::textChanged, this,
          [this, index](const QString &text) {
            m_rythmoManager->setText(index, text);
          });
}

// =============================================================================
// Slots - File Operations
// =============================================================================

void MainWindow::onOpenFile() {
  QString fileName = QFileDialog::getOpenFileName(this, tr("Ouvrir"), "",
                                                  tr("Vidéos MP4 (*.mp4)"));

  if (!fileName.isEmpty()) {
    m_playbackEngine->openFile(QUrl::fromLocalFile(fileName));
    setProperty("currentVideoPath", fileName);
  }
}

void MainWindow::onSaveProject() {
  QMessageBox::StandardButton reply;
  reply = QMessageBox::question(
      this, tr("Sauvegarder"),
      tr("Voulez-vous inclure la vidéo dans l'archive ?\n(Cela créera un "
         "fichier .zip)"),
      QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);

  if (reply == QMessageBox::Cancel)
    return;

  bool saveWithVideo = (reply == QMessageBox::Yes);
  QString filter = saveWithVideo ? tr("DubInstante Archive (*.zip)")
                                 : tr("DubInstante Project (*.dbi)");
  QString suffix = saveWithVideo ? ".zip" : ".dbi";

  QString fileName = QFileDialog::getSaveFileName(
      this, tr("Sauvegarder le projet"), "", filter);

  if (fileName.isEmpty())
    return;

  if (!fileName.endsWith(suffix, Qt::CaseInsensitive)) {
    fileName += suffix;
  }

  SaveData data;
  data.videoUrl = property("currentVideoPath").toString();
  data.videoVolume = m_playbackEngine->volume();
  data.trackCount = m_trackCount;
  data.scrollSpeed = m_speedSpinBox->value();
  data.isTextWhite = m_textColorCheck->isChecked();

  // Save audio tracks
  for (int i = 0; i < m_trackCount; ++i) {
    TrackAudioSaveData audioData;
    audioData.audioInput = m_trackPanels[i]->selectedDevice().description();
    audioData.audioGain = m_trackPanels[i]->gain();
    data.audioTracks.append(audioData);
  }

  // Save rythmo tracks
  for (int i = 0; i < m_trackCount; ++i) {
    TrackSaveData trackData;
    trackData.text = m_rythmoManager->text(i);
    trackData.style = m_rythmoManager->trackStyle(i);
    data.tracks.append(trackData);
  }

  if (saveWithVideo) {
    // Check zip availability BEFORE launching thread (for specific error
    // messages)
    QString zipError;
    if (!SaveManager::isZipAvailable(&zipError)) {
      QMessageBox::critical(this, tr("Erreur de sauvegarde"), zipError);
      return;
    }

    // Show progress dialog
    QProgressDialog *progressDialog = new QProgressDialog(this);
    progressDialog->setLabelText(
        tr("Création de l'archive ZIP en cours...\nCela peut prendre quelques "
           "minutes selon la taille de la vidéo."));
    progressDialog->setRange(0, 0); // Indeterminate
    progressDialog->setCancelButton(
        nullptr); // Disable cancel for safety during zip
    progressDialog->setWindowModality(Qt::WindowModal);
    progressDialog->show();

    // Run in background thread
    QFutureWatcher<bool> *watcher = new QFutureWatcher<bool>(this);
    connect(watcher, &QFutureWatcher<bool>::finished, this,
            [this, watcher, progressDialog, fileName]() {
              bool result = watcher->result();
              progressDialog->close();
              progressDialog->deleteLater();
              watcher->deleteLater();

              if (result) {
                statusBar()->showMessage(tr("Projet sauvegardé"), 3000);
              } else {
                QMessageBox::critical(
                    this, tr("Erreur"),
                    tr("Impossible de créer l'archive ZIP.\nVérifiez l'espace "
                       "disque ou les permissions."));
              }
            });

    QFuture<bool> future = QtConcurrent::run([this, fileName, data]() {
      return m_saveManager->saveWithMedia(fileName, data);
    });
    watcher->setFuture(future);

  } else {
    if (m_saveManager->save(fileName, data)) {
      statusBar()->showMessage(tr("Projet sauvegardé"), 3000);
    } else {
      QMessageBox::critical(this, tr("Erreur"),
                            tr("Impossible de sauvegarder le projet."));
    }
  }
}

void MainWindow::onLoadProject() {
  QString fileName = QFileDialog::getOpenFileName(
      this, tr("Charger un projet"), "", tr("DubInstante Project (*.dbi)"));

  if (fileName.isEmpty())
    return;

  SaveData data;
  if (!m_saveManager->load(fileName, data)) {
    QMessageBox::critical(
        this, tr("Erreur"),
        tr("Le fichier est corrompu ou d'une version incompatible."));
    return;
  }

  // Apply loaded data
  m_speedSpinBox->setValue(data.scrollSpeed);
  m_textColorCheck->setChecked(data.isTextWhite);

  // Set track count
  int loadedTrackCount = qBound(1, data.trackCount, MAX_TRACKS);
  setTrackCount(loadedTrackCount);

  // Restore rythmo tracks
  for (int i = 0; i < qMin(data.tracks.size(), m_trackCount); ++i) {
    m_rythmoManager->setText(i, data.tracks[i].text);
    m_rythmoManager->setTrackStyle(i, data.tracks[i].style);
    RythmoWidget *w = m_rythmoOverlay->track(i);
    if (w)
      w->setText(data.tracks[i].text);
  }

  // Restore video and volume
  if (!data.videoUrl.isEmpty()) {
    QString localPath = data.videoUrl;
    if (localPath.startsWith("file://")) {
      localPath = QUrl(localPath).toLocalFile();
    }

    if (!QFile::exists(localPath)) {
      QMessageBox::warning(
          this, tr("Relink"),
          tr("La vidéo est introuvable. Veuillez la localiser."));
      onOpenFile(); // Simple relink via open file dialog
    } else {
      m_playbackEngine->openFile(QUrl::fromLocalFile(localPath));
      setProperty("currentVideoPath", localPath);
    }
  }

  m_playbackEngine->setVolume(data.videoVolume);

  // Restore audio device selection and gain
  for (int i = 0; i < qMin(data.audioTracks.size(), m_trackCount); ++i) {
    for (const auto &dev : m_audioRecorders[i]->availableDevices()) {
      if (dev.description() == data.audioTracks[i].audioInput) {
        m_trackPanels[i]->setDevice(dev);
        break;
      }
    }
    m_trackPanels[i]->setVolume(data.audioTracks[i].audioGain);
  }

  statusBar()->showMessage(tr("Projet chargé"), 3000);
}

// =============================================================================
// Slots - Playback Updates
// =============================================================================

void MainWindow::onPositionChanged(qint64 position) {
  if (!m_positionSlider->isSliderDown()) {
    m_positionSlider->setValue(static_cast<int>(position));
  }

  m_timeLabel->setText(TimeFormatter::format(position) + " / " +
                       TimeFormatter::format(m_playbackEngine->duration()));
}

void MainWindow::onDurationChanged(qint64 duration) {
  m_positionSlider->setRange(0, static_cast<int>(duration));
}

void MainWindow::onPlaybackStateChanged(QMediaPlayer::PlaybackState state) {
  if (state == QMediaPlayer::PlayingState) {
    m_playPauseButton->setIcon(QIcon(":/resources/icons/pause.svg"));
  } else {
    m_playPauseButton->setIcon(QIcon(":/resources/icons/play.svg"));
  }
}

// =============================================================================
// Slots - Recording
// =============================================================================

void MainWindow::toggleRecording() {
  if (!m_isRecording) {
    QString currentVideo = property("currentVideoPath").toString();
    if (currentVideo.isEmpty()) {
      QMessageBox::warning(this, tr("Dubbing"),
                           tr("Chargez une vidéo avant d'enregistrer."));
      m_recordButton->setChecked(false);
      return;
    }

    m_playbackEngine->seek(0);
    m_recordingStartTimeMs = m_playbackEngine->position();

    // Start recording on all tracks
    for (int i = 0; i < m_trackCount; ++i) {
      m_trackPanels[i]->startRecording(
          QUrl::fromLocalFile(m_tempAudioPaths[i]));
    }

    // Enter fullscreen if action is checked
    if (m_actionFullscreen->isChecked()) {
      enterFullscreenRecording();
    }

    // Lock rythmo editing during recording
    m_rythmoOverlay->setEditable(false);

    m_playbackEngine->play();
    m_recordingTimer.start();

    m_isRecording = true;
    m_recordButton->setText("STOP");
    m_exportProgressBar->setVisible(false);
    m_actionOpenMp4->setEnabled(false);

  } else {
    m_playbackEngine->pause();

    // Stop recording on all tracks
    for (int i = 0; i < m_trackCount; ++i) {
      m_trackPanels[i]->stopRecording();
    }

    // Exit fullscreen if active
    if (m_isFullscreenRecording) {
      exitFullscreenRecording();
    }

    // Unlock rythmo editing
    m_rythmoOverlay->setEditable(true);

    m_lastRecordedDurationMs = m_recordingTimer.elapsed();

    m_isRecording = false;
    m_recordButton->setChecked(false);
    m_recordButton->setText("REC");
    m_actionOpenMp4->setEnabled(true);

    // Prompt for save location
    QString currentVideo = property("currentVideoPath").toString();
    QString outputFile = QFileDialog::getSaveFileName(
        this, tr("Sauvegarder le doublage"),
        QDir::homePath() + "/dub_result.mp4", tr("Video (*.mp4)"));

    if (!outputFile.isEmpty()) {
      m_exportProgressBar->setVisible(true);
      m_exportProgressBar->setValue(0);

      ExportConfig config;
      config.videoPath = currentVideo;
      config.audioPath = m_tempAudioPaths[0]; // Primary track
      config.outputPath = outputFile;
      config.durationMs = m_lastRecordedDurationMs;
      config.startTimeMs = m_recordingStartTimeMs;
      config.originalVolume = m_playbackEngine->volume();

      // Add extra audio tracks (index 1+)
      for (int i = 1; i < m_trackCount; ++i) {
        config.extraAudioPaths.append(m_tempAudioPaths[i]);
      }

      m_exportService->startExport(config);
    }
  }
}

// =============================================================================
// Fullscreen Recording
// =============================================================================

void MainWindow::enterFullscreenRecording() {
  // Reparent video and rythmo into fullscreen container
  m_videoWidget->setParent(m_fullscreenContainer);
  m_rythmoOverlay->setParent(m_fullscreenContainer);

  // Layout them inside the fullscreen container
  QVBoxLayout *fsLayout = new QVBoxLayout(m_fullscreenContainer);
  fsLayout->setContentsMargins(0, 0, 0, 0);
  fsLayout->setSpacing(0);
  fsLayout->addWidget(m_videoWidget);

  // Overlay must be raised above the video
  m_rythmoOverlay->show();
  m_rythmoOverlay->raise();
  m_videoWidget->show();

  m_isFullscreenRecording = true;

  // Install event filter on fullscreen container for resize sync
  m_fullscreenContainer->installEventFilter(this);

  m_fullscreenContainer->showFullScreen();
}

void MainWindow::exitFullscreenRecording() {
  m_fullscreenContainer->hide();

  // Clean up the layout from the fullscreen container
  QLayout *fsLayout = m_fullscreenContainer->layout();
  if (fsLayout) {
    while (fsLayout->count() > 0) {
      fsLayout->takeAt(0);
    }
    delete fsLayout;
  }

  // Reparent back into the video frame
  m_videoWidget->setParent(m_videoFrame);
  m_rythmoOverlay->setParent(m_videoFrame);

  m_videoWidget->show();
  m_rythmoOverlay->show();
  m_rythmoOverlay->raise();

  // Restore geometry to match the video frame
  m_videoWidget->setGeometry(0, 0, m_videoFrame->width(),
                             m_videoFrame->height());
  m_rythmoOverlay->setGeometry(0, 0, m_videoFrame->width(),
                               m_videoFrame->height());

  m_isFullscreenRecording = false;
}

// =============================================================================
// Shortcuts
// =============================================================================

void MainWindow::setupShortcuts() {
  // Ctrl+S: Stop recording
  QShortcut *stopRecShortcut = new QShortcut(QKeySequence("Ctrl+S"), this);
  stopRecShortcut->setContext(Qt::ApplicationShortcut);
  connect(stopRecShortcut, &QShortcut::activated, this, [this]() {
    if (m_isRecording) {
      toggleRecording();
    }
  });

  // Build persistent shortcuts menu
  m_shortcutsMenu = new QMenu(tr("Raccourcis Clavier"), this);
  m_shortcutsMenu->addAction("Ctrl+S — " + tr("Arrêter l'enregistrement"));
  m_shortcutsMenu->addSeparator();
  m_shortcutsMenu->addAction("Space — " + tr("Lecture / Pause"));
  m_shortcutsMenu->addAction("← / → — " + tr("Image par image"));
  m_shortcutsMenu->addAction("Esc — " + tr("Insérer espace + lecture"));
  m_shortcutsMenu->addAction("Backspace — " + tr("Supprimer caractère"));
}

void MainWindow::showShortcutsPopup() {
  m_shortcutsMenu->popup(QCursor::pos());
}

// =============================================================================
// Slots - Export
// =============================================================================

void MainWindow::onExportProgress(int percentage) {
  m_exportProgressBar->setValue(percentage);
}

void MainWindow::onExportFinished(bool success, const QString &message) {
  m_exportProgressBar->setVisible(false);

  if (success) {
    QMessageBox::information(this, tr("Export"), message);
  } else {
    QMessageBox::critical(this, tr("Export"), message);
  }
}

// =============================================================================
// Slots - Error Handling
// =============================================================================

void MainWindow::onError(const QString &errorMessage) {
  QMessageBox::critical(this, tr("Erreur"), errorMessage);
}

// =============================================================================
// Event Handling
// =============================================================================

bool MainWindow::eventFilter(QObject *watched, QEvent *event) {
  if (event->type() == QEvent::Resize) {
    if (watched->objectName() == "videoFrame") {
      QFrame *frame = qobject_cast<QFrame *>(watched);
      if (frame) {
        if (m_videoWidget) {
          m_videoWidget->setGeometry(0, 0, frame->width(), frame->height());
        }
        if (m_rythmoOverlay) {
          m_rythmoOverlay->setGeometry(0, 0, frame->width(), frame->height());
          m_rythmoOverlay->raise();
        }
      }
    } else if (watched->objectName() == "fullscreenContainer" &&
               m_isFullscreenRecording) {
      QWidget *container = qobject_cast<QWidget *>(watched);
      if (container) {
        if (m_videoWidget) {
          m_videoWidget->setGeometry(0, 0, container->width(),
                                     container->height());
        }
        if (m_rythmoOverlay) {
          m_rythmoOverlay->setGeometry(0, 0, container->width(),
                                       container->height());
          m_rythmoOverlay->raise();
        }
      }
    }
  }
  return QMainWindow::eventFilter(watched, event);
}

void MainWindow::resizeEvent(QResizeEvent *event) {
  QMainWindow::resizeEvent(event);
}

void MainWindow::keyPressEvent(QKeyEvent *event) {
  // Escape in fullscreen recording: stop recording + exit fullscreen
  if (event->key() == Qt::Key_Escape && m_isFullscreenRecording &&
      m_isRecording) {
    toggleRecording();
    event->accept();
    return;
  }

  // Global Play/Pause via Space
  if (event->key() == Qt::Key_Space) {
    if (m_playbackEngine->playbackState() == QMediaPlayer::PlayingState) {
      m_playbackEngine->pause();
    } else {
      m_playbackEngine->play();
    }
    event->accept();
    return;
  }

  // Frame-by-frame navigation
  qreal fps = m_playbackEngine->videoFrameRate();
  int frameStep = (fps > 0) ? static_cast<int>(1000.0 / fps) : 40;

  if (event->key() == Qt::Key_Left) {
    // Only intercept if we are not in an input widget
    if (!focusWidget() || !focusWidget()->inherits("QAbstractSpinBox")) {
      m_playbackEngine->seek(m_playbackEngine->position() - frameStep);
      event->accept();
      return;
    }
  } else if (event->key() == Qt::Key_Right) {
    // Only intercept if we are not in an input widget
    if (!focusWidget() || !focusWidget()->inherits("QAbstractSpinBox")) {
      m_playbackEngine->seek(m_playbackEngine->position() + frameStep);
      event->accept();
      return;
    }
  }

  QMainWindow::keyPressEvent(event);
}
