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
#include "Range.hpp"
#include "OpticalFlow.hpp"

class MatchMain
{
public:
    MatchMain
    (const Transform& otf, const Transform& tf,
     const ExtractFeaturePoint& efp, const MatchFeaturePoint& mfp,
     const Estimate& est, const Range& rg, const OpticalFlow& of);
    ~MatchMain();
    
    // １つ目の画像に合わせて２つ目の画像を修正する
    void ModifylatterImg
    (const cv::Mat& forImg, const cv::Mat& latImg, cv::Mat& modLatImg);
   
    // 動画を途中から修正
    void ModifyVideoMid
    (VideoReader& vrOriginal, VideoReader& vrPreFixed, VideoWriterPic& vw,
     const int frameNum, const cv::Mat& curRotMat);
    
    void ModifyPOVImg
    (const cv::Mat& img, cv::Mat& rotImg);
    
    void ModifyPOVVideo(VideoReader& vr, VideoWriter& vw);
    
    void ModifyVideo(VideoReader& vr, VideoWriter& vw);
    
    void getMatchCoord
    (const std::vector<cv::KeyPoint>& forKeyPoints,
     const cv::Mat& forDescriptors,
     const std::vector<cv::KeyPoint>& latKeyPoints,
     const cv::Mat& latDescriprots,
     std::vector<cv::Point3f>& forSpheres,
     std::vector<cv::Point3f>& latSpheres);
    
        
private:
    const Transform& otf;
    const Transform& tf;
    const ExtractFeaturePoint& efp;
    const MatchFeaturePoint& mfp;
    const Estimate& est;
    const Range& rg;
    const OpticalFlow& of;
    
    cv::Mat accMat;
    cv::Mat preMat;
    std::vector<cv::KeyPoint> tmpKeyPoints;
    cv::Mat tmpDescriptors;
};

#endif /* MatchMain_hpp */
