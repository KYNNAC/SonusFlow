#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "miniaudio.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;

    // --- MiniAudio Members ---
    ma_engine engine;               // Main MiniAudio engine instance
    bool audioInitialized;          // Flag to track if MiniAudio is successfully initiated

    // --- Private Methods for Audio ---
    void initializeAudio();         // Function to set up MiniAudio
    void shutdownAudio();           // Function to clean up MiniAudio
};
#endif // MAINWINDOW_H
