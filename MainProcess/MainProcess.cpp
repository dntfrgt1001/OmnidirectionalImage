//
//  MainProcess.cpp
//  OmnidirectionalImage
//
//  Created by masakazu nakazawa on 2016/10/27.
//  Copyright © 2016年 masakazu. All rights reserved.
//

#include "MainProcess.hpp"

void MainProcess::modifyLatImgFeatureMatch
(const cv::Mat &forImg, const cv::Mat &latImg, cv::Mat &modLatImg)
{
    cv::Mat rotMat = fme.getRotMat(forImg, latImg);
    
    setMatInfo(rotMat);
    
    tf.rotateImg(latImg, modLatImg, accRotMat);
}

void MainProcess::modifyLatImgOpticalFlow
(const cv::Mat &forImg, const cv::Mat &latImg, cv::Mat &modLatImg)
{
    cv::Mat froChgMat = (cv::Mat_<float>(3,3) << 0,0,1,0,1,0,-1,0,0);
    
    cv::Mat rotMat = ofe.getRotMat(forImg, latImg, froChgMat);
    
    setMatInfo(rotMat);
    
    tf.rotateImg(latImg, modLatImg, accRotMat);
}

void MainProcess::modVideo(VideoReader &vr, VideoWriter &vw)
{
    cv::namedWindow("former image");
    cv::namedWindow("latter image");
    cv::namedWindow("modified latter image");
    
    cv::Mat forImg, latImg;
    vr.readImg(latImg);
    vw.writeImg(latImg);
    std::cout << "0-th frame finished" <<std::endl;
    
    for (int i = 1; vr.hasNext(); i++) {
        // 後画像を前画像にコピー
        forImg = latImg.clone();
        
        // 後画像を読み込み
        vr.readImg(latImg);
        
        // 後画像を修正
        cv::Mat modLatImg;
        modifyLatImgOpticalFlow(forImg, latImg, modLatImg);
        vw.writeImg(modLatImg);
   
        cv::imshow("former image", forImg);
        cv::imshow("latter image", latImg);
        cv::imshow("modified latter image", modLatImg);
        
        std::cout << i << "-th frame finished" << std::endl;
        
        cv::waitKey();
    }
}

void MainProcess::setMatInfo(const cv::Mat rotMat)
{
    accRotMat = accRotMat * rotMat;
    
    Rotation::getFroChgMat(accRotMat, froChgMat);
}