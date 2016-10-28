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
    cv::Mat estRotMat = fme.getRotMat(forImg, latImg);
    
    tf.rotateImg(latImg, modLatImg, estRotMat);
}

void MainProcess::modifyLatImgOpticalFlow
(const cv::Mat &forImg, const cv::Mat &latImg, cv::Mat &modLatImg)
{
    cv::Mat froChgMat = (cv::Mat_<float>(3,3) << 0,0,1,0,1,0,-1,0,0);
    
    cv::Mat estRotMat = ofe.getRotMat(forImg, latImg, froChgMat);
    
    tf.rotateImg(latImg, modLatImg, estRotMat);
}
