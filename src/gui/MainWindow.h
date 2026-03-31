/**
 * @file MainWindow.h
 * @brief Main application window.
 *
 * This is the primary UI class that orchestrates all components.
 * It creates and connects Core services to GUI widgets but contains
 * NO business logic itself.
 *
 * @note Part of the GUI layer - wiring only, no calculations.
 */

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QCheckBox>
#include <QElapsedTimer>
#include <QFrame>
#include <QKeyEvent>
#include <QLabel>
#include <QMainWindow>
#include <QMediaPlayer>
#include <QMenu>
#include <QProgressBar>
#include <QPushButton>
#include <QShortcut>
#include <QSpinBox>
#include <QVector>

#include <QAction>
#include <QLineEdit>
#include <QMenuBar>
#include <QToolButton>
#include <QWidgetAction>

// Forward declarations - Core layer
class PlaybackEngine;
class RythmoManager;
class AudioRecorder;
class ExportService;
class SaveManager;

// Forward declarations - GUI layer
class VideoWidget;
class RythmoOverlay;
class TrackPanel;
class ClickableSlider;
class QVBoxLayout;

// Forward declaration - Utils
struct ExportConfig;

/**
 * @class MainWindow
 * @brief Main application window orchestrating Core and GUI components.
 *
 * Responsibilities:
 * - Create and own Core services
 * - Create and layout GUI widgets
 * - Wire signals/slots between Core and GUI
 * - Handle top-level menu and keyboard shortcuts
 *
 * This class should be "thin" - it connects components but doesn't
 * contain business logic.
 */
class MainWindow : public QMainWindow {
  Q_OBJECT

public:
  explicit MainWindow(QWidget *parent = nullptr);
  ~MainWindow() override = default;

  static constexpr int MAX_TRACKS = 4;

protected:
  bool eventFilter(QObject *watched, QEvent *event) override;
  void keyPressEvent(QKeyEvent *event) override;
  void resizeEvent(QResizeEvent *event) override;

private slots:
  // File operations
  void onOpenFile();
  void onSaveProject();
  void onLoadProject();

  // Playback UI updates
  void onPositionChanged(qint64 position);
  void onDurationChanged(qint64 duration);
  void onPlaybackStateChanged(QMediaPlayer::PlaybackState state);

  // Recording
  void toggleRecording();

  // Export
  void onExportProgress(int percentage);
  void onExportFinished(bool success, const QString &message);

  // Error handling
  void onError(const QString &errorMessage);

private:
  void setupUi();
  void createMenus();
  void setupConnections();
  void setupShortcuts();
  void loadStylesheet();
  void enterFullscreenRecording();
  void exitFullscreenRecording();
  void showShortcutsPopup();

  // Dynamic track management
  void setTrackCount(int count);
  void connectTrack(int index);

  // =========================================================================
  // Core Services (Business Logic)
  // =========================================================================

  PlaybackEngine *m_playbackEngine;
  RythmoManager *m_rythmoManager;
  QVector<AudioRecorder *> m_audioRecorders;
  ExportService *m_exportService;
  SaveManager *m_saveManager;

  // =========================================================================
  // GUI Components
  // =========================================================================

  VideoWidget *m_videoWidget;
  RythmoOverlay *m_rythmoOverlay;
  QVector<TrackPanel *> m_trackPanels;
  QVBoxLayout *m_tracksLayout;

  // Playback controls
  QPushButton *m_playPauseButton;
  QPushButton *m_stopButton;
  ClickableSlider *m_positionSlider;
  QLabel *m_timeLabel;

  // Volume controls
  QPushButton *m_volumeButton;
  ClickableSlider *m_volumeSlider;
  QSpinBox *m_volumeSpinBox;

  // Recording controls
  QPushButton *m_recordButton;
  QSpinBox *m_speedSpinBox;
  QCheckBox *m_textColorCheck;
  QProgressBar *m_exportProgressBar;

  // Track count controls
  QLabel *m_trackCountLabel;

  // Fullscreen recording
  QFrame *m_videoFrame;
  QWidget *m_fullscreenContainer;
  QMenu *m_shortcutsMenu;

  // Menus and Actions
  QAction *m_actionOpenMp4;
  QAction *m_actionLoadProject;
  QAction *m_actionSaveProject;

  QAction *m_actionExpertMode;
  QAction *m_actionFullscreen;
  QAction *m_actionShortcuts;
  QAction *m_actionGlobalSettings;

  QAction *m_actionPersonalizeRythmo;
  QAction *m_actionExportRythmo;

  // =========================================================================
  // State
  // =========================================================================

  int m_trackCount;
  int m_previousVolume;
  bool m_isRecording;
  bool m_isFullscreenRecording;
  QStringList m_tempAudioPaths;
  QElapsedTimer m_recordingTimer;
  qint64 m_lastRecordedDurationMs;
  qint64 m_recordingStartTimeMs;
};

#endif // MAINWINDOW_H
