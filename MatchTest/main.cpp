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

//#include "Transform.hpp"
#include "Rotation.hpp"
#include "ExtractFeaturePoint.hpp"
#include "MatchFeaturePoint.hpp"
#include "Quaternion.hpp"
#include "Range.hpp"
#include "Perspective.hpp"
#include "MainProcess.hpp"
#include "FeatureMatchEstimator.hpp"
//#include "OpticalFlowEstimator.hpp"
#include "CalcOpticalFlow.hpp"
#include "Epipolar.hpp"

#include "JackInHeadEstimator.hpp"
#include "Core.hpp"

int main(int argc, const char * argv[])
{    
    const std::string path = "/Users/masakazu/Desktop/TestImage/";
    const std::string inputName1 = path + "high-speed1.jpg";
    const std::string inputName2 = path + "high-speed2.jpg";

    const cv::Size fsOut(1280, 640);
    const cv::Size fsProc(960, 480);
//    const cv::Size fs(1000, 500);
    
    cv::Mat input1, img1, input2, img2;
    input1 = cv::imread(inputName1);
    cv::resize(input1, img1, fsOut);
    input2 = cv::imread(inputName2);
    cv::resize(input2, img2, fsOut);

    // フレームサイズをセット
    Map::fsProc = fsProc;
    Map::fsOut = fsOut;
    // 画像処理用のフレームサイズをセット
    Map::setProcSize();
    
    const int numThre = 10;
    const Epipolar epi(numThre);
    // -----------------------------------------------------
    // 特徴点マッチング用
    const int divNum = 6;
    const int mergin = 10;
    ExtractFeaturePoint efp(fsProc, divNum, mergin);
    
    const float eucThre = 200;
    const float sphereThre = M_PI_2;
    MatchFeaturePoint mfp(eucThre, sphereThre);
    
    const float fovAngle = M_PI / 4.0;
    const Range ran(fovAngle);
    
    FeatureMatchEstimator fme(efp, mfp, epi, ran);
    // -----------------------------------------------------
    // オプティカルフロー用
    const int persRad = 200;
    const float ranAng = M_PI / 4.0;
    const Perspective per(persRad, ranAng);
    
    const float margin = 0.1;
    const float angRag = M_PI / 4.0;
    const float normRat = 3.0;
    const CalcOpticalFlow cof(margin, per, angRag, normRat);
    
    OpticalFlowEstimator ofe(cof, per, epi);
    // -----------------------------------------------------
    // 笠原の手法
    JackInHeadEstimator jihe;
    // -----------------------------------------------------
    
    MainProcess mp(fme);
    cv::Mat curRotMat = (cv::Mat_<float>(3,3) << 1,0,0, 0,0,1, 0,-1,0);
    mp.curRotMat = curRotMat;
    
    cv::Mat modImg2;
    mp.modImg(img1, img2, modImg2);
    
    cv::namedWindow("img1");
    cv::namedWindow("img2");
    cv::namedWindow("mod img2");
    
    cv::imshow("img1", img1);
    cv::imshow("img2", img2);
    cv::imshow("mod img2", modImg2);
    
    cv::waitKey();
    
    return 0;
}
