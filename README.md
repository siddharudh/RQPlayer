# RQPlayer

Raw Video (YUV) and Audio (PCM) frames player based on Qt.

The whole idea is to outsource the decoding part to external tools (like FFmpeg etc) and just concentrate on playing already decoded video and audio frames by reading files / named pipes etc.
<br/>
<br/>

### Clone and Build RQPlayer 

On Ubuntu 22.04:
```
git clone https://github.com/siddharudh/RQPlayer.git

cd RQPlayer
scripts/install-dep-ubuntu2204.sh

mkdir build
cd build
qmake ../src/RQPlayer.pro
make
```

### Example 1: playing pre-decoded files

1) Create raw video and audio files using FFmpeg

```
ffmpeg -i clip.mp4 -map 0:v -r 25 -s 640x480 -f rawvideo -c:v rawvideo -pix_fmt yuv422p video.yuv

ffmpeg -i clip.mp4 -map 0:a:0 -r 25 -ar 48000 -ac 1 -f s16le -c:a pcm_s16le audio.pcm
```
2) Play raw video and audio files using RQPlayer
```
./RQPlayer -r 25 -s 640x480 -v video.yuv -a audio.pcm
```
<br/>

### Example 2: on-the-fly decode and play (using named pipes)

1) Create named pipes for video and audio data transfer
```
mkfifo /tmp/vpipe /tmp/apipe
```

2) Start decoding session using FFmpeg

```
ffmpeg -y -i clip.mp4 -map 0:v -r 25 -s 640x480 -f rawvideo -c:v rawvideo -pix_fmt yuv422p /tmp/vpipe -map 0:a:0 -r 25 -ar 48000 -ac 1 -f s16le -c:a pcm_s16le /tmp/apipe
```

3) Play video and audio data coming from pipes using RQPlayer
```
./RQPlayer -r 25 -s 640x480 -v /tmp/vpipe -a /tmp/apipe
```
