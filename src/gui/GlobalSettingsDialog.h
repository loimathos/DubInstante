#ifndef GLOBALSETTINGSDIALOG_H
#define GLOBALSETTINGSDIALOG_H

#include <QDialog>
#include <QButtonGroup>
#include <QPushButton>
#include <QStackedWidget>
#include <QComboBox>
#include <QSpinBox>
#include <QCheckBox>
#include <QListWidget>
#include <QLineEdit>
#include <QList>
#include <QAudioDevice>

/**
 * @class GlobalSettingsDialog
 * @brief Dialogue for persistent application configuration.
 */
class GlobalSettingsDialog : public QDialog {
    Q_OBJECT

public:
    explicit GlobalSettingsDialog(QWidget *parent = nullptr);
    ~GlobalSettingsDialog() override = default;

private slots:
    void onTabChanged(int index);
    void addPreferredOutput();
    void removePreferredOutput();
    void saveSettings();

private:
    void setupUi();
    void loadSettings();
    void populateAudioDevices();

    // Tab control
    QButtonGroup *m_tabGroup;
    QList<QPushButton*> m_tabButtons;
    QStackedWidget *m_stackedWidget;

    // General Controls
    QComboBox *m_themeCombo;
    QSpinBox *m_countdownSpin;
    QCheckBox *m_autoSaveCheck;
    QComboBox *m_autoSaveIntervalCombo;

    // Audio Controls
    QComboBox *m_defaultMicCombo;
    QListWidget *m_outputsList;
    QLineEdit *m_newOutputNameEdit;
    QComboBox *m_newOutputDeviceCombo;
    QPushButton *m_addOutputBtn;
    QPushButton *m_removeOutputBtn;

    // Cached hardware devices
    QList<QAudioDevice> m_inputDevices;
    QList<QAudioDevice> m_outputDevices;
};

#endif // GLOBALSETTINGSDIALOG_H
