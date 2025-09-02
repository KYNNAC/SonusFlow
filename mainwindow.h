#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QByteArray>
#include "miniaudio.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onAmbienceChanged(int index);
    void onPlayVoiceClicked();
    void onVoiceSelectionChanged(int index);

private:
    Ui::MainWindow *ui;

    // --- MiniAudio Members ---
    ma_engine engine;
    bool audioInitialized;
    ma_decoder voiceDecoder;
    ma_sound voiceSound;
    QByteArray voiceData;
    bool isVoiceLoaded;
    bool isVoicePlaying;
    QString currentVoiceName;

    // --- Sonus Flow Data ---
    QMap<QString, QString> voicePathMap;

    // --- Private Methods ---
    void initializeAudio();
    void shutdownAudio();
    void diagnoseResources();
    void populateVoiceComboBox();
};

#endif
