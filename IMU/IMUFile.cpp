//
//  IMUFile.cpp
//  OmnidirectionalImage
//
//  Created by masakazu nakazawa on 2016/11/22.
//  Copyright © 2016年 masakazu. All rights reserved.
//

#include "IMUFile.hpp"

IMUFile::IMUFile(const std::string& fileName):
IMU(), ifs(fileName), dataSize(3)
{
    if (!ifs) {
        std::cerr << "cannnot open file" << std::endl;
        exit(1);
    }
}

int IMUFile::inputDataSet(std::vector<IMUData> &datas)
{
    // データ格納領域確保
    datas = std::vector<IMUData>(dataSize);
    
    int dataCount = 0;
    while (dataCount < dataSize) {
        // ファイルから一行読み込み
        std::string dataLine;
        if (!std::getline(ifs, dataLine)) break;
        
        // 文字列からデータ読み込み
        IMUData data;
        const int inNum =
            sscanf(dataLine.c_str(),
                   "%d %d %d %d %d %d %d %d %d",
                    &data.accel_x, &data.accel_y, &data.accel_z,
                    &data.gyro_x, &data.gyro_y, &data.gyro_z,
                    &data.mag_x, &data.mag_y, &data.mag_z);
     
        // データが正しく読み込まれたか
        if (inNum == 9) {
            datas[dataCount] = data;
            dataCount++;
        }
    }
    
    // データ組数を返す
    return dataCount;
}
