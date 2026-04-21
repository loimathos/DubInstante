#ifndef TRACKWIDGET_H
#define TRACKWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QComboBox>
#include <QSlider>
#include <QFrame>

class TrackWidget : public QFrame {
    Q_OBJECT
    
public:
    explicit TrackWidget(int trackIndex, const QString& title, const QString& badgeColor, QWidget *parent = nullptr);
    ~TrackWidget() override = default;

    int trackIndex() const { return m_trackIndex; }
    void setVuLevel(int percentage); // 0 to 100
    
    QString currentInputDevice() const { return m_inputCombo->currentText(); }
    int currentVolume() const { return m_volumeSlider->value(); }
    void setInputDevice(const QString& device);
    void setVolume(int volume);

signals:
    void optionsClicked();
    void inputSelected(const QString& inputName);
    void volumeChanged(int volume);

private slots:
    void onVolumeSliderChanged(int value);

private:
    void setupUi(const QString& title, const QString& badgeColor);
    void setupConnections();

    int m_trackIndex;

    // UI Components
    QLabel *m_titleLabel;
    QPushButton *m_optionsButton;
    QComboBox *m_inputCombo;
    QSlider *m_volumeSlider;
    
    // Custom VU Meter simply rendered with layouts and frames
    QFrame *m_vuMeterBg;
    QFrame *m_vuMeterFill;
};

#endif // TRACKWIDGET_H
