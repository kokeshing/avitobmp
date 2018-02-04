#include <stdio.h>
#include <stdint.h>
#include <string.h>

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

// bmpファイルヘッダ 14バイト
typedef struct{
	uint8_t file_type[2];        // ファイルタイプ bmpファイルでは BM を格納
	uint8_t file_size[4];        // ファイルサイズを符号なし整数値で格納 uint32_tで691254を格納すればいいはずなのだが なぜか  36 8C 0A 00 なぜか 00 00 36 8C 0A と入ってしまうので1バイトごとに書き込む
	uint16_t reserved1;          // 予約領域1 0を格納
	uint16_t reserved2;          // 予約領域2 0を格納
	uint8_t imagedata_offset[4];   // 0またはイメージデータ先頭までのバイト単位でのオフセットを符号なし整数値で格納 これもuint32_tで 54 を格納すればいいはずなのだが以下略
}bmp_file_header;

// bmpINFOタイプの情報ヘッダ 40バイト
typedef struct{
	uint32_t header_size;         // ヘッダサイズ 情報ヘッダのサイズを符号なし整数値で格納
	int32_t width;                // 幅を符号付き整数値で格納
	int32_t height;               // 高さを符号付き整数値で格納
	uint16_t plane_num;           // プレーン数を符号なし整数値で格納 bmpファイルでは1
	uint16_t bit_num;             // ピット数を符号なし整数値で格納 今回は24
	uint32_t compression_type;    // 圧縮タイプをお符号なし整数値で格納 今回は0
	uint32_t imagedata_size;      // イメージデータサイズ 0でいい
	uint32_t horizon_dpi;         // 水平解像度・垂直解像度 どっちも0でいい
	uint32_t vertical_dpi;
	uint32_t color_index_num;     // カラーインデックス数 2^24を格納
	uint32_t important_index_num; // 重要インデックス数 0でいい
}bmp_info_header;

// bmp全体 14+40+(高さ×幅x3)バイト 今回は14+40+640x360x3=691254バイト またaviファイルも640x360である必要がある
typedef struct{
    bmp_file_header fheader;
    bmp_info_header iheader;
    uint8_t img[640][360][3];
}bmp_data;

// 関数プロトタイプ宣言
void initialize_fheader(bmp_data*);
void initialize_iheader(bmp_data*);
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
    int32_t tmp[4] = {0};

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
        fread(&tmp[0], sizeof(int8_t), 1, fp);
        fread(&tmp[1], sizeof(int8_t), 1, fp);
        fread(&tmp[2], sizeof(int8_t), 1, fp);
        fread(&tmp[3], sizeof(int8_t), 1, fp);
    }

    fread(&aviheader.cb, sizeof(int32_t), 1, fp);
    
    // マイクロ秒単位でのfpsを読み込み
    fread(&aviheader.dwMicroSecPerFrame[0], sizeof(int8_t), 1, fp);
    fread(&aviheader.dwMicroSecPerFrame[1], sizeof(int8_t), 1, fp);
    fread(&aviheader.dwMicroSecPerFrame[2], sizeof(int8_t), 1, fp);
    fread(&aviheader.dwMicroSecPerFrame[3], sizeof(int8_t), 1, fp);
    // マイクロ秒単位でのfpsを計算
    int32_t fpmicros = 0;
    fpmicros = four_int8_to_one_int32(aviheader.dwMicroSecPerFrame);
    
    fread(&aviheader.dwMaxBytesPerSec, sizeof(int32_t), 1, fp);
    fread(&aviheader.dwPaddingGranularity, sizeof(int32_t), 1, fp);
    fread(&aviheader.dwFlags, sizeof(int32_t), 1, fp);
    
    // 総フレーム数を読み込み
    fread(&aviheader.dwTotalFrames[0], sizeof(int8_t), 1, fp);
    fread(&aviheader.dwTotalFrames[1], sizeof(int8_t), 1, fp);
    fread(&aviheader.dwTotalFrames[2], sizeof(int8_t), 1, fp);
    fread(&aviheader.dwTotalFrames[3], sizeof(int8_t), 1, fp);
    // 総フレーム数を計算
    int32_t total_frame = 0;
    total_frame = four_int8_to_one_int32(aviheader.dwTotalFrames);
    
    fread(&aviheader.dwInitialFrames, sizeof(int32_t), 1, fp);
    fread(&aviheader.dwStreams, sizeof(int32_t), 1, fp);
    fread(&aviheader.dwSuggestedBufferSize, sizeof(int32_t), 1, fp);
    
    // 横を読み込み
    fread(&aviheader.dwWidth[0], sizeof(int8_t), 1, fp);
    fread(&aviheader.dwWidth[1], sizeof(int8_t), 1, fp);
    fread(&aviheader.dwWidth[2], sizeof(int8_t), 1, fp);
    fread(&aviheader.dwWidth[3], sizeof(int8_t), 1, fp);
    // 総フレーム数を計算
    int32_t width = 0;
    width = four_int8_to_one_int32(aviheader.dwWidth);


    // 縦を読み込み
    fread(&aviheader.dwHeight[0], sizeof(int8_t), 1, fp);
    fread(&aviheader.dwHeight[1], sizeof(int8_t), 1, fp);
    fread(&aviheader.dwHeight[2], sizeof(int8_t), 1, fp);
    fread(&aviheader.dwHeight[3], sizeof(int8_t), 1, fp);
    // 総フレーム数を計算
    int32_t height = 0;
    height = four_int8_to_one_int32(aviheader.dwHeight);

    // エラー処理
    if(width > 640 || height > 360){
        puts(u8"640x360以下のaviしか扱えません");
        return -1;
    }

    fread(&aviheader.dwReserved[0], sizeof(int32_t), 1, fp);
    fread(&aviheader.dwReserved[1], sizeof(int32_t), 1, fp);
    fread(&aviheader.dwReserved[2], sizeof(int32_t), 1, fp);
    fread(&aviheader.dwReserved[3], sizeof(int32_t), 1, fp);

    // はじめりのフレーム、終わりのフレーム、フレーム長さを計算
    int32_t start_frame = 0, end_frame = 0, length_frame = 0;
    start_frame = start_sec * 1000000 / fpmicros;
    if(start_frame == 0){
        start_frame = 1;
    }
    end_frame = end_sec * 1000000 / fpmicros;
    length_frame = end_frame - start_frame + 1;

    // エラー処理
    if(length_frame < 0 || start_frame > total_frame || end_frame > total_frame){
        puts(u8"動画秒数が正しく指定されていません");
        return -1;
    }

    // フレームカウンタ
    int32_t frame_count = 0;
    
    for(int32_t i = 0; i < length_frame; i++){	
    	// フレームカウンタがスタートフレームに達するまで読み込み
    	while(frame_count < start_frame){
    		// さらに4バイト空読み
    		fread(&tmp[0], sizeof(int8_t), 1, fp);
    		fread(&tmp[1], sizeof(int8_t), 1, fp);
    		fread(&tmp[2], sizeof(int8_t), 1, fp);
    		fread(&tmp[3], sizeof(int8_t), 1, fp);

            // 00dbの文字列に当たったらフレームカウンタをインクリメント
            if(tmp[0] == 48 && tmp[1] == 48 && tmp[2] == 100 && tmp[3] == 98){
                frame_count++;
            }
    	}

        // 16進数ダンプで"30 30 64 62" つまり 00db の文字列に当たるまでファイルを空読みする
        while(tmp[0] != 48 || tmp[1] != 48 || tmp[2] != 100 || tmp[3] != 98){
            // さらに4バイト空読み
            fread(&tmp[0], sizeof(int8_t), 1, fp);
            fread(&tmp[1], sizeof(int8_t), 1, fp);
            fread(&tmp[2], sizeof(int8_t), 1, fp);
            fread(&tmp[3], sizeof(int8_t), 1, fp);
        }

    	// 00db以降のbmpの画素部分データを読み取って格納 この際aviファイルはRBGではなくBRGなのでついでにRBGにする
    	for(uint32_t i = 0; i < width; i++){
        	for(uint32_t j = 0; j < height; j++){
                fread(&data.img[i][j][2], sizeof(int8_t), 1, fp);
                fread(&data.img[i][j][0], sizeof(int8_t), 1, fp);
                fread(&data.img[i][j][1], sizeof(int8_t), 1, fp);
        	}
    	}

    	// 保存先ファイルを開く
    	sprintf(save_file_nm, "./bmp/%04d.bmp", i);
    	fw = fopen(save_file_nm, "wb");

    	// 連番.bmpに書き込んでいく
    	fwrite(&data.fheader, sizeof(int8_t), 14, fw);
    	fwrite(&data.iheader, sizeof(int8_t), 40, fw);
    	for(uint32_t i = 0; i < width; i++){
        	for(uint32_t j = 0; j < height; j++){
            	for(uint32_t c = 0; c < 3; c++){
                	fwrite(&data.img[i][j][c], sizeof(uint8_t), 1, fw);
            	}
        	}
    	}

    	// 保存先ファイルを閉じる
    	fclose(fw);

    	// 次のチャンクを探す
    	fread(&tmp[0], sizeof(int8_t), 1, fp);
    	fread(&tmp[1], sizeof(int8_t), 1, fp);
    	fread(&tmp[2], sizeof(int8_t), 1, fp);
    	fread(&tmp[3], sizeof(int8_t), 1, fp);
    }

    return 0;
}

// ファイルヘッダの初期化
void initialize_fheader(bmp_data* data){
    data->fheader.file_type[0] = 66;
    data->fheader.file_type[1] = 77;
    data->fheader.file_size[0] = 54;
    data->fheader.file_size[1] = 140;
    data->fheader.file_size[2] = 10;
    data->fheader.file_size[3] = 0;
    data->fheader.reserved1 = 0;
    data->fheader.reserved1 = 0;
    data->fheader.imagedata_offset[0] = 54;
    data->fheader.imagedata_offset[1] = 0;
    data->fheader.imagedata_offset[2] = 0;
    data->fheader.imagedata_offset[3] = 0;
}

//情報ヘッダの初期化
void initialize_iheader(bmp_data* data){
    data->iheader.header_size = 40;
    data->iheader.width = 640;
    data->iheader.height = 360;
    data->iheader.plane_num = 1;
    data->iheader.bit_num = 24;
    data->iheader.compression_type = 0;
    data->iheader.imagedata_size = 0;
    data->iheader.horizon_dpi = 0;
    data->iheader.vertical_dpi = 0;
    data->iheader.color_index_num = 16777216;
    data->iheader.important_index_num = 0;
}

int32_t four_int8_to_one_int32(uint8_t* a){
    int32_t ret = 0;
    ret = a[0] + a[1] * 256 + a[2] * 256 * 256 + a[3] * 256 * 256 * 256;
    return ret;
}

