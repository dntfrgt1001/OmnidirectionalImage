//
//  IMUFile.hpp
//  OmnidirectionalImage
//
//  Created by masakazu nakazawa on 2016/11/22.
//  Copyright © 2016年 masakazu. All rights reserved.
//

#ifndef IMUFile_hpp
#define IMUFile_hpp

#include <stdio.h>
#include <iostream>
#include <fstream>


#include "IMU.hpp"

class IMUFile: public IMU
{
public:
    IMUFile(const std::string& fileName);
    
    int inputDataSet(std::vector<IMUData>& datas);
    
private:
    // 入力ファイルストリーム
    std::ifstream ifs;
    
    // 一度に読み込みデータ組数
    const int dataSize;
};

#endif /* IMUFile_hpp */
