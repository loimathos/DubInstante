#include "Exporter.h"
#include <QDebug>
#include <QRegularExpression>
#include <QStandardPaths>

Exporter::Exporter(QObject *parent)
    : QObject(parent), m_process(new QProcess(this)), m_totalDurationMs(0) {
  connect(m_process, &QProcess::finished, this,
          [this](int exitCode, QProcess::ExitStatus exitStatus) {
            if (exitStatus == QProcess::NormalExit && exitCode == 0) {
              emit progressChanged(100);
              emit finished(true, "Export réussi !");
            } else {
              QString error = m_process->readAllStandardError();
              emit finished(false, "Échec de l'export: " + error);
            }
          });

  connect(m_process, &QProcess::errorOccurred, this,
          [this](QProcess::ProcessError error) {
            if (error == QProcess::FailedToStart) {
              emit finished(false,
                            "FFmpeg n'a pas pu démarrer. Est-il installé ?");
            } else {
              emit finished(false, "Erreur lors de l'exécution de FFmpeg.");
            }
          });

  // Parse FFmpeg output for progress
  connect(m_process, &QProcess::readyReadStandardError, this, [this]() {
    if (m_totalDurationMs <= 0)
      return;

    QString output = m_process->readAllStandardError();
    // Debug FFmpeg output
    qDebug() << "[FFmpeg]" << output;

    // Look for various time formats: time=00:00:00.00 or time=123.45
    static QRegularExpression re("time=(\\d+):(\\d+):(\\d+).(\\d+)");
    static QRegularExpression reSec("time=(\\d+)\\.(\\d+)");

    QRegularExpressionMatch match = re.match(output);
    qint64 currentTimeMs = 0;

    if (match.hasMatch()) {
      int hours = match.captured(1).toInt();
      int mins = match.captured(2).toInt();
      int secs = match.captured(3).toInt();
      int ms = match.captured(4).toInt();
      currentTimeMs = (hours * 3600 + mins * 60 + secs) * 1000 + ms * 10;
    } else {
      match = reSec.match(output);
      if (match.hasMatch()) {
        currentTimeMs = match.captured(1).toLongLong() * 1000 +
                        match.captured(2).toInt() * 10;
      }
    }

    if (currentTimeMs > 0) {
      int percentage =
          static_cast<int>((currentTimeMs * 100) / m_totalDurationMs);
      if (percentage > 100)
        percentage = 100;
      emit progressChanged(percentage);
    }
  });
}

void Exporter::setTotalDuration(qint64 durationMs) {
  m_totalDurationMs = durationMs;
}

bool Exporter::isFFmpegAvailable() const {
  QProcess check;
  check.start("ffmpeg", QStringList() << "-version");
  check.waitForFinished();
  return (check.exitCode() == 0);
}

void Exporter::merge(const QString &videoPath, const QString &audioPath,
                     const QString &outputPath, qint64 durationMs,
                     qint64 startTimeMs, float originalVolume) {
  if (m_process->state() != QProcess::NotRunning) {
    emit finished(false, "Un export est déjà en cours.");
    return;
  }

  emit progressChanged(0);

  // Audio Mixing & High Quality Video Command
  // -filter_complex: Mix original audio (volume scaled) + recorded audio
  // -c:v libx264 -crf 18 -preset superfast: Visually lossless, fast encoding
  QStringList args;
  args << "-y";
  args << "-threads" << "0";

  // Fast seek to start time (Input Seeking)
  if (startTimeMs > 0) {
    args << "-ss" << QString::number(startTimeMs / 1000.0, 'f', 3);
  }

  args << "-i" << videoPath;
  args << "-i" << audioPath;

  // Video Settings: High Quality
  args << "-c:v" << "libx264";
  args << "-preset" << "superfast"; // Balanced speed/quality
  args << "-crf" << "18";           // Visually lossless
  args << "-pix_fmt" << "yuv420p";

  // Audio Settings: Mixing
  // [0:a] is video audio, [1:a] is dub audio
  // Check if original volume is effectively muted
  if (originalVolume < 0.01f) {
    // Just use the dub audio
    args << "-map" << "0:v:0";
    args << "-map" << "1:a:0";
    args << "-c:a" << "aac";
    args << "-b:a" << "192k";
  } else {
    // Mix both
    QString filter = QString("[0:a]volume=%1[a0];[1:a]volume=1.0[a1];[a0][a1]"
                             "amix=inputs=2:duration=longest[aout]")
                         .arg(originalVolume);
    args << "-filter_complex" << filter;
    args << "-map" << "0:v:0";
    args << "-map" << "[aout]";
    args << "-c:a" << "aac";
    args << "-b:a" << "192k";
  }

  if (durationMs > 0) {
    args << "-t" << QString::number(durationMs / 1000.0, 'f', 3);
  } else {
    args << "-shortest";
  }

  args << outputPath;

  qDebug() << "Starting export with args:" << args;
  m_process->start("ffmpeg", args);
}
