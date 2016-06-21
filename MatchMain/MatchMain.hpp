//
//  MatchMain.hpp
//  OmnidirectionalImage
//
//  Created by masakazu nakazawa on 2016/06/15.
//  Copyright © 2016年 masakazu. All rights reserved.
//

#ifndef MatchMain_hpp
#define MatchMain_hpp

#include <stdio.h>
#include <iostream>

#include <opencv2/core/core.hpp>

#include "Transform.hpp"
#include "ExtractFeaturePoint.hpp"
#include "MatchFeaturePoint.hpp"
#include "Affine.hpp"
#include "VideoReader.hpp"
#include "VideoWriter.hpp"

class MatchMain
{
public:
    MatchMain
    (Transform& transform, ExtractFeaturePoint& extractFeature,
     MatchFeaturePoint& matchFeature, Affine& affine);
    ~MatchMain();
    
    // １つ目の画像に合わせて２つ目の画像を修正する
    void ModifylatterImg
    (const cv::Mat& forImg, const cv::Mat& latImg, cv::Mat& modLatImg);
    // 初期フレームに合わせて動画を修正する
    void ModifyVideo(VideoReader& vr, VideoWriter& vw);
    
private:
    Transform& transform;
    ExtractFeaturePoint& extractFeature;
    MatchFeaturePoint& matchFeature;
    Affine& affine;
    cv::Mat accMat;
};

#endif /* MatchMain_hpp */
