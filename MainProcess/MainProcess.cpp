//
//  MainProcess.cpp
//  OmnidirectionalImage
//
//  Created by masakazu nakazawa on 2016/10/27.
//  Copyright © 2016年 masakazu. All rights reserved.
//

#include "MainProcess.hpp"

void MainProcess::modifyLatImg
(const cv::Mat &forImg, const cv::Mat &latImg, cv::Mat &modLatImg)
{
    cv::Mat estRotMat = fme.estRotMat(forImg, latImg);
    
    tf.rotateImg(latImg, modLatImg, estRotMat);
    
    cv::namedWindow("previous");
    cv::namedWindow("next");
    cv::namedWindow("mod next");
    
    cv::imshow("previous", forImg);
    cv::imshow("next", latImg);
    cv::imshow("mod next", modLatImg);
    
    cv::waitKey();
}