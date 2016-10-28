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

    const cv::Size fso(800, 400);
    const cv::Size fs(800, 400);
    
    cv::Mat input1, img1, input2, img2;
    input1 = cv::imread(inputName1);
    cv::resize(input1, img1, fs);
    input2 = cv::imread(inputName2);
    cv::resize(input2, img2, fs);
    
    const Transform tfo(fso);
    const Transform tf(fs);
    
    // -----------------------------------------------------
    const int persRad = 200;
    const float ranAng = M_PI / 4.0;
    const Perspective per(tf, persRad, ranAng);
    
    const float margin = 0.1;
    const float angRag = M_PI / 6.0;
    const CalcOpticalFlow cof(margin, per, angRag);

    int numThre = 15;
    const Epipolar epi(numThre);

    const OpticalFlowEstimator ofe(tf, cof, per, epi);
    
    // -----------------------------------------------------
    
    const int divNum = 6;
    ExtractFeaturePoint efp(fs, tf, divNum);
    
    const float distThre = 200;
    const float coordThre = 0.4;
    MatchFeaturePoint mfp(fs, tf, distThre, coordThre);
    
    float fieldAngle = M_PI / 4.0;
    const Range ran(fs, tf, fieldAngle);
    
    const FeatureMatchEstimator fme(tf, efp, mfp, epi, ran);
    
    // -----------------------------------------------------
    
    MainProcess mp(tfo, fme, ofe);
    
    cv::Mat modImg2;
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
