#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDebug>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QPushButton>
#include <utility>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , audioInitialized(false)
    , isVoiceLoaded(false)
    , isVoicePlaying(false)
{
    ui->setupUi(this);

    diagnoseResources();
    initializeAudio();

    // The backgrounds are represented by the pages in the QStackedWidget.
    ui->ambienceComboBox->addItem("Quiet Cafe");
    ui->ambienceComboBox->addItem("Busy Sidewalk");
    ui->ambienceComboBox->addItem("Concert");

    // Dynamically populate the voice dropdown from the resource files
    populateVoiceComboBox();

    // Connect signals to slots
    connect(ui->ambienceComboBox, &QComboBox::currentIndexChanged, this, &MainWindow::onAmbienceChanged);
    connect(ui->voiceComboBox, &QComboBox::currentIndexChanged, this, &MainWindow::onVoiceSelectionChanged);
    connect(ui->playVoiceButton, &QPushButton::clicked, this, &MainWindow::onPlayVoiceClicked);
}

MainWindow::~MainWindow()
{
    // Clean up the sound and decoder resources when the window is destroyed
    if (isVoiceLoaded)
    {
        ma_sound_uninit(&voiceSound);
        ma_decoder_uninit(&voiceDecoder);
    }
    shutdownAudio();
    delete ui;
}

void MainWindow::onAmbienceChanged(int index)
{
    // Stacked widget shows the corresponding page.
    ui->mixerStackedWidget->setCurrentIndex(index);
}

void MainWindow::onVoiceSelectionChanged(int index)
{
    // If a sound is already loaded when the user changes the selection, stop and unload it.
    if (isVoiceLoaded)
    {
        qDebug() << "Selection changed. Unloading old sound.";
        ma_sound_uninit(&voiceSound);
        ma_decoder_uninit(&voiceDecoder);

        // Reset all our state variables and UI
        isVoiceLoaded = false;
        isVoicePlaying = false;
        currentVoiceName = "";
        ui->voiceStatusLabel->setText("Status: Ready");
        ui->playVoiceButton->setText("Play Voice");
    }
}

void MainWindow::onPlayVoiceClicked()
{
    // Check if the audio engine is running.
    if (!audioInitialized)
    {
        qDebug() << "Audio engine not ready. Cannot play sound.";
        ui->voiceStatusLabel->setText("Status: Error");
        return;
    }

    // --- CASE 1: A sound is loaded and currently playing ---
    if (isVoiceLoaded && isVoicePlaying)
    {
        ma_sound_stop(&voiceSound);
        isVoicePlaying = false;
        ui->voiceStatusLabel->setText("Status: Stopped");
        ui->playVoiceButton->setText("Play Voice");
        qDebug() << "Voice playback stopped.";
        return;
    }

    // --- CASE 2: A sound is loaded but stopped/paused ---
    if (isVoiceLoaded && !isVoicePlaying)
    {
        ma_sound_seek_to_pcm_frame(&voiceSound, 0); // Rewind to start
        ma_sound_start(&voiceSound);
        isVoicePlaying = true;
        ui->voiceStatusLabel->setText("Status: Playing");
        ui->playVoiceButton->setText("Stop Voice");
        qDebug() << "Voice playback started.";
        return;
    }

    // --- CASE 3: No sound is loaded for this selection yet ---
    if (!isVoiceLoaded)
    {
        QString selectedVoice = ui->voiceComboBox->currentText();
        if (selectedVoice.isEmpty()) {
            qDebug() << "No voice selected to play.";
            return;
        }

        // Look up the full resource path in our map
        QString soundFilePath = voicePathMap.value(selectedVoice);

        QFile audioFile(soundFilePath);
        if (!audioFile.open(QIODevice::ReadOnly))
        {
            qDebug() << "Qt failed to open the resource file:" << soundFilePath;
            ui->voiceStatusLabel->setText("Status: Load Error");
            return;
        }

        voiceData = audioFile.readAll();
        audioFile.close();

        ma_decoder_config decoderConfig = ma_decoder_config_init_default();
        ma_result result = ma_decoder_init_memory(voiceData.data(), voiceData.size(), &decoderConfig, &voiceDecoder);
        if (result != MA_SUCCESS)
        {
            qDebug() << "MiniAudio failed to initialize decoder. Error:" << ma_result_description(result);
            ui->voiceStatusLabel->setText("Status: Decode Error");
            return;
        }

        result = ma_sound_init_from_data_source(&engine, &voiceDecoder, MA_SOUND_FLAG_DECODE, NULL, &voiceSound);
        if (result != MA_SUCCESS)
        {
            qDebug() << "MiniAudio failed to init sound. Error:" << ma_result_description(result);
            ui->voiceStatusLabel->setText("Status: Init Error");
            ma_decoder_uninit(&voiceDecoder);
            return;
        }

        qDebug() << "Voice file loaded:" << selectedVoice;
        isVoiceLoaded = true;
        currentVoiceName = selectedVoice; // Remember which voice we loaded

        ma_sound_start(&voiceSound);
        isVoicePlaying = true;
        ui->voiceStatusLabel->setText("Status: Playing");
        ui->playVoiceButton->setText("Stop Voice");
        qDebug() << "Playback started.";
    }
}

void MainWindow::populateVoiceComboBox()
{
    voicePathMap.clear();
    ui->voiceComboBox->clear();

    QDir voiceDir(":/sounds/voices");
    QStringList fileFilters;
    fileFilters << "*.mp3" << "*.flac" << "*.wav";

    QStringList voiceFiles = voiceDir.entryList(fileFilters, QDir::Files);

    for (const QString& filename : std::as_const(voiceFiles))
    {
        QString prettyName = QFileInfo(filename).baseName();
        prettyName.replace('_', ' ');
        QString fullPath = voiceDir.filePath(filename);
        voicePathMap[prettyName] = fullPath;
    }

    ui->voiceComboBox->addItems(voicePathMap.keys());
}

void MainWindow::initializeAudio()
{
    ma_result result = ma_engine_init(NULL, &engine);

    if (result != MA_SUCCESS)
    {
        qDebug() << "Failed to initialize MiniAudio engine. Error:" << ma_result_description(result);
        audioInitialized = false;
        return;
    }

    qDebug() << "MiniAudio engine initialized successfully!";
    audioInitialized = true;
}

void MainWindow::shutdownAudio()
{
    if (audioInitialized)
    {
        ma_engine_uninit(&engine);
        qDebug() << "MiniAudio engine shut down.";
        audioInitialized = false;
    }
}

void MainWindow::diagnoseResources()
{
    qDebug() << "--- Starting Resource Diagnostic ---";

    // Test for a specific file to ensure the resource system is working
    const char* testPath = ":/sounds/voices/apollo11_1.mp3";
    if (QFile::exists(testPath))
    {
        qDebug() << "SUCCESS: Test file exists at" << testPath;
    }
    else
    {
        qDebug() << "FAILURE: Test file was NOT found at" << testPath;
    }

    QDir root (":/");
    if (!root.exists())
    {
        qDebug() << "CRITICAL FAILURE: The root of the resource system ':/' does not exist.";
        return;
    }

    qDebug() << "Listing contents of root ':/' ...";
    QStringList entries = root.entryList();
    if (entries.isEmpty())
    {
        qDebug() << "The resource root is empty.";
    }
    else
    {
        for (const QString& entry : std::as_const(entries))
        {
            qDebug() << "Found entry:" << entry;
        }
    }

    qDebug() << "--- End of Resource Diagnostic ---";
}
