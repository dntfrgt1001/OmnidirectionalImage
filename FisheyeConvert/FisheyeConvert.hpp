//
//  FisheyeConvert.hpp
//  OmnidirectionalImage
//
//  Created by masakazu nakazawa on 2017/02/03.
//  Copyright © 2017年 masakazu. All rights reserved.
//

#ifndef FisheyeConvert_hpp
#define FisheyeConvert_hpp

#include <stdio.h>
#include <string>
#include <opencv2/core.hpp>

class FisheyeConvert {
public:
    FisheyeConvert(const std::string& videoName);
    
    // 画像を変換
    void convertImg(const cv::Mat& fisheyeImg, cv::Mat& equirectImg);
    
private:
    const std::string& videoName;
    
};

#endif /* FisheyeConvert_hpp */
