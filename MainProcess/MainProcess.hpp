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
#include <sys/time.h>

#include "Transform.hpp"
#include "FeatureMatchEstimator.hpp"
#include "OpticalFlowEstimator.hpp"
#include "VideoReader.hpp"
#include "VideoWriter.hpp"
#include "Rotation.hpp"
#include "JackInHeadEstimator.hpp"

class MainProcess
{
public:
    MainProcess(const Transform& tf, Estimator& est):
    tf(tf), est(est),
    curRotMat(cv::Mat::eye(3,3,CV_32FC1)), accRotMat(cv::Mat::eye(3,3,CV_32FC1))
    {};
    
    // 動画の姿勢修正
    void modVideo(VideoReader& vr, VideoWriter& vw);
    
    // 隣接フレーム間の処理と後方フレームの姿勢修正
    void modImg
    (const cv::Mat& forImg, const cv::Mat& latImg, cv::Mat& latImgMod);
    
    void printMatInfo();
    
private:
    const Transform& tf; // 出力画像用
    
    Estimator& est; // エスティメータ
    
    int frameNum; // フレーム番号
    cv::Mat accRotMat; // 初期フレームからの姿勢変化
    cv::Mat curRotMat; // 現在フレームの姿勢変化
};

#endif /* MainProcess_hpp */
