//
//  FisheyeConvert.cpp
//  OmnidirectionalImage
//
//  Created by masakazu nakazawa on 2017/02/03.
//  Copyright © 2017年 masakazu. All rights reserved.
//

#include "FisheyeConvert.hpp"

FisheyeConvert::FisheyeConvert
(const cv::Size& fsFish, const int radiusFish, const float fov):
fsFish(fsFish), mergin(fsFish.height / 2 - radiusFish),
radius(radiusFish * M_PI_2 / fov)
//fsEqui(cv::Size(radiusFish*M_PI_2/fov*4, radiusFish*M_PI_2/fov*2))
{
    const int fovDiff = radiusFish - radius;
    
    rectLeft = cv::Rect(fovDiff, mergin + fovDiff,
                        2 * radius, 2 * radius);
    rectRight = cv::Rect(fovDiff + fsFish.width/2, mergin + fovDiff,
                         2 * radius, 2 * radius);

    /*
    cv::Mat test = cv::Mat::zeros(fs, CV_8UC1);
    cv::Mat fishLeft = test(rectLeft);
    cv::Mat fishRight= test(rectRight);

    fishLeft = cv::Mat::ones(fishLeft.size(), CV_8UC1)*256;
    fishRight = cv::Mat::ones(fishRight.size(), CV_8UC1)*256;
    cv::namedWindow("test");
    cv::imshow("test", test);
    cv::waitKey();
     */
}



void FisheyeConvert::convertVideo(VideoReader &vr, VideoWriter &vw) const
{
    cv::namedWindow("fish img");
    cv::namedWindow("equi img");
    
    while (vr.hasNext()) {
        cv::Mat fishImg;
        vr.readImg(fishImg);
        
        cv::Mat equiImg;
        convertImg(fishImg, equiImg);
        vw.writeImg(equiImg);
        
        cv::imshow("fish img", fishImg);
        cv::imshow("equi img", equiImg);
        
        cv::waitKey(10);
    }
}

void FisheyeConvert::convertImg
(const cv::Mat &fishImg, cv::Mat &equiImg) const
{
    // 左側の魚眼画像
    cv::Mat equiLeftImg;
    convertSingleImg(fishImg(rectLeft), equiLeftImg);
    
    // 右側の魚眼画像
    cv::Mat equiRightImg;
    convertSingleImg(fishImg(rectRight), equiRightImg);
    
    // 大きさの基準
    const int widthStd = Map::fsProc.width/4;
    const int heightStd = Map::fsProc.height;
    
    equiImg = cv::Mat(Map::fsProc, CV_8UC3);
//    cv::Rect centerRect(fsEqui.width/4, 0, fsEqui.width/2, fsEqui.height);
//    cv::Rect leftRect(0, 0, fsEqui.width/4, fsEqui.height);
//    cv::Rect rightRect((int)fsEqui.width*3.0/4.0, 0, fsEqui.width/4, fsEqui.height);
    
    cv::Rect rectl(0, 0, widthStd, heightStd);
    cv::Rect rectcl(widthStd, 0, widthStd, heightStd);
    cv::Rect rectcr(widthStd*2, 0, widthStd, heightStd);
    cv::Rect rectr(widthStd*3, 0, widthStd, heightStd);
  
    equiLeftImg(rectcl).copyTo(equiImg(rectcl));
    equiLeftImg(rectcr).copyTo(equiImg(rectcr));
    equiRightImg(rectcl).copyTo(equiImg(rectr));
    equiRightImg(rectcr).copyTo(equiImg(rectl));
    
//    equiLeftImg(centerRect).copyTo(equiImg(centerRect));
//    equiRightImg(leftRect).copyTo(equiImg(rightRect));
//    equiRightImg(rightRect).copyTo(equiImg(leftRect));
}

void FisheyeConvert::convertSingleImg
(const cv::Mat &sinFishImg, cv::Mat &sinEquiImg) const
{
    // 焦点距離
    const float f = radius / M_PI_2;
    
    sinEquiImg = cv::Mat::zeros(Map::fsProc, CV_8UC3);
    
    for (int v = 0; v < Map::fsProc.height; v++) {
        for (int u = Map::fsProc.width/4; u < Map::fsProc.width * 3.0/4.0; u++) {
            // 経緯度に変換
            Polar polar = Map::equirect2polar(Equirect(u, v));
            
            // Z軸の正の向きとのなす角
//            float achi = cosf(polar.x) * cosf(polar.y);
//            float chi = acosf(achi<-1? -1: 1<achi? 1: achi);
            float chi =
            acosf(Map::normalDomain(cosf(polar.x)*cosf(polar.y), -1, 1));
            
            // X軸の正の向きとのなす角
            float psi = (polar.x==0)? M_PI_2:
                                      atanf(-tanf(polar.y)/sinf(polar.x));
            if (polar.x < 0) {
                if (psi > 0) psi = psi - M_PI;
                else psi = psi + M_PI;
            }

            
            // 魚眼画像の座標
            int uf =
            Map::normalDomain((int) (f*chi*cosf(psi) + radius), 0, radius*2);
            int vf =
            Map::normalDomain((int) (f*chi*sinf(psi) + radius), 0, radius*2);
            
            sinEquiImg.at<cv::Vec3b>(v,u) =
            Map::getBiliPixel<cv::Vec3b>(sinFishImg, Equirect(uf, vf));
            
        }
    }
}