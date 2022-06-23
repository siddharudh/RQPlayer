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

#include "filereaders.h"
#include "framespresenter.h"
#include "audiooutput.h"

void processCommandLine();

int main(int argc, char *argv[])
{
    QCoreApplication::setApplicationName("RQPlayer");
    QCoreApplication::setApplicationVersion("1.0");

#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif

    QGuiApplication app(argc, argv);
    QQmlApplicationEngine engine;

    processCommandLine();

    using namespace RQPlayer;
    qmlRegisterType<FramesPresenter>("RQPlayer", 1, 0, "FramesPresenter");

    const QUrl url("qrc:/main.qml");
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
        if (url == objUrl && !obj)
            QCoreApplication::exit(-1);
    }, Qt::QueuedConnection);
    engine.load(url);

    QVideoSurfaceFormat videoFormat(
                QSize(640, 480), QVideoFrame::Format_YUV422P);
    videoFormat.setFrameRate(25);

    QAudioFormat audioFormat;
    audioFormat.setByteOrder(QAudioFormat::LittleEndian);
    audioFormat.setChannelCount(1);
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

    AudioOutput audioOutput(audioFormat, &app);

    VideoFileReader videoFileReader("/tmp/vpipe", videoFormat, &app);
    AudioFileReader audioFileReader("/tmp/apipe", audioFormat,
                             videoFormat.frameRate(), &app);

    QObject::connect(&videoFileReader, &VideoFileReader::frameReady,
                     presenter, &FramesPresenter::presentFrame,
                     Qt::BlockingQueuedConnection);
    QObject::connect(&audioFileReader, &AudioFileReader::samplesReady,
                     &audioOutput, &AudioOutput::playAudio,
                     Qt::BlockingQueuedConnection);

    videoFileReader.start();
    audioFileReader.start();

    return app.exec();
}

void processCommandLine()
{
    QCommandLineParser parser;
    parser.setApplicationDescription("Raw video & audio frames player built with Qt.");
    parser.addHelpOption();
    parser.addVersionOption();

    parser.process(QCoreApplication::arguments());
}
