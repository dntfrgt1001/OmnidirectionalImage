//
//  IMU.hpp
//  OmnidirectionalImage
//
//  Created by masakazu nakazawa on 2016/11/11.
//  Copyright © 2016年 masakazu. All rights reserved.
//

#ifndef IMU_hpp
#define IMU_hpp

#include <stdio.h>
#include <iostream>

#include <termios.h>
#include <fcntl.h>

class  IMU
{
public:
    IMU(const std::string& outPath, const std::string& port,
        const speed_t baudRate, const size_t bufSize,
        const std::string& splpat);
    
    // termios構造体の初期化
    void initTermios(const std::string& port);
    
private:
    const size_t bufSize;
    
};

#endif /* IMU_hpp */
