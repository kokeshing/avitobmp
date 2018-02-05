#include "bmp_lib.h"

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