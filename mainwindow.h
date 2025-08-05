#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "miniaudio.h" // <-- Keep this for ma_engine

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

private:
    Ui::MainWindow *ui;

    // --- MiniAudio Members ---
    ma_engine engine;
    bool audioInitialized;

    // --- Private Methods ---
    void initializeAudio();
    void shutdownAudio();
};

#endif // MAINWINDOW_H
