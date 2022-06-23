/* audiooutput.cpp
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

#include "audiooutput.h"

#include <QAudioOutput>

namespace RQPlayer {

AudioOutput::AudioOutput(const QAudioFormat &audioFormat,
                         QObject *parent)
    : QObject(parent), m_audioFormat(audioFormat)
{
    m_audioOutput = new QAudioOutput(m_audioFormat, this);
    m_audioStream = m_audioOutput->start();
}

void AudioOutput::playAudio(const QAudioBuffer &buf)
{
    m_audioStream->write(buf.constData<char>(), buf.byteCount());
}

} // namespace RQPlayer
