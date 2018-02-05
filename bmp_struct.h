#ifndef _bmp_lib_H
#define _bmp_lib_H

#include <stdio.h>
#include <stdint.h>

// bmpファイルヘッダ 14バイト
typedef struct{
    uint8_t file_type[2];        // ファイルタイプ bmpファイルでは BM を格納したお
    uint8_t file_size[4];        // ファイルサイズを符号なし整数値で格納 uint32_tで691254を格納すればいいはずなのだが なぜか  36 8C 0A 00 なぜか 00 00 36 8C 0A と入ってしまうので1バイトごとに書き込んだお
    uint16_t reserved1;          // 予約領域1 0を格納したお
    uint16_t reserved2;          // 予約領域2 0を格納したお
    uint8_t imagedata_offset[4];   // 0またはイメージデータ先頭までのバイト単位でのオフセットを符号なし整数値で格納 これもuint32_tで 54 を格納すればいいはずなのだが以下略したお
}bmp_file_header;

// bmpINFOタイプの情報ヘッダ 40バイト
typedef struct{
    uint32_t header_size;         // ヘッダサイズ 情報ヘッダのサイズを符号なし整数値で格納したお
    int32_t width;                // 幅を符号付き整数値で格納したお
    int32_t height;               // 高さを符号付き整数値で格納したお
    uint16_t plane_num;           // プレーン数を符号なし整数値で格納 bmpファイルでは1したお
    uint16_t bit_num;             // ピット数を符号なし整数値で格納 今回は24にしたいな
    uint32_t compression_type;    // 圧縮タイプをお符号なし整数値で格納 今回は0にするつもり
    uint32_t imagedata_size;      // イメージデータサイズ 0でいいかも？
    uint32_t horizon_dpi;         // 水平解像度・垂直解像度 どっちも0でいいきがする
    uint32_t vertical_dpi;
    uint32_t color_index_num;     // カラーインデックス数 2^24を格納したかも
    uint32_t important_index_num; // 重要インデックス数 0でいいはず
}bmp_info_header;

// bmp全体 14+40+(高さ×幅x3)バイト 今回は14+40+640x360x3=691254バイト またaviファイルも640x360である必要があるのだろうか？
typedef struct{
    bmp_file_header fheader;
    bmp_info_header iheader;
    uint8_t img[640][360][3];
}bmp_data;

// 関数プロトタイプ宣言なのか？
void initialize_fheader(bmp_data*);
void initialize_iheader(bmp_data*);
void write_to_bmp(FILE **fw, bmp_data* data);


#endif