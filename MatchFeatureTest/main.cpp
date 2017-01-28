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
    
    const Transform tf(fs);
    
    const int divNum = 6;
    ExtractFeaturePoint efp(fs, tf, divNum);
    
    const float distThre = 200;
    const float coordThre = 0.3;
    MatchFeaturePoint mfp(tf, distThre, coordThre);
    
    cv::Mat grayImg1, grayImg2;
    cv::cvtColor(img1, grayImg1, CV_BGR2GRAY);
    cv::cvtColor(img2, grayImg2, CV_BGR2GRAY);

    // 特徴抽出
    std::vector<cv::KeyPoint> KeyPointSet1, KeyPointSet2;
    cv::Mat descriptor1, descriptor2;
    efp.extFeat(grayImg1, KeyPointSet1, descriptor1);
    efp.extFeat(grayImg2, KeyPointSet2, descriptor2);
    
    cv::Mat drawKeyImg1, drawKeyImg2;
    efp.drawKeyPointClear(grayImg1, KeyPointSet1, drawKeyImg1);
    efp.drawKeyPointClear(grayImg2, KeyPointSet2, drawKeyImg2);
    
    // 特徴マッチ
    std::vector<cv::DMatch> matchs;
    mfp.match(descriptor1, descriptor2, matchs);
    
    mfp.filterMatchDistance(matchs);
    
    // マッチした特徴点
    std::vector<cv::KeyPoint> keyPoints1, keyPoints2;
    mfp.getMatchKeyPoint(KeyPointSet1, KeyPointSet2, matchs, keyPoints1, keyPoints2);
    
    // 画像座標に変換
    std::vector<Equirect> equirects1(keyPoints1.size());
    std::vector<Equirect> equirects2(keyPoints2.size());
    for (int i = 0; i < keyPoints1.size(); i++) {
        equirects1[i] = keyPoints1[i].pt;
        equirects2[i] = keyPoints2[i].pt;
    }

    // 球面座標に変換
    std::vector<Sphere> spheres1, spheres2;
    tf.equirect2sphere(equirects1, spheres1);
    tf.equirect2sphere(equirects2, spheres2);
    
    // 幾何学距離でフィルタ
    mfp.filterCoordDistance(spheres1, spheres2);
    
    std::vector<Equirect> equirectsLast1, equirectsLast2;
    tf.sphere2equirect(spheres1, equirectsLast1);
    tf.sphere2equirect(spheres2, equirectsLast2);
    
    
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
