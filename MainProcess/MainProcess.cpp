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
    curRotMat = fme.getRotMat(forImg, latImg);
    
    setMatInfo();
    
    tf.rotateImg(latImg, modLatImg, accRotMat);
}

void MainProcess::modifyLatImgOpticalFlow
(const cv::Mat &forImg, const cv::Mat &latImg, cv::Mat &modLatImg)
{
    curRotMat = ofe.getRotMat(forImg, latImg, curRotMat);
    
    setMatInfo();
    
    // 画像のテストに注意
    tf.rotateImg(latImg, modLatImg, accRotMat);
}

void MainProcess::setMatInfo()
{
    accRotMat = accRotMat * curRotMat;
    
    cv::Mat froChgMat = Rotation::getFroChgMat(curRotMat);
    cv::Vec3f rotVec = Rotation::RotMat2RotVec(curRotMat);
    
    std::cout <<  "------------------------------------------------";
    std::cout << std::endl;
    std::cout << "Rot Mat = " << std::endl << curRotMat << std::endl;
    std::cout <<  "------------------------------------------------";
    std::cout << std::endl;
    std::cout << "Rot Vec = " << rotVec;
    std::cout << ", norm = " << cv::norm(rotVec) << std::endl;
    
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
        float normRotVec = cv::norm(Rotation::RotMat2RotVec(curRotMat));
        std::cout << "norm = " << normRotVec << std::endl;
        float normRotThre = M_PI / 15;
        if (normRotThre < normRotVec) {
            std::cout << "OpticalFlow Method" << std::endl;
            modifyLatImgOpticalFlow(forImg, latImg, latImgMod);
        } else {
            std::cout << "Feature Matching Method" << std::endl;
            modifyLatImgFeatureMatch(forImg, latImg, latImgMod);
        }
        
        // modifyLatImgFeatureMatch(forImg, latImg, latImgMod);
        // modifyLatImgOpticalFlow(forImg, latImg, latImgMod);
        
        vw.writeImg(latImgMod);
        
        // 出力
        cv::imshow("former image", forImg);
        cv::imshow("latter image", latImg);
        cv::imshow("modified latter image", latImgMod);
        std::cout << i << "-th frame finished" << std::endl;
        
        cv::waitKey();
    }
}
