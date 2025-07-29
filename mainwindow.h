#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QMainWindow>
#include <QMap>
#include <QStringList>
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

private:
    Ui::MainWindow *ui;

    // --- MiniAudio Members ---
    ma_engine engine;
    bool audioInitialized;

    // --- SonusFlow Data ---
    QMap<QString, QStringList> ambienceLayers;

    // --- Private Methods ---
    void initializeAudio();
    void shutdownAudio();
    void setupMixerFor(const QString &ambienceName);
};

#endif
