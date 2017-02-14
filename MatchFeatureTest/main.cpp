//
//  main.cpp
//  MatchFeatureTest
//
//  Created by masakazu nakazawa on 2016/12/05.
//  Copyright © 2016年 masakazu. All rights reserved.
//

#include <iostream>

#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>

#include "Transform.hpp"
#include "ExtractFeaturePoint.hpp"
#include "MatchFeaturePoint.hpp"

int main(int argc, const char * argv[])
{
    const std::string path = "/Users/masakazu/Desktop/TestImage/";
    const std::string inputName1 = path + "image0025.jpg";
    const std::string inputName2 = path + "image0026.jpg";
    
    const cv::Size fs(960, 480);

    cv::Mat input1, img1, input2, img2;
    input1 = cv::imread(inputName1);
    cv::resize(input1, img1, fs);
    input2 = cv::imread(inputName2);
    cv::resize(input2, img2, fs);
    
//    const Transform tf(fs);
    
    const int divNum = 6;
    const int mergin = 10;
    ExtractFeaturePoint efp(fs, divNum, mergin);
    
    const float eucThre = 250;
    const float sphereThre = 0.3;
    MatchFeaturePoint mfp(eucThre, sphereThre);
    
    cv::Mat grayImg1, grayImg2;
    cv::cvtColor(img1, grayImg1, CV_BGR2GRAY);
    cv::cvtColor(img2, grayImg2, CV_BGR2GRAY);

    // 特徴抽出
    std::vector<cv::KeyPoint> keyPoints1, keyPoints2;
    cv::Mat descriptor1, descriptor2;
    efp.extFeat(grayImg1, keyPoints1, descriptor1);
    efp.extFeat(grayImg2, keyPoints2, descriptor2);
    
    cv::Mat drawKeyImg1, drawKeyImg2;
    efp.drawKeyPointClear(grayImg1, keyPoints1, drawKeyImg1);
    efp.drawKeyPointClear(grayImg2, keyPoints2, drawKeyImg2);
    
    // 特徴マッチ
    std::vector<cv::DMatch> matchs;
    mfp.match(descriptor1, descriptor2, matchs);
    
    mfp.filterMatchEuc(matchs);
    
    // マッチした特徴点
    mfp.orderKeyPoint(keyPoints1, keyPoints2, matchs);
    
    // 画像座標に変換
    std::vector<Equirect> equirects1(keyPoints1.size());
    std::vector<Equirect> equirects2(keyPoints2.size());
    for (int i = 0; i < keyPoints1.size(); i++) {
        equirects1[i] = keyPoints1[i].pt;
        equirects2[i] = keyPoints2[i].pt;
    }

    // 球面座標に変換
    std::vector<Sphere> spheres1, spheres2;
    Map::equirect2sphere(equirects1, spheres1);
    Map::equirect2sphere(equirects2, spheres2);
    
    // 幾何学距離でフィルタ
    mfp.filterMatchSphere(spheres1, spheres2);
    
    std::vector<Equirect> equirectsLast1, equirectsLast2;
    Map::sphere2equirect(spheres1, equirectsLast1);
    Map::sphere2equirect(spheres2, equirectsLast2);
    
    cv::Mat drawMatchImg;
    mfp.drawMatchVert(grayImg1, equirectsLast1, grayImg2, equirectsLast2, drawMatchImg);
//    mfp.drawMatchVert(grayImg1, KeyPointSet1, grayImg2, KeyPointSet2, matchs, drawMatchImg);
    
    cv::namedWindow("key point 1");
    cv::imshow("key point 1", drawKeyImg1);
    cv::namedWindow("key point 2");
    cv::imshow("key point 2", drawKeyImg2);
    
    cv::namedWindow("match img");
    cv::imshow("match img", drawMatchImg);
    
    cv::waitKey();
    
    cv::imwrite(path + "drawImg1.jpg", drawKeyImg1);
    cv::imwrite(path + "drawImg2.jpg", drawKeyImg2);
    cv::imwrite(path + "matchImg.jpg", drawMatchImg);
    
    return 0;
}
