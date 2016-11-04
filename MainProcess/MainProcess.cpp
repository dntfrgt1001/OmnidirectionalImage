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
    cv::Mat rotMat = ofe.getRotMat(forImg, latImg, curRotMat);
    
    setMatInfo(rotMat);
    
    tf.rotateImg(latImg, modLatImg, accRotMat);
}

void MainProcess::setMatInfo(const cv::Mat rotMat)
{
    accRotMat = accRotMat * rotMat;
    
    cv::Mat froChgMat;
    Rotation::getFroChgMat(rotMat, froChgMat);
    cv::Vec3f rotVec;
    Rotation::RotMat2RotVec(rotMat, rotVec);
    std::cout <<  "------------------------------------------------";
    std::cout << std::endl;
    std::cout << "Rot Mat = " << std::endl << rotMat << std::endl;
    std::cout <<  "------------------------------------------------";
    std::cout << std::endl;
    std::cout << "Rot Vec = " << rotVec << std::endl;
//    std::cout << std::endl;
    std::cout <<  "------------------------------------------------";
    std::cout << std::endl;
    std::cout << "Acc Mat = " << std::endl << accRotMat << std::endl;
    std::cout <<  "------------------------------------------------";
    std::cout << std::endl;
    std::cout << "Front Mat = " << std::endl << froChgMat << std::endl;
    std::cout <<  "------------------------------------------------";
    std::cout << std::endl;
}

void MainProcess::modVideo(VideoReader &vr, VideoWriter &vw)
{
    cv::Mat forImg, latImg;
    
    cv::namedWindow("former image");
    cv::namedWindow("latter image");
    cv::namedWindow("modified latter image");
    
    // 初期フレームを書き出す
    vr.readImg(latImg);
    vw.writeImg(latImg);
    
    std::cout << "0-th frame finished" << std::endl;
    
    for (int i = 1; vr.hasNext(); i++) {
        // 画像をシフト
        forImg = latImg.clone();
        vr.readImg(latImg);
        
        // 後画像を修正
        cv::Mat latImgMod;
        //modifyLatImgFeatureMatch(forImg, latImg, latImgMod);
        modifyLatImgOpticalFlow(forImg, latImg, latImgMod);
        vw.writeImg(latImgMod);
        
        // 出力
        cv::imshow("former image", forImg);
        cv::imshow("latter image", latImg);
        cv::imshow("modified latter image", latImgMod);
        std::cout << i << "-th frame finished" << std::endl;
        
        cv::waitKey();
    }
}
