//
//  JackInHeadEstimator.hpp
//  OmnidirectionalImage
//
//  Created by masakazu nakazawa on 2017/01/15.
//  Copyright © 2017年 masakazu. All rights reserved.
//

#ifndef JackInHeadEstimator_hpp
#define JackInHeadEstimator_hpp

#include <stdio.h>

#include <opencv2/core.hpp>

#include "Estimator.hpp"
#include "CalcOpticalFlow.hpp"
#include "Rotation.hpp"

class JackInHeadEstimator: public Estimator
{
public:
    JackInHeadEstimator(const Transform& tf): Estimator(tf) {
        srand((unsigned) time(NULL));
    };
    
    // インタフェース
    cv::Mat getRotMat
    (const cv::Mat& forImg, const cv::Mat& latImg, const State& state);
    
    // 実体
    cv::Mat getRotMat
    (const cv::Mat& forImg, const cv::Mat& latImg);
    
    // アフィン変換により回転行列を推定
    cv::Mat getRotMatAffine
    (const std::vector<Sphere>& forSpheres,
     const std::vector<Sphere>& latSpheres);
    
    // 全要素から訓練データをランダムに抽出する
    // 訓練データを除去した要素はテストデータになる
    void extTrainFromAll
    (std::vector<Sphere>& forAll, std::vector<Sphere>& latAll,
     std::vector<Sphere>& forTrain, std::vector<Sphere>& latTrain);
    
    // オプティカルフローのマスク
    void getOptflowMask(cv::Mat& mask);
    
    // オプティカルフローを描画
    void drawOptflow
    (const cv::Mat& img, const std::vector<Equirect> forEquis,
     const std::vector<Equirect> latEquis, cv::Mat& drawImg);
    
private:
    
};

#endif /* JackInHeadEstimator_hpp */
