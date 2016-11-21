//
//  IMUFile.cpp
//  OmnidirectionalImage
//
//  Created by masakazu nakazawa on 2016/11/21.
//  Copyright © 2016年 masakazu. All rights reserved.
//

#include "IMUFile.hpp"

IMUFile::IMUFile(const std::string& fileName): ifs(fileName)
{
    // ファイルを正常に開いたか
    if (!ifs) {
        std::cerr << "cannot open file" << std::endl;
        exit(1);
    }
}

IMUFile::~IMUFile()
{
    ifs.close();
}

bool IMUFile::inputDataSet(IMU_Data& data)
{
    // データ組を文字列で読み込み
    std::string dataLine;
    std::getline(ifs, dataLine);
    
    // int型でデータを読み込み
    const int inNum =  sscanf(dataLine.c_str(),
                              "%d %d %d %d %d %d %d %d %d",
                              &data.accel_x, &data.accel_y, &data.accel_z,
                              &data.gyro_x, &data.gyro_y, &data.gyro_z,
                              &data.mag_x, &data.mag_y, &data.mag_z);

    // 読み込み失敗またはファイル末尾
    if (inNum < 9) return false;
    
    return true;
}



