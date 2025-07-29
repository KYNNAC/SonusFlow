#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , audioInitialized(false)
{
    ui->setupUi(this);
    initializeAudio();
}

MainWindow::~MainWindow()
{
    shutdownAudio();
    delete ui;
}

void MainWindow::initializeAudio()
{
    ma_result result = ma_engine_init(NULL, &engine);

    if (result != MA_SUCCESS) {
        qDebug() << "Failed to initialize MiniAudio engine. Error:" << ma_result_description(result);
        audioInitialized = false;
        return;
    }

    qDebug() << "MiniAudio engine initialized successfully!";
    audioInitialized = true;
}

void MainWindow::shutdownAudio()
{
    if (audioInitialized) {
        ma_engine_uninit(&engine);
        qDebug() << "MiniAudio engine shut down.";
        audioInitialized = false;
    }
}
