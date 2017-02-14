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
#include <opencv2/opencv.hpp>

#include "Core.hpp"
#include "VideoReader.hpp"
#include "VideoWriter.hpp"

class FisheyeConvert {
public:
    FisheyeConvert(const cv::Size& fsFish, const int radiusFish, const float fov);
    
    // 画像を変換
    void convertImg(const cv::Mat& fishImg, cv::Mat& equiImg) const;
    
    void convertSingleImg(const cv::Mat& sinFishImg, cv::Mat& sinEquiImg) const;
    
    void convertVideo(VideoReader& vr, VideoWriter& vw) const;
    
private:
    const cv::Size fsFish;
    const int radius;
//    const cv::Size fsEqui;
    const int mergin;
    cv::Rect rectLeft;
    cv::Rect rectRight;
};

#endif /* FisheyeConvert_hpp */
