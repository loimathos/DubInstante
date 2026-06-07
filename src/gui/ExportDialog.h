#ifndef EXPORTDIALOG_H
#define EXPORTDIALOG_H

#include "../core/ExportService.h"
#include <QDialog>
#include <QLineEdit>
#include <QPushButton>
#include <QComboBox>
#include <QCheckBox>
#include <QSpinBox>
#include <QLabel>
#include <QSlider>
#include <QVector>
#include <QVBoxLayout>

class ExportDialog : public QDialog {
    Q_OBJECT

public:
    explicit ExportDialog(const QString &sourceVideo,
                          const QString &primaryAudio,
                          const QStringList &extraAudios,
                          qint64 lastRecordedDurationMs,
                          qint64 recordingStartTimeMs,
                          float currentOriginalVolume,
                          const QVector<float> &currentTrackVolumes,
                          const QVector<bool> &currentTrackMutes,
                          QWidget *parent = nullptr);
    ~ExportDialog() override = default;

    ExportConfig exportConfig() const;

private slots:
    void onFormatChanged(int index);
    void onBrowseClicked();
    void onAdvancedToggled(bool checked);
    void validateSettings();
    void onVolumeSliderChanged(int value);
    void onMuteToggled();

private:
    void setupUi();
    void populateFields();
    void updateWarningText(const QString &warning);
    void addTrackRow(QWidget *parent, const QString &title, int index);

    // Initial parameters passed from Main Window
    QString m_sourceVideoPath;
    QString m_primaryAudioPath;
    QStringList m_extraAudioPaths;
    qint64 m_lastRecordedDurationMs;
    qint64 m_recordingStartTimeMs;
    
    float m_defaultOriginalVolume;
    QVector<float> m_defaultTrackVolumes;
    QVector<bool> m_defaultTrackMutes;

    // UI Components
    QLineEdit *m_outputPathEdit;
    QPushButton *m_browseButton;
    
    QComboBox *m_formatCombo;
    QComboBox *m_resolutionCombo;
    QComboBox *m_videoQualityCombo;
    QComboBox *m_audioQualityCombo;
    QComboBox *m_rangeCombo;
    
    QCheckBox *m_advancedCheck;
    QWidget *m_advancedContainer;
    QSpinBox *m_customCrfSpin;
    QSpinBox *m_customBitrateSpin;
    QLabel *m_warningLabel;

    // Audio mixing panel
    QVBoxLayout *m_audioTracksLayout;
    
    QSlider *m_originalVolumeSlider;
    QLabel *m_originalVolPercentLabel;
    QPushButton *m_originalMuteBtn;
    
    QVector<QSlider *> m_trackSliders;
    QVector<QLabel *> m_trackPercentLabels;
    QVector<QPushButton *> m_trackMuteBtns;

    QPushButton *m_btnExport;
    QPushButton *m_btnCancel;
};

#endif // EXPORTDIALOG_H
