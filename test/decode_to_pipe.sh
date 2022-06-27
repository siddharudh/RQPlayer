#!/bin/bash

ffmpeg -y -i $1 -map 0:v -r 25 -s 640x480 -f rawvideo -c:v rawvideo -pix_fmt yuv422p /tmp/vpipe -map 0:a:0 -r 25 -ar 48000 -ac 1 -f s16le -c:a pcm_s16le /tmp/apipe
