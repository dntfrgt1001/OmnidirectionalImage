//
//  main.cpp
//  MatchTest
//
//  Created by masakazu nakazawa on 2016/01/09.
//  Copyright © 2016年 masakazu. All rights reserved.
//

#include <iostream>

#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/features2d.hpp>
#include <opencv2/xfeatures2d.hpp>
#include <opencv2/calib3d.hpp>

#include "Transform.hpp"
#include "Rotation.hpp"
#include "ExtractFeaturePoint.hpp"
#include "MatchFeaturePoint.hpp"
#include "Quaternion.hpp"
#include "Range.hpp"
#include "Perspective.hpp"
#include "MainProcess.hpp"
#include "FeatureMatchEstimator.hpp"
#include "OpticalFlowEstimator.hpp"
#include "CalcOpticalFlow.hpp"
#include "Epipolar.hpp"

int main(int argc, const char * argv[])
{
    const std::string path = "/Users/masakazu/Desktop/";
    const std::string inputName1 = path + "image1.jpg";
    const std::string inputName2 = path + "image2.jpg";

    const cv::Size fso(960, 480);
    const cv::Size fs(960, 480);
    
    cv::Mat input1, img1, input2, img2;
    input1 = cv::imread(inputName1);
    cv::resize(input1, img1, fso);
    input2 = cv::imread(inputName2);
    cv::resize(input2, img2, fso);
    
    const Transform tfo(fso);
    const Transform tf(fs);
    
    const int numThre = 15;
    const Epipolar epi(numThre);
    // -----------------------------------------------------
    // 特徴点マッチング用
    const int divNum = 6;
    ExtractFeaturePoint efp(fs, tf, divNum);
    
    const float distThre = 200;
    const float coordThre = 0.4;
    MatchFeaturePoint mfp(tf, distThre, coordThre);
    
    const float fieldAngle = M_PI / 4.0;
    const Range ran(fs, tf, fieldAngle);
    
    const FeatureMatchEstimator fme(tf, efp, mfp, epi, ran);
    // -----------------------------------------------------
    // オプティカルフロー用
    const int persRad = 150;
    const float ranAng = M_PI / 4.0;
    const Perspective per(tf, persRad, ranAng);
    
    const float margin = 0.1;
    const float angRag = M_PI / 3.0;
    const float normRat = 3.0;
    const CalcOpticalFlow cof(margin, per, angRag, normRat);
    
    const OpticalFlowEstimator ofe(tf, cof, per, epi);
    // -----------------------------------------------------
    
    MainProcess mp(tfo, fme, ofe);
    
    cv::Mat modImg2;
    //mp.modifyLatImgFeatureMatch(img1, img2, modImg2);
    mp.modifyLatImgOpticalFlow(img1, img2, modImg2);
    
    cv::namedWindow("img1");
    cv::namedWindow("img2");
    cv::namedWindow("mod img2");
    
    cv::imshow("img1", img1);
    cv::imshow("img2", img2);
    cv::imshow("mod img2", modImg2);
    
    cv::waitKey();
    
    return 0;
}
