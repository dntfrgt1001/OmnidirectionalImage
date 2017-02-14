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
#include "Core.hpp"

int main(int argc, const char * argv[])
{
 // ffmpeg -f image2 -r 30 -i image\%04d.jpg -an -vcodec libx264 -pix_fmt yuv420p video.mp4
    //    const std::string path = "/Users/masakazu/Desktop/casio/bowling/02/";
    const std::string path = "/Users/masakazu/Desktop/fishtest/";
    const std::string inputVideoName = path + "sample5-equi";
    const std::string outputVideoName = path + "sample5-1";
    
    const cv::Size fsOut(1280, 640);
    const cv::Size fsProc(1280, 640);
    
    int stride = 1;
    VideoReaderPic vr(fsOut, inputVideoName, stride);
    VideoWriterPic vw(fsOut, outputVideoName);
    
    Map::fsOut = fsOut;
    Map::fsProc = fsProc;
    Map::setProcSize();
    
    const int numThre = 10;
    const Epipolar epi(numThre);
    // -----------------------------------------------------
    // 特徴点マッチ用
    const int divNum = 6;
    const int mergin = 10;
    const ExtractFeaturePoint efp(fsProc, divNum, mergin);

    const float eucThre = 250;
    const float sphereThre = 0.35;
    const MatchFeaturePoint mfp(eucThre, sphereThre);
    
    const float fieldAngle = M_PI / 2.5;
    const Range ran(fieldAngle);
    
    FeatureMatchEstimator fme(efp, mfp, epi, ran);
    // -----------------------------------------------------
    // オプティカルフロー用
    const int persRad = 150;
    const float ranAng = M_PI / 4.0;
    const Perspective per(persRad, ranAng);
    
    const float margin = 0.05;
    const float angRag = M_PI / 4.0;
    const float normRat = 3.0;
    const CalcOpticalFlow cof(margin, per, angRag, normRat);

    OpticalFlowEstimator ofe(cof, per, epi);
    // -----------------------------------------------------

    JackInHeadEstimator jhe;
    // -----------------------------------------------------
    
    MainProcess mp(fme);
//    MainProcess mp(tfo, ofe);
    
    mp.modVideo(vr, vw);
//    mp.modVideo(vr, vw, fme);
    
    cv::Mat curRotMat = (cv::Mat_<float>(3,3)
                         << -0.39498305, -0.34497485, 0.85145819,
                            -0.64893043, 0.76081359, 0.0072172284,
                            -0.65029073, -0.54968637, -0.52437317);
    
    //VideoReaderPic vrPreFixed(frameSizeOriginal, inputVideoNamePreFixed, stride);
    //mm.ModifyVideoMid(vr, vrPreFixed, vw, 30, curRotMat);
    
    return 0;
}
