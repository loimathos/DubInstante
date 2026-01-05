#ifndef AUDIORECORDERMANAGER_H
#define AUDIORECORDERMANAGER_H

#include <QAudioDevice>
#include <QAudioInput>
#include <QMediaCaptureSession>
#include <QMediaDevices>
#include <QMediaRecorder>
#include <QObject>
#include <QUrl>

/**
 * @brief Manages audio input and recording capabilities.
 */
class AudioRecorderManager : public QObject {
  Q_OBJECT

public:
  explicit AudioRecorderManager(QObject *parent = nullptr);

  // Device Management
  QList<QAudioDevice> availableDevices() const;
  void setDevice(const QAudioDevice &device);
  void setVolume(float volume); // 0.0 to 1.0

  // Recording Control
  void startRecording(const QUrl &outputUrl);
  void stopRecording();

  QMediaRecorder::RecorderState recorderState() const;

signals:
  void errorOccurred(const QString &error);
  void durationChanged(qint64 duration);
  void recorderStateChanged(QMediaRecorder::RecorderState state);

private:
  QMediaCaptureSession m_captureSession;
  QAudioInput *m_audioInput;
  QMediaRecorder *m_recorder;
};

#endif // AUDIORECORDERMANAGER_H
