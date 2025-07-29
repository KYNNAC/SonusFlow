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

    // The ambiences are represented by the pages in the QStackedWidget.
    // Add the names to the dropdown to match the page order.
    ui->ambienceComboBox->addItem("Quiet Cafe");
    ui->ambienceComboBox->addItem("Busy Sidewalk");
    ui->ambienceComboBox->addItem("Concert");

    // Connect the signal to slot.
    connect(ui->ambienceComboBox, &QComboBox::currentIndexChanged, this, &MainWindow::onAmbienceChanged);
}

MainWindow::~MainWindow()
{
    shutdownAudio();
    delete ui;
}

void MainWindow::onAmbienceChanged(int index)
{
    // Simply tell the stacked widget to show the corresponding page.
    ui->mixerStackedWidget->setCurrentIndex(index);
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
