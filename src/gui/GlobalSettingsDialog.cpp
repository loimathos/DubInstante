#include "GlobalSettingsDialog.h"
#include "../core/SettingsManager.h"

#include <QMediaDevices>
#include <QAudioDevice>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QGroupBox>
#include <QLabel>
#include <QMessageBox>
#include <QFrame>

GlobalSettingsDialog::GlobalSettingsDialog(QWidget *parent)
    : QDialog(parent) {
    setupUi();
    populateAudioDevices();
    loadSettings();
}

void GlobalSettingsDialog::setupUi() {
    setObjectName("globalSettingsDialog");
    setWindowTitle(tr("Paramètres Globaux"));
    setMinimumSize(660, 520);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(20, 20, 20, 20);
    mainLayout->setSpacing(14);

    // Title & Subtitle
    QLabel *titleLabel = new QLabel(tr("Paramètres Globaux"), this);
    titleLabel->setObjectName("settingsDialogTitle");
    mainLayout->addWidget(titleLabel);

    QLabel *subtitleLabel = new QLabel(tr("Configurez les préférences de votre studio (thème, sauvegarde automatique, routages audio)."), this);
    subtitleLabel->setObjectName("settingsDialogSubtitle");
    mainLayout->addWidget(subtitleLabel);

    // Sidebar Layout (Sidebar + Content Stack)
    QHBoxLayout *bodyLayout = new QHBoxLayout();
    bodyLayout->setSpacing(18);

    // Sidebar
    QFrame *sidebar = new QFrame(this);
    sidebar->setObjectName("settingsSidebarCard");
    sidebar->setFixedWidth(160);
    QVBoxLayout *sidebarLayout = new QVBoxLayout(sidebar);
    sidebarLayout->setContentsMargins(8, 8, 8, 8);
    sidebarLayout->setSpacing(6);

    m_tabGroup = new QButtonGroup(this);
    m_tabGroup->setExclusive(true);

    QStringList tabLabels = { tr("Général"), tr("Audio & Micros") };
    for (int i = 0; i < tabLabels.size(); ++i) {
        QPushButton *btn = new QPushButton(tabLabels[i], sidebar);
        btn->setCheckable(true);
        btn->setProperty("cssClass", "settingsTabButton");
        btn->setMinimumHeight(34);
        if (i == 0) btn->setChecked(true);
        m_tabGroup->addButton(btn, i);
        m_tabButtons.append(btn);
        sidebarLayout->addWidget(btn);
    }
    sidebarLayout->addStretch();
    bodyLayout->addWidget(sidebar);

    // Content Stack
    m_stackedWidget = new QStackedWidget(this);

    // ==========================================
    // TAB 1: General Settings
    // ==========================================
    QFrame *generalPage = new QFrame(m_stackedWidget);
    generalPage->setObjectName("settingsCard");
    QVBoxLayout *genLayout = new QVBoxLayout(generalPage);
    genLayout->setContentsMargins(18, 18, 18, 18);
    genLayout->setSpacing(14);

    QFormLayout *genForm = new QFormLayout();
    genForm->setVerticalSpacing(12);
    genForm->setHorizontalSpacing(10);

    // Theme Selector
    QLabel *themeLabel = new QLabel(tr("Thème visuel"), generalPage);
    themeLabel->setProperty("cssClass", "fineLabel");
    m_themeCombo = new QComboBox(generalPage);
    m_themeCombo->addItem(tr("Automatique (Système)"), "system");
    m_themeCombo->addItem(tr("Mode Clair"), "light");
    m_themeCombo->addItem(tr("Mode Sombre Premium"), "dark");
    genForm->addRow(themeLabel, m_themeCombo);

    // Countdown Selector
    QLabel *countdownLabel = new QLabel(tr("Décompte pré-enregistrement"), generalPage);
    countdownLabel->setProperty("cssClass", "fineLabel");
    m_countdownSpin = new QSpinBox(generalPage);
    m_countdownSpin->setRange(0, 10);
    m_countdownSpin->setSuffix(tr(" secondes"));
    m_countdownSpin->setSpecialValueText(tr("Désactivé (Instantané)"));
    genForm->addRow(countdownLabel, m_countdownSpin);

    genLayout->addLayout(genForm);

    // Auto-save Group
    QGroupBox *autoSaveGroup = new QGroupBox(tr("Sauvegarde Automatique (Cache)"), generalPage);
    QVBoxLayout *autoSaveLayout = new QVBoxLayout(autoSaveGroup);
    autoSaveLayout->setContentsMargins(14, 20, 14, 14);
    autoSaveLayout->setSpacing(10);

    m_autoSaveCheck = new QCheckBox(tr("Activer la sauvegarde automatique en cache"), autoSaveGroup);
    autoSaveLayout->addWidget(m_autoSaveCheck);

    QWidget *intervalWidget = new QWidget(autoSaveGroup);
    QHBoxLayout *intervalLayout = new QHBoxLayout(intervalWidget);
    intervalLayout->setContentsMargins(0, 0, 0, 0);
    intervalLayout->setSpacing(8);
    QLabel *intervalLabel = new QLabel(tr("Fréquence de sauvegarde :"), intervalWidget);
    intervalLabel->setProperty("cssClass", "fineLabel");
    m_autoSaveIntervalCombo = new QComboBox(intervalWidget);
    m_autoSaveIntervalCombo->addItem(tr("Chaque 1 minute"), 1);
    m_autoSaveIntervalCombo->addItem(tr("Chaque 3 minutes"), 3);
    m_autoSaveIntervalCombo->addItem(tr("Chaque 5 minutes"), 5);
    m_autoSaveIntervalCombo->addItem(tr("Chaque 10 minutes"), 10);
    m_autoSaveIntervalCombo->addItem(tr("Chaque 15 minutes"), 15);
    intervalLayout->addWidget(intervalLabel);
    intervalLayout->addWidget(m_autoSaveIntervalCombo);
    intervalLayout->addStretch();
    autoSaveLayout->addWidget(intervalWidget);

    connect(m_autoSaveCheck, &QCheckBox::toggled, intervalWidget, &QWidget::setEnabled);

    genLayout->addWidget(autoSaveGroup);
    genLayout->addStretch();
    m_stackedWidget->addWidget(generalPage);

    // ==========================================
    // TAB 2: Audio Settings
    // ==========================================
    QFrame *audioPage = new QFrame(m_stackedWidget);
    audioPage->setObjectName("settingsCard");
    QVBoxLayout *audLayout = new QVBoxLayout(audioPage);
    audLayout->setContentsMargins(18, 18, 18, 18);
    audLayout->setSpacing(14);

    QFormLayout *audForm = new QFormLayout();
    audForm->setVerticalSpacing(12);
    audForm->setHorizontalSpacing(10);

    // Default Microphone
    QLabel *micLabel = new QLabel(tr("Microphone par défaut"), audioPage);
    micLabel->setProperty("cssClass", "fineLabel");
    m_defaultMicCombo = new QComboBox(audioPage);
    audForm->addRow(micLabel, m_defaultMicCombo);
    audLayout->addLayout(audForm);

    // Preferred Outputs Group
    QGroupBox *outputsGroup = new QGroupBox(tr("Sélection rapide des Sorties (Casques/Moniteurs)"), audioPage);
    QVBoxLayout *outputsLayout = new QVBoxLayout(outputsGroup);
    outputsLayout->setContentsMargins(14, 20, 14, 14);
    outputsLayout->setSpacing(10);

    QHBoxLayout *listActionsLayout = new QHBoxLayout();
    m_outputsList = new QListWidget(outputsGroup);
    m_outputsList->setMinimumHeight(100);
    m_removeOutputBtn = new QPushButton(tr("Retirer (−)"), outputsGroup);
    m_removeOutputBtn->setObjectName("settingsCancelButton");
    m_removeOutputBtn->setMinimumHeight(30);

    listActionsLayout->addWidget(m_outputsList, 1);
    listActionsLayout->addWidget(m_removeOutputBtn, 0, Qt::AlignTop);
    outputsLayout->addLayout(listActionsLayout);

    // Add Output Area
    QFrame *addFrame = new QFrame(outputsGroup);
    addFrame->setObjectName("settingsCardTrackSelector");
    QFormLayout *addForm = new QFormLayout(addFrame);
    addForm->setContentsMargins(10, 8, 10, 8);
    addForm->setSpacing(6);

    m_newOutputNameEdit = new QLineEdit(addFrame);
    m_newOutputNameEdit->setPlaceholderText(tr("ex. Mon Casque Sony, Enceintes Studio..."));
    m_newOutputDeviceCombo = new QComboBox(addFrame);

    m_addOutputBtn = new QPushButton(tr("Ajouter aux favoris (+)"), addFrame);
    m_addOutputBtn->setProperty("cssClass", "presetButton");
    m_addOutputBtn->setMinimumHeight(30);

    addForm->addRow(tr("Nom personnalisé :"), m_newOutputNameEdit);
    addForm->addRow(tr("Périphérique :"), m_newOutputDeviceCombo);
    addForm->addRow(m_addOutputBtn);

    outputsLayout->addWidget(addFrame);
    audLayout->addWidget(outputsGroup);
    audLayout->addStretch();
    m_stackedWidget->addWidget(audioPage);

    bodyLayout->addWidget(m_stackedWidget, 1);
    mainLayout->addLayout(bodyLayout);

    // Bottom Action Buttons
    QHBoxLayout *bottomLayout = new QHBoxLayout();
    bottomLayout->addStretch();

    QPushButton *cancelBtn = new QPushButton(tr("Annuler"), this);
    cancelBtn->setObjectName("settingsCancelButton");
    cancelBtn->setMinimumHeight(34);
    cancelBtn->setMinimumWidth(100);
    connect(cancelBtn, &QPushButton::clicked, this, &QDialog::reject);

    QPushButton *saveBtn = new QPushButton(tr("Enregistrer"), this);
    saveBtn->setObjectName("settingsSaveButton");
    saveBtn->setMinimumHeight(34);
    saveBtn->setMinimumWidth(120);
    connect(saveBtn, &QPushButton::clicked, this, &GlobalSettingsDialog::saveSettings);

    bottomLayout->addWidget(cancelBtn);
    bottomLayout->addWidget(saveBtn);
    mainLayout->addLayout(bottomLayout);

    // Connections
    connect(m_tabGroup, &QButtonGroup::idClicked, this, &GlobalSettingsDialog::onTabChanged);
    connect(m_addOutputBtn, &QPushButton::clicked, this, &GlobalSettingsDialog::addPreferredOutput);
    connect(m_removeOutputBtn, &QPushButton::clicked, this, &GlobalSettingsDialog::removePreferredOutput);
}

void GlobalSettingsDialog::onTabChanged(int index) {
    m_stackedWidget->setCurrentIndex(index);
}

void GlobalSettingsDialog::populateAudioDevices() {
    m_inputDevices = QMediaDevices::audioInputs();
    m_outputDevices = QMediaDevices::audioOutputs();

    m_defaultMicCombo->clear();
    m_defaultMicCombo->addItem(tr("Utiliser le micro système par défaut"), "");
    for (const QAudioDevice &device : m_inputDevices) {
        m_defaultMicCombo->addItem(device.description(), device.description());
    }

    m_newOutputDeviceCombo->clear();
    for (const QAudioDevice &device : m_outputDevices) {
        m_newOutputDeviceCombo->addItem(device.description(), device.description());
    }
}

void GlobalSettingsDialog::loadSettings() {
    SettingsManager &sm = SettingsManager::instance();

    // General tab
    int themeIdx = m_themeCombo->findData(sm.theme());
    if (themeIdx >= 0) m_themeCombo->setCurrentIndex(themeIdx);

    m_countdownSpin->setValue(sm.countdownDuration());

    m_autoSaveCheck->setChecked(sm.autoSaveEnabled());
    int intervalIdx = m_autoSaveIntervalCombo->findData(sm.autoSaveInterval());
    if (intervalIdx >= 0) m_autoSaveIntervalCombo->setCurrentIndex(intervalIdx);

    // Audio tab
    int micIdx = m_defaultMicCombo->findData(sm.defaultMicrophone());
    if (micIdx >= 0) m_defaultMicCombo->setCurrentIndex(micIdx);

    m_outputsList->clear();
    int count = 1;
    for (const QString &out : sm.preferredOutputs()) {
        QStringList parts = out.split("|");
        if (parts.size() >= 2) {
            m_outputsList->addItem(QString("%1 -- %2 (%3)").arg(QString::number(count), parts[0], parts[1]));
            count++;
        }
    }
}

void GlobalSettingsDialog::addPreferredOutput() {
    QString label = m_newOutputNameEdit->text().trimmed();
    QString devDesc = m_newOutputDeviceCombo->currentData().toString();

    if (label.isEmpty()) {
        QMessageBox::warning(this, tr("Champs manquants"), tr("Veuillez donner un nom à cette sortie (ex. Casque Sony)."));
        return;
    }

    // Add to list widget immediately
    int count = m_outputsList->count() + 1;
    m_outputsList->addItem(QString("%1 -- %2 (%3)").arg(QString::number(count), label, devDesc));

    // Clear add fields
    m_newOutputNameEdit->clear();
}

void GlobalSettingsDialog::removePreferredOutput() {
    QListWidgetItem *item = m_outputsList->currentItem();
    if (!item) return;

    delete item;

    // Recalculate numbers
    for (int i = 0; i < m_outputsList->count(); ++i) {
        QListWidgetItem *listItem = m_outputsList->item(i);
        QString currentText = listItem->text();
        // Remove old number prefix e.g. "2 -- "
        int prefixIdx = currentText.indexOf(" -- ");
        if (prefixIdx >= 0) {
            listItem->setText(QString("%1 -- %2").arg(QString::number(i + 1), currentText.mid(prefixIdx + 4)));
        }
    }
}

void GlobalSettingsDialog::saveSettings() {
    SettingsManager &sm = SettingsManager::instance();

    // General settings
    sm.setTheme(m_themeCombo->currentData().toString());
    sm.setCountdownDuration(m_countdownSpin->value());
    sm.setAutoSaveEnabled(m_autoSaveCheck->isChecked());
    sm.setAutoSaveInterval(m_autoSaveIntervalCombo->currentData().toInt());

    // Audio settings
    sm.setDefaultMicrophone(m_defaultMicCombo->currentData().toString());

    // Preferred outputs list
    QStringList outputs;
    for (int i = 0; i < m_outputsList->count(); ++i) {
        QString text = m_outputsList->item(i)->text();
        // parse e.g. "1 -- Casque Sony (Sony WH-1000XM4)"
        int prefixIdx = text.indexOf(" -- ");
        if (prefixIdx >= 0) {
            QString content = text.mid(prefixIdx + 4);
            int parenIdx = content.lastIndexOf(" (");
            if (parenIdx >= 0) {
                QString label = content.left(parenIdx);
                QString dev = content.mid(parenIdx + 2);
                dev.chop(1); // remove ending ')'
                outputs.append(QString("%1|%2").arg(label, dev));
            }
        }
    }
    sm.setPreferredOutputs(outputs);

    accept();
}
