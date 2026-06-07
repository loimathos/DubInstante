#include "SettingsManager.h"

SettingsManager& SettingsManager::instance() {
    static SettingsManager inst;
    return inst;
}

SettingsManager::SettingsManager(QObject *parent) : QObject(parent) {}

QString SettingsManager::theme() const {
    QSettings settings;
    return settings.value("theme", "system").toString();
}

void SettingsManager::setTheme(const QString &theme) {
    QSettings settings;
    settings.setValue("theme", theme);
}

bool SettingsManager::autoSaveEnabled() const {
    QSettings settings;
    return settings.value("autosave/enabled", true).toBool();
}

void SettingsManager::setAutoSaveEnabled(bool enabled) {
    QSettings settings;
    settings.setValue("autosave/enabled", enabled);
}

int SettingsManager::autoSaveInterval() const {
    QSettings settings;
    return settings.value("autosave/interval", 5).toInt();
}

void SettingsManager::setAutoSaveInterval(int minutes) {
    QSettings settings;
    settings.setValue("autosave/interval", minutes);
}

int SettingsManager::countdownDuration() const {
    QSettings settings;
    return settings.value("countdown", 3).toInt();
}

void SettingsManager::setCountdownDuration(int seconds) {
    QSettings settings;
    settings.setValue("countdown", seconds);
}

QString SettingsManager::defaultMicrophone() const {
    QSettings settings;
    return settings.value("audio/default_microphone", "").toString();
}

void SettingsManager::setDefaultMicrophone(const QString &micName) {
    QSettings settings;
    settings.setValue("audio/default_microphone", micName);
}

QString SettingsManager::trackMicrophone(int trackIdx) const {
    QSettings settings;
    return settings.value(QString("tracks/mic_%1").arg(trackIdx), "").toString();
}

void SettingsManager::setTrackMicrophone(int trackIdx, const QString &micName) {
    QSettings settings;
    settings.setValue(QString("tracks/mic_%1").arg(trackIdx), micName);
}

QStringList SettingsManager::preferredOutputs() const {
    QSettings settings;
    return settings.value("audio/preferred_outputs").toStringList();
}

void SettingsManager::setPreferredOutputs(const QStringList &outputs) {
    QSettings settings;
    settings.setValue("audio/preferred_outputs", outputs);
}

QString SettingsManager::activeOutputProfile() const {
    QSettings settings;
    return settings.value("audio/active_output_profile", "").toString();
}

void SettingsManager::setActiveOutputProfile(const QString &profileName) {
    QSettings settings;
    settings.setValue("audio/active_output_profile", profileName);
}

bool SettingsManager::expertMode() const {
    QSettings settings;
    return settings.value("expert_mode", false).toBool();
}

void SettingsManager::setExpertMode(bool enabled) {
    QSettings settings;
    settings.setValue("expert_mode", enabled);
}
