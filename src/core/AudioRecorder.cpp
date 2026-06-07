/**
 * @file AudioRecorder.cpp
 * @brief Implementation of the AudioRecorder class.
 */

#include "AudioRecorder.h"
#include <QtEndian>
#include <cmath>

AudioRecorder::AudioRecorder(QObject *parent)
    : QObject(parent)
    , m_audioInput(new QAudioInput(this))
    , m_recorder(new QMediaRecorder(this))
    , m_audioSource(nullptr)
    , m_audioIODevice(nullptr)
    , m_isMonitoring(false)
{
    // Configure the capture session
    m_captureSession.setAudioInput(m_audioInput);
    m_captureSession.setRecorder(m_recorder);

    // Forward recorder signals
    connect(m_recorder, &QMediaRecorder::durationChanged,
            this, &AudioRecorder::durationChanged);
    connect(m_recorder, &QMediaRecorder::recorderStateChanged,
            this, &AudioRecorder::recorderStateChanged);
    
    // Convert recorder error to string signal
    connect(m_recorder, &QMediaRecorder::errorOccurred,
            this, [this](QMediaRecorder::Error error, const QString &errorString) {
                Q_UNUSED(error)
                emit errorOccurred(errorString);
            });
}

// =============================================================================
// Device Management
// =============================================================================

QList<QAudioDevice> AudioRecorder::availableDevices() const
{
    return QMediaDevices::audioInputs();
}

void AudioRecorder::setDevice(const QAudioDevice &device)
{
    m_audioInput->setDevice(device);
    
    // Restart monitoring with the new device if we were monitoring
    if (m_isMonitoring) {
        stopMonitoring();
        startMonitoring();
    }
}

void AudioRecorder::setVolume(float volume)
{
    m_audioInput->setVolume(volume);
}

// =============================================================================
// Level Monitoring
// =============================================================================

void AudioRecorder::startMonitoring()
{
    if (m_isMonitoring) {
        return;
    }

    QAudioDevice device = m_audioInput->device();
    if (device.isNull()) {
        // Use default device if none set
        device = QMediaDevices::defaultAudioInput();
        if (device.isNull()) {
            return;
        }
    }

    // Configure format: 16-bit signed PCM, mono, 16kHz (low overhead)
    QAudioFormat format;
    format.setSampleRate(16000);
    format.setChannelCount(1);
    format.setSampleFormat(QAudioFormat::Int16);

    if (!device.isFormatSupported(format)) {
        // Fallback to preferred format
        format = device.preferredFormat();
    }

    m_audioSource = new QAudioSource(device, format, this);
    m_audioSource->setVolume(m_audioInput->volume());

    m_audioIODevice = m_audioSource->start();
    if (m_audioIODevice) {
        connect(m_audioIODevice, &QIODevice::readyRead,
                this, &AudioRecorder::processAudioBuffer);
        m_isMonitoring = true;
    }
}

void AudioRecorder::stopMonitoring()
{
    if (!m_isMonitoring) {
        return;
    }

    if (m_audioSource) {
        m_audioSource->stop();
        m_audioSource->deleteLater();
        m_audioSource = nullptr;
    }
    m_audioIODevice = nullptr;
    m_isMonitoring = false;

    emit levelChanged(0.0f);
}

void AudioRecorder::processAudioBuffer()
{
    if (!m_audioIODevice) {
        return;
    }

    QByteArray data = m_audioIODevice->readAll();
    if (data.isEmpty()) {
        return;
    }

    // Determine sample format from the audio source
    QAudioFormat format = m_audioSource->format();

    float maxAmplitude = 0.0f;

    if (format.sampleFormat() == QAudioFormat::Int16) {
        const qint16 *samples = reinterpret_cast<const qint16 *>(data.constData());
        int sampleCount = data.size() / sizeof(qint16);
        
        for (int i = 0; i < sampleCount; ++i) {
            float absValue = qAbs(static_cast<float>(samples[i])) / 32768.0f;
            if (absValue > maxAmplitude) {
                maxAmplitude = absValue;
            }
        }
    } else if (format.sampleFormat() == QAudioFormat::Float) {
        const float *samples = reinterpret_cast<const float *>(data.constData());
        int sampleCount = data.size() / sizeof(float);
        
        for (int i = 0; i < sampleCount; ++i) {
            float absValue = qAbs(samples[i]);
            if (absValue > maxAmplitude) {
                maxAmplitude = absValue;
            }
        }
    } else {
        return; // Unsupported format
    }

    // Clamp and emit
    float level = qBound(0.0f, maxAmplitude, 1.0f);
    emit levelChanged(level);
}

// =============================================================================
// Recording Control
// =============================================================================

void AudioRecorder::startRecording(const QUrl &outputUrl)
{
    m_recorder->setOutputLocation(outputUrl);
    m_recorder->record();
}

void AudioRecorder::stopRecording()
{
    m_recorder->stop();
}

QMediaRecorder::RecorderState AudioRecorder::recorderState() const
{
    return m_recorder->recorderState();
}
