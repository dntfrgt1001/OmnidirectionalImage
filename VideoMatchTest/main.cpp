//
//  main.cpp
//  VideoMatchTest
//
//  Created by masakazu nakazawa on 2016/01/11.
//  Copyright © 2016年 masakazu. All rights reserved.
//

#include <iostream>

#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/features2d.hpp>

#include "Transform.hpp"
#include "ExtractFeaturePoint.hpp"
#include "MatchFeaturePoint.hpp"
#include "Rotation.hpp"
#include "MatchMain.hpp"
#include "VideoReader.hpp"
#include "VideoWriter.hpp"
#include "Estimate.hpp"
#include "Range.hpp"

#include "Epipolar.hpp"
#include "FeatureMatchEstimator.hpp"
#include "OpticalFlowEstimator.hpp"
#include "MainProcess.hpp"

int main(int argc, const char * argv[])
{
    // ffmpeg -f image2 -r 30 -i image%4d.jpg -pix_fmt yuv420p video.mp4
    
    const std::string path = "/Users/masakazu/Desktop/";
    const std::string inputVideoName = path + "02.mp4";
    const std::string outputVideoName = path + "02-1.mp4";
    
    const cv::Size fso(500, 250);
    const cv::Size fs(500, 250);
    
    int stride = 1;
    VideoReaderMov vr(fso, inputVideoName, stride);
    VideoWriterMov vw(fso, outputVideoName);
    
    const Transform tfo(fso);
    const Transform tf(fs);
    
    const int numThre = 10;
    const Epipolar epi(numThre);
    // -----------------------------------------------------
    // 特徴点マッチ用
    const int divNum = 6;
    const ExtractFeaturePoint efp(fs, tf, divNum);
    
    const float distThre = 200;
    const float coordThre = 0.4;
    const MatchFeaturePoint mfp(tf, distThre, coordThre);
    
    const float fieldAngle = M_PI / 3.0;
    const Range ran(fs, tf, fieldAngle);
    
    FeatureMatchEstimator fme(tf, efp, mfp, epi, ran);
    // -----------------------------------------------------
    // オプティカルフロー用
    const int persRad = 150;
    const float ranAng = M_PI / 4.0;
    const Perspective per(tf, persRad, ranAng);
    
    const float margin = 0.1;
    const float angRag = M_PI / 4.0;
    const float normRat = 3.0;
    const CalcOpticalFlow cof(margin, per, angRag, normRat);

    OpticalFlowEstimator ofe(tf, cof, per, epi);
    // -----------------------------------------------------

    MainProcess mp(tfo, fme, ofe);
    
    mp.modVideo(vr, vw);
    
    cv::Mat curRotMat = (cv::Mat_<float>(3,3)
                         << -0.39498305, -0.34497485, 0.85145819,
                            -0.64893043, 0.76081359, 0.0072172284,
                            -0.65029073, -0.54968637, -0.52437317);
    
    //VideoReaderPic vrPreFixed(frameSizeOriginal, inputVideoNamePreFixed, stride);
    //mm.ModifyVideoMid(vr, vrPreFixed, vw, 30, curRotMat);
    
    return 0;
}
