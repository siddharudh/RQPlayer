/* audiooutput.h
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

#ifndef RQPLAYER_AUDIOOUTPUT_H
#define RQPLAYER_AUDIOOUTPUT_H

#include <QObject>

class QAudioOutput;
class QIODevice;

#include <QAudioFormat>
#include <QAudioBuffer>

namespace RQPlayer {

class AudioOutput : public QObject
{
    Q_OBJECT
public:
    explicit AudioOutput(const QAudioFormat &audioFormat,
                         QObject *parent = nullptr);

public slots:
    void playAudio(const QAudioBuffer &buf);

private:
    QAudioFormat m_audioFormat;
    QAudioOutput *m_audioOutput;
    QIODevice *m_audioStream;
};

} // namespace RQPlayer

#endif // RQPLAYER_AUDIOOUTPUT_H
