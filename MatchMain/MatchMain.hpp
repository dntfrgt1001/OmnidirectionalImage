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

#include <opencv2/core.hpp>

#include "Transform.hpp"
#include "ExtractFeaturePoint.hpp"
#include "MatchFeaturePoint.hpp"
#include "Rotation.hpp"
#include "VideoReader.hpp"
#include "VideoWriter.hpp"
#include "Estimate.hpp"

class MatchMain
{
public:
    MatchMain
    (const Transform& otf, const Transform& tf,
     const ExtractFeaturePoint& efp,
     const MatchFeaturePoint& mfp, const Estimate& est);
    ~MatchMain();
    
    // １つ目の画像に合わせて２つ目の画像を修正する
    void ModifylatterImg
    (const cv::Mat& forImg, const cv::Mat& latImg, cv::Mat& modLatImg);
    // 初期フレームに合わせて動画を修正する
   
    void ModifyPOVImg
    (const cv::Mat& img, cv::Mat& rotImg);
    
    void ModifyPOVVideo(VideoReader& vr, VideoWriter& vw);
    
    void ModifyVideo(VideoReader& vr, VideoWriter& vw);
    
private:
    const Transform& otf;
    const Transform& tf;
    const ExtractFeaturePoint& efp;
    const MatchFeaturePoint& mfp;
    const Estimate& est;
    cv::Mat accMat;
    cv::Mat preMat;
    std::vector<cv::KeyPoint> tmpKeyPoints;
    cv::Mat tmpDescriptors;
};

#endif /* MatchMain_hpp */
