/* main.cpp
 *
 * Copyright (C) 2021-2022 Siddharudh P T <siddharudh@gmail.com>
 *
 * This file is part of RQPlayer.
 *
 * RQPlayer is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * RQPlayer is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with RQPlayer.  If not, see <https://www.gnu.org/licenses/>.
 */

#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QCommandLineParser>
#include <QFile>

#include "filereaders.h"
#include "orchestrator.h"
#include "framespresenter.h"
#include "audiooutput.h"

struct PlayerOptions {
    QString videoFile;
    QString audioFile;
    QSize frameSize;
    double frameRate;
    int audioChannels;
};

void processCommandLine(PlayerOptions &options);

int main(int argc, char *argv[])
{
    QCoreApplication::setApplicationName("RQPlayer");
    QCoreApplication::setApplicationVersion("1.0");

#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif

    QGuiApplication app{argc, argv};
    QQmlApplicationEngine engine;

    PlayerOptions options;

    processCommandLine(options);

    if (options.videoFile.isEmpty() && options.audioFile.isEmpty()) {
        options.videoFile = "/tmp/vpipe";
        options.audioFile = "/tmp/apipe";
    }

    if (options.frameSize.isEmpty()) {
        options.frameSize = {640, 480};
    }
    if (options.audioChannels < 1 || options.audioChannels > 16) {
        options.audioChannels = 1;
    }

    using namespace RQPlayer;
    qmlRegisterType<FramesPresenter>("RQPlayer", 1, 0, "FramesPresenter");

    const QUrl url{"qrc:/main.qml"};
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url, options](QObject *obj, const QUrl &objUrl) {
        if (url == objUrl) {
            if (obj) {
                obj->setProperty("width", options.frameSize.width());
                obj->setProperty("height", options.frameSize.height());
            }
            else {
                QCoreApplication::exit(-1);
            }
        }
    }, Qt::QueuedConnection);

    engine.load(url);

    QVideoSurfaceFormat videoFormat{
        options.frameSize, QVideoFrame::Format_YUV422P};
    videoFormat.setFrameRate(options.frameRate);

    QAudioFormat audioFormat;
    audioFormat.setByteOrder(QAudioFormat::LittleEndian);
    audioFormat.setChannelCount(options.audioChannels);
    audioFormat.setCodec("audio/pcm");
    audioFormat.setSampleRate(48000);
    audioFormat.setSampleSize(16);
    audioFormat.setSampleType(QAudioFormat::SignedInt);

    auto rootObjects = engine.rootObjects();
    if (rootObjects.isEmpty()) {
        qDebug() << "QQmlApplicationEngine rootObjects is empty";
        return 1;
    }
    QObject *rootObject = rootObjects.first();
    FramesPresenter *presenter = rootObject->findChild<FramesPresenter *>();
    if (!presenter) {
        qDebug() << "Couldn't find FramesPresenter object under rootObject";
        return 1;
    }
    presenter->setFormat(videoFormat);

    AudioOutput audioOutput{audioFormat, &app};

    VideoFileReader videoFileReader{options.videoFile, videoFormat, &app};
    AudioFileReader audioFileReader{options.audioFile, audioFormat,
                videoFormat.frameRate(), &app};

    Orchestrator orchestrator;

    QObject::connect(&videoFileReader, &VideoFileReader::frameReady,
                     &orchestrator, &Orchestrator::enqueueVideoFrame,
                     Qt::DirectConnection);
    QObject::connect(&audioFileReader, &AudioFileReader::samplesReady,
                     &orchestrator, &Orchestrator::enqueueAudioFrame,
                     Qt::DirectConnection);

    QObject::connect(&orchestrator, &Orchestrator::videoFrameReady,
                     presenter, &FramesPresenter::presentFrame,
                     Qt::BlockingQueuedConnection);
    QObject::connect(&orchestrator, &Orchestrator::audioFrameReady,
                     &audioOutput, &AudioOutput::playAudio,
                     Qt::BlockingQueuedConnection);

    QObject::connect(&app, &QGuiApplication::aboutToQuit,
                     &app, [&]() {
        QObject::disconnect(&videoFileReader, &VideoFileReader::frameReady,
                         &orchestrator, &Orchestrator::enqueueVideoFrame);
        QObject::disconnect(&audioFileReader, &AudioFileReader::samplesReady,
                         &orchestrator, &Orchestrator::enqueueAudioFrame);

        QObject::disconnect(&orchestrator, &Orchestrator::videoFrameReady,
                         presenter, &FramesPresenter::presentFrame);
        QObject::disconnect(&orchestrator, &Orchestrator::audioFrameReady,
                         &audioOutput, &AudioOutput::playAudio);
        videoFileReader.stop();
        audioFileReader.stop();
        videoFileReader.wait();
        audioFileReader.wait();
        orchestrator.stop();
    });

    videoFileReader.start();
    audioFileReader.start();
    orchestrator.start();

    return app.exec();
}

void processCommandLine(PlayerOptions &options)
{
    QCommandLineParser parser;
    parser.setApplicationDescription(
                "Raw video & audio frames player built with Qt.");
    parser.addHelpOption();
    parser.addOption({{"v", "video-file"},
                      "Video file path", "file"});
    parser.addOption({{"a", "audio-file"},
                      "Audio file path", "file"});
    parser.addOption({{"s", "frame-size"},
                      "Video frame size", "WxH"});
    parser.addOption({{"r", "frame-rate"},
                      "Frame rate", "fps"});
    parser.addOption({{"c", "audio-channels"},
                      "Audio channels", "count"});
    parser.process(QCoreApplication::arguments());

    // Set PlayerOptions from parsed command line arguments
    options.videoFile = parser.value("video-file");
    options.audioFile = parser.value("audio-file");
    auto frameSizeParts = parser.value("frame-size").split("x");
    if (frameSizeParts.size() == 2) {
        options.frameSize = {frameSizeParts[0].toInt(),
                             frameSizeParts[1].toInt()};
    }
    options.frameRate = parser.value("frame-rate").toDouble();
    options.audioChannels = parser.value("audio-channels").toInt();
}
