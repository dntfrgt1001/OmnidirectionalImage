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
    FisheyeConvert
    (const cv::Size& fsFish, const int radFish, const float fov,
     const float radius, const float focus);
    
    // 動画を変換
    void convertVideo(VideoReader& vr, VideoWriter& vw) const;
    
    // 画像を変換
    void convertImg(const cv::Mat& fishImg, cv::Mat& equiImg) const;
    
    // 片方の魚眼画像を変換
    void convertSingleImg(const cv::Mat& sinFishImg, cv::Mat& sinEquiImg) const;
    
    // 全天球画像座標を魚眼画像座標に変換
    virtual cv::Point2f equirect2fish(const Equirect& equi) const = 0;
    
    // 全天球画像座標を魚眼極座標に変換
    cv::Point2f equirect2FishPolar(const Equirect& equi) const;
    
protected:
    // 射影方式依存
    const float focus;
    const float radius;
    cv::Rect rectLeft;
    cv::Rect rectRight;
};

class FisheyeConvertEquiDist: public FisheyeConvert
{
public:
    FisheyeConvertEquiDist
    (const cv::Size& fsFish, const int radFish, const float fov);
    
    cv::Point2f equirect2fish(const Equirect& equi) const;
};

class FisheyeConvertStereGraph: public FisheyeConvert {
public:
    FisheyeConvertStereGraph
    (const cv::Size& fsFish, const int radFish, const float fov);
    
    cv::Point2f equirect2fish(const Equirect& equi) const;
};

class FisheyeConvertEquiSolid: public FisheyeConvert {
public:
    FisheyeConvertEquiSolid
    (const cv::Size& fsFish, const int radFish, const float fov);
    
    cv::Point2f equirect2fish(const Equirect& equi) const;
};

#endif /* FisheyeConvert_hpp */
