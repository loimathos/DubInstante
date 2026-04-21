#include "TrackWidget.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGraphicsDropShadowEffect>

TrackWidget::TrackWidget(int trackIndex, const QString& title, const QString& badgeColor, QWidget *parent)
    : QFrame(parent), m_trackIndex(trackIndex)
{
    setObjectName(QString("track_%1").arg(trackIndex));
    setProperty("cssClass", "track");
    setAttribute(Qt::WA_StyledBackground, true);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    setMinimumWidth(160);

    auto *shadow = new QGraphicsDropShadowEffect(this);
    shadow->setBlurRadius(16);
    shadow->setOffset(0, 4);
    shadow->setColor(QColor(17, 24, 39, 18));
    setGraphicsEffect(shadow);

    setupUi(title, badgeColor);
    setupConnections();
}

void TrackWidget::setupUi(const QString& title, const QString& badgeColor)
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(16, 16, 16, 16);
    mainLayout->setSpacing(12);

    // --- Header ---
    QHBoxLayout *headerLayout = new QHBoxLayout();
    headerLayout->setContentsMargins(0, 0, 0, 6);
    
    m_titleLabel = new QLabel(title, this);
    m_titleLabel->setProperty("cssClass", "track-header-title");

    m_optionsButton = new QPushButton("⚙️", this);
    m_optionsButton->setFixedSize(28, 28);
    m_optionsButton->setFlat(true);
    m_optionsButton->setProperty("cssClass", "track-header-btn");
    
    headerLayout->addWidget(m_titleLabel);
    headerLayout->addStretch();
    headerLayout->addWidget(m_optionsButton);

    QFrame *headerLine = new QFrame(this);
    headerLine->setFrameShape(QFrame::NoFrame);
    headerLine->setFixedHeight(1);
    headerLine->setProperty("cssClass", "track-header-line");

    mainLayout->addLayout(headerLayout);
    mainLayout->addWidget(headerLine);

    // --- IN Control ---
    QHBoxLayout *inLayout = new QHBoxLayout();
    inLayout->setSpacing(10);
    
    QLabel *inLabel = new QLabel("IN:", this);
    inLabel->setProperty("cssClass", "track-control-label");
    inLabel->setFixedWidth(30);

    m_inputCombo = new QComboBox(this);
    m_inputCombo->setProperty("cssClass", "track-control-select");
    m_inputCombo->addItem("Aucune entrée");
    m_inputCombo->addItem("Microphone");
    m_inputCombo->addItem("Line In");

    inLayout->addWidget(inLabel);
    inLayout->addWidget(m_inputCombo, 1);
    
    mainLayout->addLayout(inLayout);

    // --- VOL Control ---
    QHBoxLayout *volLayout = new QHBoxLayout();
    volLayout->setSpacing(10);

    QLabel *volLabel = new QLabel("VOL:", this);
    volLabel->setProperty("cssClass", "track-control-label");
    volLabel->setFixedWidth(30);

    m_volumeSlider = new QSlider(Qt::Horizontal, this);
    m_volumeSlider->setRange(0, 100);
    m_volumeSlider->setValue(80);
    m_volumeSlider->setProperty("cssClass", "track-control-slider");

    volLayout->addWidget(volLabel);
    volLayout->addWidget(m_volumeSlider, 1);

    mainLayout->addLayout(volLayout);

    // --- Spacer to push VU and Badge to bottom ---
    mainLayout->addStretch();

    // --- VU Meter ---
    QFrame *vuContainer = new QFrame(this);
    vuContainer->setProperty("cssClass", "track-vu");
    vuContainer->setMinimumHeight(18);
    QVBoxLayout *vuLayout = new QVBoxLayout(vuContainer);
    vuLayout->setContentsMargins(6, 6, 6, 6);
    vuLayout->setSpacing(0);

    m_vuMeterBg = new QFrame(vuContainer);
    m_vuMeterBg->setProperty("cssClass", "vu-bg");
    m_vuMeterBg->setFixedHeight(5);
    
    QHBoxLayout *bgLayout = new QHBoxLayout(m_vuMeterBg);
    bgLayout->setContentsMargins(0, 0, 0, 0);
    bgLayout->setSpacing(0);

    m_vuMeterFill = new QFrame(m_vuMeterBg);
    m_vuMeterFill->setProperty("cssClass", "vu-fill");
    
    bgLayout->addWidget(m_vuMeterFill);
    bgLayout->addStretch();
    
    // Default val
    bgLayout->setStretch(0, 0);
    bgLayout->setStretch(1, 100);

    vuLayout->addWidget(m_vuMeterBg);
    mainLayout->addWidget(vuContainer);

    // --- Color Badge ---
    QHBoxLayout *badgeLayout = new QHBoxLayout();
    badgeLayout->setContentsMargins(0, 5, 0, 0);
    badgeLayout->setSpacing(8);

    QFrame *colorBox = new QFrame(this);
    colorBox->setFixedSize(14, 14);
    colorBox->setStyleSheet(QString("background-color: %1; border-radius: 3px;").arg(badgeColor));

    QLabel *badgeText = new QLabel(QString("Texte %1").arg(title.split(" ").last()), this); // Demo text depending on track
    badgeText->setProperty("cssClass", "color-badge-text");

    badgeLayout->addWidget(colorBox);
    badgeLayout->addWidget(badgeText);
    badgeLayout->addStretch();

    mainLayout->addLayout(badgeLayout);
}

void TrackWidget::setupConnections()
{
    connect(m_volumeSlider, &QSlider::valueChanged, this, &TrackWidget::onVolumeSliderChanged);
    connect(m_optionsButton, &QPushButton::clicked, this, &TrackWidget::optionsClicked);
    connect(m_inputCombo, &QComboBox::currentTextChanged, this, &TrackWidget::inputSelected);
}

void TrackWidget::onVolumeSliderChanged(int value)
{
    emit volumeChanged(value);
}

void TrackWidget::setVuLevel(int percentage)
{
    int clamped = qBound(0, percentage, 100);
    QHBoxLayout* bgLayout = qobject_cast<QHBoxLayout*>(m_vuMeterBg->layout());
    if (bgLayout) {
        bgLayout->setStretch(0, clamped);
        bgLayout->setStretch(1, 100 - clamped);
    }
}

void TrackWidget::setInputDevice(const QString& device)
{
    int index = m_inputCombo->findText(device);
    if (index >= 0) {
        m_inputCombo->setCurrentIndex(index);
    }
}

void TrackWidget::setVolume(int volume)
{
    int clamped = qBound(0, volume, 100);
    m_volumeSlider->setValue(clamped);
}
