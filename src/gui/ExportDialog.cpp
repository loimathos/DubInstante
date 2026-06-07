#include "ExportDialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QGroupBox>
#include <QFileDialog>
#include <QFileInfo>
#include <QDir>
#include <QScrollArea>
#include <QIcon>
#include <QStyle>

ExportDialog::ExportDialog(const QString &sourceVideo,
                          const QString &primaryAudio,
                          const QStringList &extraAudios,
                          qint64 lastRecordedDurationMs,
                          qint64 recordingStartTimeMs,
                          float currentOriginalVolume,
                          const QVector<float> &currentTrackVolumes,
                          const QVector<bool> &currentTrackMutes,
                          QWidget *parent)
    : QDialog(parent)
    , m_sourceVideoPath(sourceVideo)
    , m_primaryAudioPath(primaryAudio)
    , m_extraAudioPaths(extraAudios)
    , m_lastRecordedDurationMs(lastRecordedDurationMs)
    , m_recordingStartTimeMs(recordingStartTimeMs)
    , m_defaultOriginalVolume(currentOriginalVolume)
    , m_defaultTrackVolumes(currentTrackVolumes)
    , m_defaultTrackMutes(currentTrackMutes)
{
    setupUi();
    populateFields();
    validateSettings();
}

void ExportDialog::setupUi()
{
    setObjectName("exportSettingsDialog");
    setWindowTitle(tr("Paramètres d'Exportation"));
    setMinimumSize(620, 680);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(20, 20, 20, 20);
    mainLayout->setSpacing(14);

    // 1. Header (Title & Subtitle)
    QLabel *titleLabel = new QLabel(tr("Exporter le Projet"), this);
    titleLabel->setObjectName("settingsDialogTitle");
    mainLayout->addWidget(titleLabel);

    QLabel *subtitleLabel = new QLabel(tr("Configurez les options d'export et le mixage final pour votre doublage."), this);
    subtitleLabel->setObjectName("settingsDialogSubtitle");
    mainLayout->addWidget(subtitleLabel);

    // 2. Card: File Destination and Format
    QFrame *destCard = new QFrame(this);
    destCard->setObjectName("settingsCard");
    QVBoxLayout *destLayout = new QVBoxLayout(destCard);
    destLayout->setContentsMargins(14, 14, 14, 14);
    destLayout->setSpacing(10);

    QLabel *destTitle = new QLabel(tr("Format & Destination"), destCard);
    destTitle->setProperty("cssClass", "fineLabel");
    destLayout->addWidget(destTitle);

    QHBoxLayout *destInputsLayout = new QHBoxLayout();
    destInputsLayout->setSpacing(8);

    m_formatCombo = new QComboBox(destCard);
    m_formatCombo->addItem("MP4", "mp4");
    m_formatCombo->addItem("MKV", "mkv");
    m_formatCombo->addItem("MOV", "mov");
    m_formatCombo->addItem("AVI", "avi");
    m_formatCombo->setFixedWidth(90);
    destInputsLayout->addWidget(m_formatCombo);

    m_outputPathEdit = new QLineEdit(destCard);
    m_outputPathEdit->setPlaceholderText(tr("Chemin du fichier de sortie..."));
    destInputsLayout->addWidget(m_outputPathEdit);

    m_browseButton = new QPushButton(destCard);
    m_browseButton->setIcon(QIcon(":/resources/icons/folder_open.svg"));
    m_browseButton->setToolTip(tr("Parcourir..."));
    m_browseButton->setFixedSize(32, 32);
    m_browseButton->setProperty("cssClass", "iconButton");
    destInputsLayout->addWidget(m_browseButton);

    destLayout->addLayout(destInputsLayout);
    mainLayout->addWidget(destCard);

    // 3. Card: Format Presets & Timeline Range
    QFrame *presetsCard = new QFrame(this);
    presetsCard->setObjectName("settingsCard");
    QFormLayout *presetsLayout = new QFormLayout(presetsCard);
    presetsLayout->setContentsMargins(14, 14, 14, 14);
    presetsLayout->setVerticalSpacing(12);
    presetsLayout->setHorizontalSpacing(14);

    QLabel *resLabel = new QLabel(tr("Résolution vidéo"), presetsCard);
    resLabel->setProperty("cssClass", "fineLabel");
    m_resolutionCombo = new QComboBox(presetsCard);
    m_resolutionCombo->addItem(tr("Originale (Pas d'échelle)"), "");
    m_resolutionCombo->addItem(tr("Full HD (1080p)"), "1920:-2");
    m_resolutionCombo->addItem(tr("HD (720p)"), "1280:-2");
    presetsLayout->addRow(resLabel, m_resolutionCombo);

    QLabel *vQualLabel = new QLabel(tr("Qualité vidéo (Preset)"), presetsCard);
    vQualLabel->setProperty("cssClass", "fineLabel");
    m_videoQualityCombo = new QComboBox(presetsCard);
    m_videoQualityCombo->addItem(tr("Rapide (Qualité standard - CRF 26)"), "superfast");
    m_videoQualityCombo->addItem(tr("Standard (Recommandé - CRF 21)"), "medium");
    m_videoQualityCombo->addItem(tr("Haute (Traitement lent - CRF 16)"), "slow");
    m_videoQualityCombo->setCurrentIndex(1); // Standard default
    presetsLayout->addRow(vQualLabel, m_videoQualityCombo);

    QLabel *aQualLabel = new QLabel(tr("Qualité audio"), presetsCard);
    aQualLabel->setProperty("cssClass", "fineLabel");
    m_audioQualityCombo = new QComboBox(presetsCard);
    m_audioQualityCombo->addItem(tr("Économique (128 kbps)"), 128);
    m_audioQualityCombo->addItem(tr("Standard (192 kbps)"), 192);
    m_audioQualityCombo->addItem(tr("Supérieure (256 kbps)"), 256);
    m_audioQualityCombo->addItem(tr("Studio (320 kbps)"), 320);
    m_audioQualityCombo->setCurrentIndex(1); // 192 kbps default
    presetsLayout->addRow(aQualLabel, m_audioQualityCombo);

    QLabel *rangeLabel = new QLabel(tr("Plage d'export"), presetsCard);
    rangeLabel->setProperty("cssClass", "fineLabel");
    m_rangeCombo = new QComboBox(presetsCard);
    m_rangeCombo->addItem(tr("Tout le projet (Vidéo complète)"), "all");
    if (m_lastRecordedDurationMs > 0) {
        m_rangeCombo->addItem(tr("Dernier enregistrement uniquement"), "last");
        m_rangeCombo->setCurrentIndex(1);
    } else {
        m_rangeCombo->setCurrentIndex(0);
    }
    presetsLayout->addRow(rangeLabel, m_rangeCombo);

    mainLayout->addWidget(presetsCard);

    // 4. Advanced Override Section
    m_advancedCheck = new QCheckBox(tr("Activer les réglages de qualité avancés"), this);
    m_advancedCheck->setObjectName("advancedCheck");
    mainLayout->addWidget(m_advancedCheck);

    m_advancedContainer = new QWidget(this);
    m_advancedContainer->setVisible(false);
    QFormLayout *advLayout = new QFormLayout(m_advancedContainer);
    advLayout->setContentsMargins(14, 4, 14, 4);
    advLayout->setVerticalSpacing(10);
    advLayout->setHorizontalSpacing(14);

    QLabel *customCrfLabel = new QLabel(tr("Valeur CRF personnalisée (Vidéo)"), m_advancedContainer);
    customCrfLabel->setProperty("cssClass", "fineLabel");
    m_customCrfSpin = new QSpinBox(m_advancedContainer);
    m_customCrfSpin->setRange(0, 51);
    m_customCrfSpin->setValue(21);
    m_customCrfSpin->setToolTip(tr("CRF: 0 (sans perte) à 51 (qualité horrible). Recommandé: 15-25"));
    advLayout->addRow(customCrfLabel, m_customCrfSpin);

    QLabel *customArLabel = new QLabel(tr("Débit audio personnalisé (kbps)"), m_advancedContainer);
    customArLabel->setProperty("cssClass", "fineLabel");
    m_customBitrateSpin = new QSpinBox(m_advancedContainer);
    m_customBitrateSpin->setRange(32, 512);
    m_customBitrateSpin->setSuffix(" kbps");
    m_customBitrateSpin->setValue(192);
    advLayout->addRow(customArLabel, m_customBitrateSpin);

    m_warningLabel = new QLabel(m_advancedContainer);
    m_warningLabel->setObjectName("warningLabel");
    m_warningLabel->setWordWrap(true);
    m_warningLabel->setStyleSheet("color: #f3a400; font-size: 11px; font-weight: 600; padding: 4px;");
    advLayout->addRow(m_warningLabel);

    mainLayout->addWidget(m_advancedContainer);

    // 5. Card: Audio Mixing Panel (Scrollable if many tracks)
    QGroupBox *mixGroup = new QGroupBox(tr("Mélange des Volumes"), this);
    QVBoxLayout *mixGroupLayout = new QVBoxLayout(mixGroup);
    mixGroupLayout->setContentsMargins(10, 18, 10, 10);

    QScrollArea *scrollArea = new QScrollArea(mixGroup);
    scrollArea->setWidgetResizable(true);
    scrollArea->setFrameStyle(QFrame::NoFrame);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    QWidget *scrollWidget = new QWidget(scrollArea);
    m_audioTracksLayout = new QVBoxLayout(scrollWidget);
    m_audioTracksLayout->setContentsMargins(4, 4, 4, 4);
    m_audioTracksLayout->setSpacing(10);

    // Original Video row
    QWidget *origRow = new QWidget(scrollWidget);
    QHBoxLayout *origRowLayout = new QHBoxLayout(origRow);
    origRowLayout->setContentsMargins(0, 0, 0, 0);
    origRowLayout->setSpacing(8);

    QLabel *origTitle = new QLabel(tr("Vidéo originale :"), origRow);
    origTitle->setFixedWidth(140);
    origTitle->setProperty("cssClass", "fineLabel");
    origRowLayout->addWidget(origTitle);

    m_originalVolumeSlider = new QSlider(Qt::Horizontal, origRow);
    m_originalVolumeSlider->setRange(0, 200);
    m_originalVolumeSlider->setValue(qBound(0, static_cast<int>(m_defaultOriginalVolume * 100), 200));
    origRowLayout->addWidget(m_originalVolumeSlider);

    m_originalVolPercentLabel = new QLabel(QString("%1%").arg(m_originalVolumeSlider->value()), origRow);
    m_originalVolPercentLabel->setFixedWidth(38);
    m_originalVolPercentLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    origRowLayout->addWidget(m_originalVolPercentLabel);

    m_originalMuteBtn = new QPushButton(origRow);
    m_originalMuteBtn->setIcon(QIcon(":/resources/icons/volume.svg"));
    m_originalMuteBtn->setCheckable(true);
    m_originalMuteBtn->setFixedSize(28, 28);
    m_originalMuteBtn->setChecked(m_defaultOriginalVolume < 0.01f);
    m_originalMuteBtn->setProperty("cssClass", "iconButton");
    origRowLayout->addWidget(m_originalMuteBtn);

    m_audioTracksLayout->addWidget(origRow);

    // Mic tracks
    int trackIndex = 1;
    // Add primary mic track if we have a valid path
    if (!m_primaryAudioPath.isEmpty()) {
        addTrackRow(scrollWidget, tr("Piste Micro 1 (Principal)"), 0);
        trackIndex++;
    }

    // Add extra mic tracks
    for (int i = 0; i < m_extraAudioPaths.size(); ++i) {
        if (!m_extraAudioPaths[i].isEmpty()) {
            addTrackRow(scrollWidget, tr("Piste Micro %1").arg(trackIndex), i + 1);
            trackIndex++;
        }
    }

    m_audioTracksLayout->addStretch();
    scrollWidget->setLayout(m_audioTracksLayout);
    scrollArea->setWidget(scrollWidget);
    mixGroupLayout->addWidget(scrollArea);
    
    // Set mixer height boundary
    mixGroup->setMinimumHeight(150);
    mainLayout->addWidget(mixGroup);

    // 6. Footer Actions
    QHBoxLayout *actionLayout = new QHBoxLayout();
    actionLayout->addStretch();

    m_btnCancel = new QPushButton(tr("Annuler"), this);
    m_btnCancel->setMinimumSize(90, 32);
    m_btnCancel->setProperty("cssClass", "presetButton");
    actionLayout->addWidget(m_btnCancel);

    m_btnExport = new QPushButton(tr("Exporter"), this);
    m_btnExport->setMinimumSize(100, 32);
    m_btnExport->setDefault(true);
    m_btnExport->setStyleSheet("background-color: #926bff; color: white; font-weight: 700; border-radius: 8px; border: none; padding: 4px 16px;");
    actionLayout->addWidget(m_btnExport);

    mainLayout->addLayout(actionLayout);

    // Connections
    connect(m_formatCombo, &QComboBox::currentIndexChanged, this, &ExportDialog::onFormatChanged);
    connect(m_browseButton, &QPushButton::clicked, this, &ExportDialog::onBrowseClicked);
    connect(m_advancedCheck, &QCheckBox::toggled, this, &ExportDialog::onAdvancedToggled);
    connect(m_customCrfSpin, &QSpinBox::valueChanged, this, &ExportDialog::validateSettings);
    connect(m_customBitrateSpin, &QSpinBox::valueChanged, this, &ExportDialog::validateSettings);
    connect(m_videoQualityCombo, &QComboBox::currentIndexChanged, this, &ExportDialog::validateSettings);
    connect(m_audioQualityCombo, &QComboBox::currentIndexChanged, this, &ExportDialog::validateSettings);

    connect(m_originalVolumeSlider, &QSlider::valueChanged, this, &ExportDialog::onVolumeSliderChanged);
    connect(m_originalMuteBtn, &QPushButton::toggled, this, &ExportDialog::onMuteToggled);

    connect(m_btnCancel, &QPushButton::clicked, this, &QDialog::reject);
    connect(m_btnExport, &QPushButton::clicked, this, &QDialog::accept);
}

void ExportDialog::addTrackRow(QWidget *parent, const QString &title, int index)
{
    QWidget *row = new QWidget(parent);
    QHBoxLayout *rowLayout = new QHBoxLayout(row);
    rowLayout->setContentsMargins(0, 0, 0, 0);
    rowLayout->setSpacing(8);

    QLabel *lbl = new QLabel(title + " :", row);
    lbl->setFixedWidth(140);
    lbl->setProperty("cssClass", "fineLabel");
    rowLayout->addWidget(lbl);

    float defaultVolume = 1.0f;
    if (m_defaultTrackVolumes.size() > index) {
        defaultVolume = m_defaultTrackVolumes[index];
    }
    bool defaultMute = false;
    if (m_defaultTrackMutes.size() > index) {
        defaultMute = m_defaultTrackMutes[index];
    }

    QSlider *slider = new QSlider(Qt::Horizontal, row);
    slider->setRange(0, 200);
    slider->setValue(qBound(0, static_cast<int>(defaultVolume * 100), 200));
    slider->setProperty("trackIndex", index);
    rowLayout->addWidget(slider);

    QLabel *pct = new QLabel(QString("%1%").arg(slider->value()), row);
    pct->setFixedWidth(38);
    pct->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    rowLayout->addWidget(pct);

    QPushButton *mute = new QPushButton(row);
    mute->setIcon(QIcon(":/resources/icons/volume.svg"));
    mute->setCheckable(true);
    mute->setFixedSize(28, 28);
    mute->setChecked(defaultMute);
    mute->setProperty("trackIndex", index);
    mute->setProperty("cssClass", "iconButton");
    rowLayout->addWidget(mute);

    m_trackSliders.append(slider);
    m_trackPercentLabels.append(pct);
    m_trackMuteBtns.append(mute);

    m_audioTracksLayout->addWidget(row);

    // Connect slider/mute for this row
    connect(slider, &QSlider::valueChanged, this, &ExportDialog::onVolumeSliderChanged);
    connect(mute, &QPushButton::toggled, this, &ExportDialog::onMuteToggled);
}

void ExportDialog::populateFields()
{
    // Generate default output file path
    if (!m_sourceVideoPath.isEmpty()) {
        QFileInfo videoInfo(m_sourceVideoPath);
        QString defaultDir = videoInfo.absolutePath();
        QString baseName = videoInfo.baseName();
        QString formatExt = m_formatCombo->currentData().toString();
        m_outputPathEdit->setText(QDir(defaultDir).filePath(baseName + "_double." + formatExt));
    } else {
        m_outputPathEdit->setText(QDir::homePath() + "/dub_result.mp4");
    }
}

void ExportDialog::onFormatChanged(int index)
{
    Q_UNUSED(index);
    QString formatExt = m_formatCombo->currentData().toString();
    QString currentPath = m_outputPathEdit->text().trimmed();

    if (!currentPath.isEmpty()) {
        QFileInfo fileInfo(currentPath);
        QString absoluteDir = fileInfo.absolutePath();
        QString baseName = fileInfo.baseName();
        m_outputPathEdit->setText(QDir(absoluteDir).filePath(baseName + "." + formatExt));
    }
}

void ExportDialog::onBrowseClicked()
{
    QString formatExt = m_formatCombo->currentData().toString();
    QString filter = QString("%1 (*.%2)").arg(m_formatCombo->currentText()).arg(formatExt);
    
    QString currentPath = m_outputPathEdit->text().trimmed();
    QString startDir = currentPath.isEmpty() ? QDir::homePath() : QFileInfo(currentPath).absolutePath();

    QString selectedFile = QFileDialog::getSaveFileName(
        this, tr("Choisir le fichier d'export"), startDir, filter);

    if (!selectedFile.isEmpty()) {
        // Ensure proper suffix
        if (!selectedFile.endsWith("." + formatExt, Qt::CaseInsensitive)) {
            selectedFile += "." + formatExt;
        }
        m_outputPathEdit->setText(selectedFile);
    }
}

void ExportDialog::onAdvancedToggled(bool checked)
{
    m_advancedContainer->setVisible(checked);
    // Sync spinners to preset choices if toggle was checked
    if (checked) {
        // Sync CRF
        QString preset = m_videoQualityCombo->currentData().toString();
        if (preset == "superfast") m_customCrfSpin->setValue(26);
        else if (preset == "medium") m_customCrfSpin->setValue(21);
        else if (preset == "slow") m_customCrfSpin->setValue(16);

        // Sync Audio Bitrate
        m_customBitrateSpin->setValue(m_audioQualityCombo->currentData().toInt());
    }
    validateSettings();
}

void ExportDialog::validateSettings()
{
    QString warning = "";

    if (m_advancedCheck->isChecked()) {
        int crf = m_customCrfSpin->value();
        int audioBitrate = m_customBitrateSpin->value();

        if (crf < 10) {
            warning += tr("⚠️ CRF bas (%1) : Le fichier vidéo sera extrêmement volumineux.\n").arg(crf);
        } else if (crf > 35) {
            warning += tr("⚠️ CRF élevé (%1) : La qualité vidéo sera très pixelisée et médiocre.\n").arg(crf);
        }

        if (audioBitrate < 64) {
            warning += tr("⚠️ Débit audio faible (%1 kbps) : La qualité du doublage sera dégradée.\n").arg(audioBitrate);
        } else if (audioBitrate > 320) {
            warning += tr("⚠️ Débit audio superflu (%1 kbps) : Les débits > 320 kbps n'apportent aucun gain de qualité avec l'AAC.\n").arg(audioBitrate);
        }
    }

    updateWarningText(warning);
}

void ExportDialog::updateWarningText(const QString &warning)
{
    m_warningLabel->setText(warning);
    m_warningLabel->setVisible(!warning.isEmpty());
}

void ExportDialog::onVolumeSliderChanged(int value)
{
    QObject *snd = sender();
    if (snd == m_originalVolumeSlider) {
        m_originalVolPercentLabel->setText(QString("%1%").arg(value));
        if (value > 0 && m_originalMuteBtn->isChecked()) {
            m_originalMuteBtn->blockSignals(true);
            m_originalMuteBtn->setChecked(false);
            m_originalMuteBtn->blockSignals(false);
        }
    } else {
        // Find which slider changed
        for (int i = 0; i < m_trackSliders.size(); ++i) {
            if (snd == m_trackSliders[i]) {
                m_trackPercentLabels[i]->setText(QString("%1%").arg(value));
                if (value > 0 && m_trackMuteBtns[i]->isChecked()) {
                    m_trackMuteBtns[i]->blockSignals(true);
                    m_trackMuteBtns[i]->setChecked(false);
                    m_trackMuteBtns[i]->blockSignals(false);
                }
                break;
            }
        }
    }
}

void ExportDialog::onMuteToggled()
{
    QPushButton *btn = qobject_cast<QPushButton *>(sender());
    if (!btn) return;

    bool muted = btn->isChecked();
    if (btn == m_originalMuteBtn) {
        if (muted) {
            m_originalVolumeSlider->setValue(0);
        } else {
            // Restore default or 100%
            m_originalVolumeSlider->setValue(qBound(10, static_cast<int>(m_defaultOriginalVolume * 100), 100));
        }
    } else {
        int idx = m_trackMuteBtns.indexOf(btn);
        if (idx >= 0) {
            if (muted) {
                m_trackSliders[idx]->setValue(0);
            } else {
                float defVol = (m_defaultTrackVolumes.size() > idx) ? m_defaultTrackVolumes[idx] : 1.0f;
                m_trackSliders[idx]->setValue(qBound(10, static_cast<int>(defVol * 100), 100));
            }
        }
    }
}

ExportConfig ExportDialog::exportConfig() const
{
    ExportConfig config;
    config.videoPath = m_sourceVideoPath;
    config.audioPath = m_primaryAudioPath;
    config.extraAudioPaths = m_extraAudioPaths;
    config.outputPath = m_outputPathEdit->text().trimmed();
    config.format = m_formatCombo->currentData().toString();

    // Plage d'export configuration
    QString range = m_rangeCombo->currentData().toString();
    if (range == "last") {
        config.startTimeMs = m_recordingStartTimeMs;
        config.durationMs = m_lastRecordedDurationMs;
    } else {
        config.startTimeMs = 0;
        config.durationMs = -1;
    }

    // Resolution scaling option
    config.scaleResolution = m_resolutionCombo->currentData().toString();

    // Quality parameters (Preset or Advanced Overrides)
    if (m_advancedCheck->isChecked()) {
        config.crf = m_customCrfSpin->value();
        config.audioBitrateKbps = m_customBitrateSpin->value();
        // Determine matching speed preset roughly or default to medium
        config.speedPreset = m_videoQualityCombo->currentData().toString();
    } else {
        config.speedPreset = m_videoQualityCombo->currentData().toString();
        // Map preset to crf values
        if (config.speedPreset == "superfast") {
            config.crf = 26;
        } else if (config.speedPreset == "slow") {
            config.crf = 16;
        } else {
            config.crf = 21; // standard
        }
        config.audioBitrateKbps = m_audioQualityCombo->currentData().toInt();
    }

    // Volumes
    config.originalVolume = m_originalMuteBtn->isChecked() ? 0.0f : (m_originalVolumeSlider->value() / 100.0f);

    // Track Volumes
    for (int i = 0; i < m_trackSliders.size(); ++i) {
        float vol = m_trackMuteBtns[i]->isChecked() ? 0.0f : (m_trackSliders[i]->value() / 100.0f);
        config.trackVolumes.append(vol);
    }

    return config;
}
