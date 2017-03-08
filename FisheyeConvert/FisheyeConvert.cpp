//
//  FisheyeConvert.cpp
//  OmnidirectionalImage
//
//  Created by masakazu nakazawa on 2017/02/03.
//  Copyright © 2017年 masakazu. All rights reserved.
//

#include "FisheyeConvert.hpp"

FisheyeConvert::FisheyeConvert
(const cv::Size& fsFish, const int radFish, const float fov,
 const float radius, const float focus):
radius(radius), focus(focus)
{
    // マージンに関する計算
    const float mergin = fsFish.height / 2 - radFish;
    const int fovMergin = radFish - radius;
    
    rectLeft = cv::Rect(fovMergin, mergin + fovMergin,
                        2 * radius, 2 * radius);
    rectRight = cv::Rect(fovMergin + fsFish.width/2, mergin + fovMergin,
                         2 * radius, 2 * radius);
}



void FisheyeConvert::convertVideo(VideoReader &vr, VideoWriter &vw) const
{
    // ウィンドウ生成
    cv::namedWindow("fish");
    cv::namedWindow("equi");
    
    // 繰り返し処理
    int count = 0;
    while (vr.hasNext()) {
        // 魚眼画像を入力
        cv::Mat fishImg;
        vr.readImg(fishImg);

        // 全天球画像に変換
        cv::Mat equiImg;
        convertImg(fishImg, equiImg);
        vw.writeImg(equiImg);

        std::cout << count++ << "-th frame finished." << std::endl;
        
        cv::imshow("fish", fishImg);
        cv::imshow("equi", equiImg);
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
    
    /*
    cv::namedWindow("fish rect left img");
    cv::namedWindow("fish rect right img");
    cv::imshow("fish rect left img", fishImg(rectLeft));
    cv::imshow("fish rect right img", fishImg(rectRight));
    cv::waitKey(10);
    */
    
    // 大きさの基準
    const int widthStd = Map::fsProc.width/4;
    const int heightStd = Map::fsProc.height;
    
    // 出力画像生成
    equiImg = cv::Mat(Map::fsProc, CV_8UC3);
    
    // コピーのためのROI
    cv::Rect rectl(0, 0, widthStd, heightStd);
    cv::Rect rectcl(widthStd, 0, widthStd, heightStd);
    cv::Rect rectcr(widthStd*2, 0, widthStd, heightStd);
    cv::Rect rectr(widthStd*3, 0, widthStd, heightStd);
  
    // 画像のコピー
    equiLeftImg(rectcl).copyTo(equiImg(rectcl));
    equiLeftImg(rectcr).copyTo(equiImg(rectcr));
    equiRightImg(rectcl).copyTo(equiImg(rectr));
    equiRightImg(rectcr).copyTo(equiImg(rectl));
}

void FisheyeConvert::convertSingleImg
(const cv::Mat &sinFishImg, cv::Mat &sinEquiImg) const
{
    // 半天球画像の生成
    sinEquiImg = cv::Mat::zeros(Map::fsProc, CV_8UC3);
    
    // 各画素の値を計算
    for (int v = 0; v < Map::fsProc.height; v++) {
        for (int u = Map::fsProc.width/4; u < Map::fsProc.width * 3.0/4.0; u++) {
            // 魚眼画像座標の計算
            cv::Point2f fish = equirect2fish(Equirect(u,v));
            
            // 画素の値の計算
            sinEquiImg.at<cv::Vec3b>(v,u) =
            Map::getBiliPixel<cv::Vec3b>(sinFishImg, fish);
        }
    }
}



FisheyeConvertEquiDist::FisheyeConvertEquiDist
(const cv::Size& fsFish, const int radFish, const float fov):
FisheyeConvert(fsFish, radFish, fov, radFish*M_PI_2/fov, radFish/fov)
{
    
}

cv::Point2f FisheyeConvertEquiDist::equirect2fish(const Equirect &equi) const
{
    // 魚眼画像用極座標
    cv::Point2f fishPolar = equirect2FishPolar(equi);
    
    const float chi = fishPolar.x;
    const float psi = fishPolar.y;
    
    const float r = focus * chi;
    
    // 魚眼画像を返す
    return cv::Point2f
            (Map::normalDomain(r*cosf(psi) + radius, 0, radius*2),
             Map::normalDomain(r*sinf(psi) + radius, 0, radius*2));
}

FisheyeConvertStereGraph::FisheyeConvertStereGraph
(const cv::Size& fsFish, const int radFish, const float fov):
FisheyeConvert(fsFish, radFish, fov, radFish*1.0/1.0913, radFish*1.0/1.0913/2)
{
    
}

cv::Point2f FisheyeConvertStereGraph::equirect2fish(const Equirect &equi) const
{
    // 魚眼画像用極座標
    cv::Point2f fishPolar = equirect2FishPolar(equi);
    
    const float chi = fishPolar.x;
    const float psi = fishPolar.y;
    
    const float r = 2 * focus * tanf(chi / 2.0);
    
    return cv::Point2f
            (Map::normalDomain(r*cosf(psi) + radius, 0, radius*2),
             Map::normalDomain(r*sinf(psi) + radius, 0, radius*2));
}


FisheyeConvertEquiSolid::FisheyeConvertEquiSolid
(const cv::Size& fsFish, const int radFish, const float fov):
FisheyeConvert(fsFish, radFish, fov, radFish*1.0/1.04266, radFish*1.0/1.04266/2)
{
    
}

cv::Point2f FisheyeConvertEquiSolid::equirect2fish(const Equirect &equi) const
{
    cv::Point2f fishPolar = equirect2FishPolar(equi);
    
    const float chi = fishPolar.x;
    const float psi = fishPolar.y;
    
    const float r = 2 * focus * sinf(chi / 2.0);
    
    return cv::Point2f
            (Map::normalDomain(r*cosf(psi) + radius, 0, radius*2),
             Map::normalDomain(r*sinf(psi) + radius, 0, radius*2));
}

cv::Point2f FisheyeConvert::equirect2FishPolar(const Equirect &equi) const
{
    
    // カメラ座標に変換
    Sphere sphere = Map::equirect2sphere(equi);
    
    // Z軸の正の向きとのなす角
    const float chi =
    atan2f(sqrtf(sphere.x*sphere.x+sphere.y*sphere.y), sphere.z);
    
    // X軸の正の向きとのなす角
    const float psi = atan2f(sphere.y, sphere.x);
    
    return cv::Point2f(chi, psi);
}



