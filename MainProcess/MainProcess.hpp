//
//  MainProcess.hpp
//  OmnidirectionalImage
//
//  Created by masakazu nakazawa on 2016/10/27.
//  Copyright © 2016年 masakazu. All rights reserved.
//

#ifndef MainProcess_hpp
#define MainProcess_hpp

#include <stdio.h>

#include <opencv2/core.hpp>

#include "Transform.hpp"
#include "FeatureMatchEstimator.hpp"
#include "OpticalFlowEstimator.hpp"
#include "VideoReader.hpp"
#include "VideoWriter.hpp"
#include "Rotation.hpp"

class MainProcess
{
public:
    MainProcess
    (const Transform& tf, FeatureMatchEstimator& fme,
     OpticalFlowEstimator& ofe);
    
    void modifyLatImgFeatureMatch
    (const cv::Mat& forImg, const cv::Mat& latImg, cv::Mat& modLatImg);
    
    void modifyLatImgOpticalFlow
    (const cv::Mat& forImg, const cv::Mat& latImg, cv::Mat& modLatImg);
    
    void modVideo(VideoReader& vr, VideoWriter& vw);
    
    void updateFrameInfo();
    
private:
    const Transform& tf; // 出力画像用
    FeatureMatchEstimator& fme; // 特徴マッチベース
    OpticalFlowEstimator& ofe; // オプティカルフローベース
    int frameNum; // フレーム番号
    cv::Mat accRotMat; // 初期フレームからの姿勢変化
    cv::Mat curRotMat; // 現在フレームの姿勢変化
};

#endif /* MainProcess_hpp */
