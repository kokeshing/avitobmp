#include <stdio.h>
#include <stdint.h>
#include <string.h>

// ファイルヘッダ 14バイト
typedef struct{
	uint8_t file_type[2];        // ファイルタイプ bmpファイルでは BM を格納
	uint8_t file_size[4];        // ファイルサイズを符号なし整数値で格納 uint32_tで691254を格納すればいいはずなのだが なぜか  36 8C 0A 00 なぜか 00 00 36 8C 0A と入ってしまうので1バイトごとに書き込む
	uint16_t reserved1;          // 予約領域1 0を格納
	uint16_t reserved2;          // 予約領域2 0を格納
	uint8_t imagedata_offset[4];   // 0またはイメージデータ先頭までのバイト単位でのオフセットを符号なし整数値で格納 これもuint32_tで 54 を格納すればいいはずなのだが以下略
}bmp_file_header;

// INFOタイプの情報ヘッダ 40バイト
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

// bmp全体 14+40+(高さ×幅x3)バイト 今回は14+40+640x360x3=691254 またaviファイルも640x360である必要がある
typedef struct{
    bmp_file_header fheader;
    bmp_info_header iheader;
    uint8_t img[640][360][3];
}bmp_data;

int32_t main(int argc, char *argv[]){

    // 対象画像のパスの取得
    uint8_t path[256] = {0};
    uint32_t pathlen;
    puts(u8"aviファイルの場所を入力");
    fgets(path, 255, stdin);
    pathlen = strlen(path);
    path[pathlen-1] = 0; // 改行コードの削除

    // ファイルを開く
    FILE *fp, *fw;
    fp = fopen(path, "rb");

    // ファイルの一時的な読みこみ先
    int32_t tmp[4] = {0};

    // bmpの初期化
    bmp_data data = {0};

    // 連番で保存先ファイル名を出力するための準備
    uint8_t save_file_nm[100] = {0};

    // ファイルヘッダの初期化
    data.fheader.file_type[0] = 66;
    data.fheader.file_type[1] = 77;
    data.fheader.file_size[0] = 54;
    data.fheader.file_size[1] = 140;
    data.fheader.file_size[2] = 10;
    data.fheader.file_size[3] = 0;
    data.fheader.reserved1 = 0;
    data.fheader.reserved1 = 0;
    data.fheader.imagedata_offset[0] = 54;
    data.fheader.imagedata_offset[1] = 0;
    data.fheader.imagedata_offset[2] = 0;
    data.fheader.imagedata_offset[3] = 0;


    // 情報ヘッダの初期化
    data.iheader.header_size = 40;
    data.iheader.width = 640;
    data.iheader.height = 360;
    data.iheader.plane_num = 1;
    data.iheader.bit_num = 24;
    data.iheader.compression_type = 0;
    data.iheader.imagedata_size = 0;
    data.iheader.horizon_dpi = 0;
    data.iheader.vertical_dpi = 0;
    data.iheader.color_index_num = 16777216;
    data.iheader.important_index_num = 0;
    
    // aviファイルから先頭4バイトを読み込み
    fread(&tmp[0], sizeof(int8_t), 1, fp);
    fread(&tmp[1], sizeof(int8_t), 1, fp);
    fread(&tmp[2], sizeof(int8_t), 1, fp);
    fread(&tmp[3], sizeof(int8_t), 1, fp);

    for(int32_t i = 0; i < 166; i++){	
    	// 16進数ダンプで"30 30 64 62" つまり 00db の文字列に当たるまでファイルを空読みする
    	while(tmp[0] != 48 || tmp[1] != 48 || tmp[2] != 100 || tmp[3] != 98){
    	
    		// さらに4バイト空読み
    		fread(&tmp[0], sizeof(int8_t), 1, fp);
    		fread(&tmp[1], sizeof(int8_t), 1, fp);
    		fread(&tmp[2], sizeof(int8_t), 1, fp);
    		fread(&tmp[3], sizeof(int8_t), 1, fp);

    	}

    	// 00db以降のbmpの画素部分データを読み取って格納 この際aviファイルはRBGではなくBRGなのでついでにRBGにする
    	for(uint32_t i = 0; i < 640; i++){
        	for(uint32_t j = 0; j < 360; j++){
                fread(&data.img[i][j][2], sizeof(int8_t), 1, fp);
                fread(&data.img[i][j][0], sizeof(int8_t), 1, fp);
                fread(&data.img[i][j][1], sizeof(int8_t), 1, fp);
        	}
    	}

    	// 保存先ファイルを開く
    	sprintf(save_file_nm, "./bmp/%04d.bmp", i);
    	fw = fopen(save_file_nm, "wb");

    	// out.bmpに書き込んでいく
    	fwrite(&data.fheader, sizeof(int8_t), 14, fw);
    	fwrite(&data.iheader, sizeof(int8_t), 40, fw);
    	for(uint32_t i = 0; i < 640; i++){
        	for(uint32_t j = 0; j < 360; j++){
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