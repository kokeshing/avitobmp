#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "avi_header.h"
#include "bmp_struct.h"

// 関数プロトタイプ宣言
int32_t four_int8_to_one_int32(uint8_t*);

int32_t main(){

    // 対象aviのパスの取得
    uint8_t path[256] = {0};
    uint32_t pathlen;
    puts(u8"aviファイルの場所を入力");
    fgets(path, 255, stdin);
    pathlen = strlen(path);
    path[pathlen-1] = 0; // 改行コードの削除

    // 秒を指定
    int32_t start_sec = 0, end_sec = 0;
    puts(u8"gifにするはじめの動画時間(秒)を入力");
    scanf("%d", &start_sec);
    puts(u8"gifにする終わりの動画時間(秒)を入力");
    scanf("%d", &end_sec);


    // ファイルを開く
    FILE *fp, *fw;
    fp = fopen(path, "rb");

    // aviファイルのヘッダーの読み込み先とファイルの一時的な読みこみ先
    AVIMAINHEADER aviheader;
    uint8_t tmp[4] = {0};

    // bmpの初期化
    bmp_data data = {0};

    // 連番で保存先ファイル名を出力するための準備
    uint8_t save_file_nm[100] = {0};

    // ファイルヘッダの初期化
    initialize_fheader(&data);

    // 情報ヘッダの初期化
    initialize_iheader(&data);
    
    // aviファイルのヘッダーの読み込み
    
    //avih に到達するまで空読み
    while(tmp[0] != 97 || tmp[1] != 118 || tmp[2] != 105 || tmp[3] != 104){
    	read_four_bytes(&fp, tmp);
    }


    read_to_avi_header_struct(&fp, &aviheader);
    
    
    // マイクロ秒単位でのfpsを計算
    int32_t fpmicros = 0;
    fpmicros = four_int8_to_one_int32(aviheader.dwMicroSecPerFrame);
    
    // 総フレーム数を計算
    int32_t total_frame = 0;
    total_frame = four_int8_to_one_int32(aviheader.dwTotalFrames);

    // 総フレーム数を計算
    int32_t width = 0;
    width = four_int8_to_one_int32(aviheader.dwWidth);
    
    // 総フレーム数を計算
    int32_t height = 0;
    height = four_int8_to_one_int32(aviheader.dwHeight);

    // エラー処理
    if(width > 640 || height > 360){
        puts(u8"640x360以下のaviしか扱えません");
        return -1;
    }

    // はじめりのフレーム、終わりのフレーム、フレーム長さを計算
    int32_t start_frame = 0, end_frame = 0, length_frame = 0;
    start_frame = start_sec * 1000000 / fpmicros;
    end_frame = end_sec * 1000000 / fpmicros;
    length_frame = end_frame - start_frame + 1;

    // エラー処理
    if(length_frame < 0 || start_frame > total_frame || end_frame > total_frame){
        puts(u8"動画秒数が正しく指定されていません");
        return -1;
    }

    // frame_countは通過した"00db"の数を数える 不要なバイナリはgomiにどんどん読み込ませる
    // ファイルポインタのポインタ演算のリファレンスがなかったのでgomiにどんどん読み込ませるようにしました
    int32_t frame_count = 0;
    int32_t gomi = 0;
    
    for(int32_t i = 0; i < length_frame; i++){	
    	// フレームカウンタがスタートフレームに達するまで読み込み
    	while(frame_count < start_frame){
    		// さらに4バイト空読み
    		read_four_bytes(&fp, tmp);

            // 00dbの文字列に当たったらフレームカウンタをインクリメント
            if(tmp[0] == 48 && tmp[1] == 48 && tmp[2] == 100 && tmp[3] == 98){
            	for(int32_t i = 0; i < width * height * 3; i++){
            		fread(&gomi, sizeof(int8_t), 1, fp);
            	}
                frame_count++;
            }
    	}

        // 16進数ダンプで"30 30 64 62" つまり 00db の文字列に当たるまでファイルを空読みする
        while(tmp[0] != 48 || tmp[1] != 48 || tmp[2] != 100 || tmp[3] != 98){
            // さらに4バイト空読み
        	read_four_bytes(&fp, tmp);
        }

    	// 00db以降のbmpの画素部分データを読み取って格納 この際aviファイルはRBGではなくBRGなのでついでにRBGにする
    	for(uint32_t w = 0; w < width; w++){
        	for(uint32_t h = 0; h < height; h++){
                fread(&data.img[w][h][2], sizeof(int8_t), 1, fp);
                fread(&data.img[w][h][0], sizeof(int8_t), 1, fp);
                fread(&data.img[w][h][1], sizeof(int8_t), 1, fp);
        	}
    	}

    	// 保存先ファイルを開く
    	sprintf(save_file_nm, "./bmp/%04d.bmp", i);
    	fw = fopen(save_file_nm, "wb");

    	// 連番.bmpに書き込んでいく
    	write_to_bmp(&fw, &data);

    	// 保存先ファイルを閉じる
    	fclose(fw);

    	// 次のチャンクを探す
    	read_four_bytes(&fp, tmp);
    }
    fclose(fp);

    return 0;
}

int32_t four_int8_to_one_int32(uint8_t* a){
    int32_t ret = 0;
    ret = a[0] + a[1] * 256 + a[2] * 256 * 256 + a[3] * 256 * 256 * 256;
    return ret;
}

