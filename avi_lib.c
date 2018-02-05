#include "avi_header.h"

// 保存先の配列に1バイトずつファイルから読み込んで書き込む
void read_four_bytes(FILE **fp, uint8_t* save_location){
	for(int32_t i = 0; i < 4; i++){
		fread(&save_location[i], sizeof(int8_t), 1, *fp);
    }
}

// 構造体にaviファイルのavih以下のヘッダ情報を書き込む
void read_to_avi_header_struct(FILE **fp, AVIMAINHEADER* aviheader){
	fread(&aviheader->cb, sizeof(int32_t), 1, *fp);

	// マイクロ秒単位でのfpsを読み込み
	read_four_bytes(&*fp, aviheader->dwMicroSecPerFrame);

	fread(&aviheader->dwMaxBytesPerSec, sizeof(int32_t), 1, *fp);
    fread(&aviheader->dwPaddingGranularity, sizeof(int32_t), 1, *fp);
    fread(&aviheader->dwFlags, sizeof(int32_t), 1, *fp);

    // 総フレーム数を読み込み
    read_four_bytes(&*fp, aviheader->dwTotalFrames);


    fread(&aviheader->dwInitialFrames, sizeof(int32_t), 1, *fp);
    fread(&aviheader->dwStreams, sizeof(int32_t), 1, *fp);
    fread(&aviheader->dwSuggestedBufferSize, sizeof(int32_t), 1, *fp);

    // 横を読み込み
    read_four_bytes(&*fp, aviheader->dwWidth);

    // 縦を読み込み
    read_four_bytes(&*fp, aviheader->dwHeight);

    for(int32_t i = 0; i < 4; i++){
    	fread(&aviheader->dwReserved[i], sizeof(int32_t), 1, *fp);
    }


}