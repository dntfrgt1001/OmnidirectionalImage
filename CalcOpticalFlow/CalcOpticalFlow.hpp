//
//  CalcOpticalFlow.hpp
//  OmnidirectionalImage
//
//  Created by masakazu nakazawa on 2016/10/27.
//  Copyright © 2016年 masakazu. All rights reserved.
//

#ifndef CalcOpticalFlow_hpp
#define CalcOpticalFlow_hpp

#include <stdio.h>

#include <opencv2/core.hpp>
#include <opencv2/video.hpp>

#include "Perspective.hpp"

class CalcOpticalFlow {
public:
    CalcOpticalFlow
    (const float margin, const Perspective& per, const float angRag):
    optflowMask(per.getMask(margin)), cosRag(cosf(angRag)) {};
    
    // 正面/背面の特徴点の組を求める
    void getNormalPair
    (const cv::Mat& forPersImg, const cv::Mat& latPersImg,
     const cv::Mat& curRotMat, std::vector<cv::Point2f>& forNormals,
     std::vector<cv::Point2f>& latNormals) const;
    
    // 正面か背面の特徴点の組を求める
    void getNormalPairOneDir
    (const cv::Mat& forPersImg, const cv::Mat& latPersImg,
     const cv::Mat& curRotMat, const bool isFront,
     std::vector<cv::Point2f>& forNormals,
     std::vector<cv::Point2f>& latNormals) const;
    
    // オプティカルフローを求める
    void getOpticalFlow
    (const cv::Mat& forPersImg, const cv::Mat& latPersImg,
     std::vector<cv::Point2f>& forPerss,
     std::vector<cv::Point2f>& latPerss) const;
    
    // 正規化画像座標で半径方向と直交しないものを取り除く
    void remOrthOutlier
    (std::vector<cv::Point2f>& forNormals,
     std::vector<cv::Point2f>& latNormals) const;
    
    // 直交の条件を満たすか
    bool isOrthCond
    (const cv::Point2f& forNormal, const cv::Point2f& latNormal) const {
        cv::Point2f rad = (forNormal + latNormal) / 2.0;
        cv::Point2f cir = latNormal - forNormal;
        
        // 半径方向となす角のコサイン
        float cosAng = rad.dot(cir) / (cv::norm(rad)*cv::norm(cir));
        
        // コサインの範囲内か
        if (-cosRag < cosAng && cosAng < cosRag) return true;
        else return false;
    }
    
    // 回転方向が逆ではないか
    bool isRotDirCond
    (const cv::Point2f& forNormal, const cv::Point2f& latNormal) const {
        
    }
    
    // オプティカルフロー描画
    void drawOpticalFlow
    (const cv::Mat& persImg, const std::vector<cv::Point2f>& forPerss,
     const std::vector<cv::Point2f>& latPerss, cv::Mat& drawImg) const;
    

private:
    const cv::Mat optflowMask;
    const float cosRag;
};

#endif /* CalcOpticalFlow_hpp */
