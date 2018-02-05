#ifndef _avi_header_H
#define _avi_header_H

#include <stdio.h>
#include <stdint.h>

// aviファイルヘッダ 
typedef struct {
    uint8_t fcc[4]; // avihとなければならない 61 76 69 68
    uint32_t  cb;
    uint8_t  dwMicroSecPerFrame[4]; // 1マイクロ秒に何回フレームが切り替わるか 60fps = 16666 = 1a 41 00 00 uint32_t型を強引にuint8_tで読み出す
    uint32_t  dwMaxBytesPerSec;
    uint32_t  dwPaddingGranularity;
    uint32_t  dwFlags;
    uint8_t  dwTotalFrames[4];       // 全フレーム数 166 =  a6 00 00 00 uint32_t型を強引にuint8_tで読み出す
    uint32_t  dwInitialFrames;     
    uint32_t  dwStreams;
    uint32_t  dwSuggestedBufferSize;
    uint8_t  dwWidth[4];                 // 幅px 640px = 80 02 00 00 uint32_t型を強引にuint8_tで読み出す
    uint8_t  dwHeight[4];                // 高さpx 360px = 68 01 00 00 uint32_t型を強引にuint8_tで読み出す
    uint32_t  dwReserved[4];
} AVIMAINHEADER;

#endif