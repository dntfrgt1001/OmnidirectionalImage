//
//  MainProcess.cpp
//  OmnidirectionalImage
//
//  Created by masakazu nakazawa on 2016/10/27.
//  Copyright © 2016年 masakazu. All rights reserved.
//

#include "MainProcess.hpp"

void MainProcess::printMatInfo()
{
    cv::Mat froChgMat = Rotation::getFroChgMat(curRotMat);
    cv::Vec3f rotVec = Rotation::RotMat2RotVec(curRotMat);
    
    std::cout <<  "------------------------------------------------";
    std::cout << std::endl;
    std::cout << "Rot Mat = " << std::endl << curRotMat << std::endl;
//    std::cout <<  "------------------------------------------------";
//    std::cout << std::endl;
    std::cout << "Rot Vec = " << rotVec;
    std::cout << ", norm = " << cv::norm(rotVec) << std::endl;
    
//    std::cout << std::endl;
    std::cout <<  "------------------------------------------------";
    std::cout << std::endl;
    std::cout << "Acc Mat = " << std::endl << accRotMat << std::endl;
    std::cout <<  "------------------------------------------------";
    std::cout << std::endl;
    std::cout << "Front Mat = " << std::endl << froChgMat << std::endl;
    std::cout <<  "------------------------------------------------";
    std::cout << std::endl;
}


void MainProcess::modImg
(const cv::Mat &forImg, const cv::Mat &latImg, cv::Mat &latImgMod)
{
//    struct timeval estStart, estEnd;
//    gettimeofday(&estStart, NULL);
    
    const State curState = {frameNum, curRotMat};
    curRotMat = est.getRotMat(forImg, latImg, curState);
    
//    gettimeofday(&estEnd, NULL);
//    double estTime = (double) (estEnd.tv_sec - estStart.tv_sec) +
//                     (estEnd.tv_usec - estStart.tv_usec) * 1e-6;
//    std::cout << "est time = " << estTime << std::endl;
    
//    struct timeval rotStart, rotEnd;
//    gettimeofday(&rotStart, NULL);
    
    accRotMat = accRotMat * curRotMat;
    tf.rotateImg(latImg, latImgMod, accRotMat);
    
//    gettimeofday(&rotEnd, NULL);
//    double rotTime = (double) (rotEnd.tv_sec - rotStart.tv_sec) +
//                     (rotEnd.tv_usec - rotStart.tv_usec) * 1e-6;
//    std::cout << "rot time = " << rotTime << std::endl;
}

void MainProcess::modVideo(VideoReader &vr, VideoWriter &vw)
{
    // 前方画像と後方画像
    cv::Mat forImg, latImg;
    
    // 初期フレーム入力と出力
    vr.readImg(latImg);
    vw.writeImg(latImg);
    std::cout << "0 th frame finished" << std::endl;
    frameNum ++;
    
    // ウィンドウ生成
    cv::namedWindow("former image");
    cv::namedWindow("latter image");
    cv::namedWindow("modified latter image");
    
    // 繰り返し処理
    while (vr.hasNext()) {
//        struct timeval frmStart, frmEnd;
//        gettimeofday(&frmStart, NULL);
        
        // フレームをずらす
        forImg = latImg.clone();
        vr.readImg(latImg);
        
        // 後方フレームの修正と出力
        cv::Mat latImgMod;
        modImg(forImg, latImg, latImgMod);
        vw.writeImg(latImgMod);
        
        // フレーム番号を表示して更新
        std::cout << frameNum << " th frame finished" << std::endl;
        frameNum ++;
        
        // 現在のフレームを表示
        cv::imshow("former image", forImg);
        cv::imshow("latter image", latImg);
        cv::imshow("modified latter image", latImgMod);
        
//        gettimeofday(&frmEnd, NULL);
//        double frmTime = (double) (frmEnd.tv_sec - frmStart.tv_sec) +
//                         (frmEnd.tv_usec - frmStart.tv_usec) * 1e-6;
//        std::cout << "frm time = " << frmTime << std::endl;
        
        cv::waitKey(1000);
    }
}


void MainProcess::modVideo(VideoReader &vr, VideoWriter &vw, Estimator &estSub)
{
    // 前方画像と後方画像
    cv::Mat forImg, latImg;
    
    // 初期フレーム入力と出力
    vr.readImg(latImg);
    vw.writeImg(latImg);
    std::cout << "0 th frame finished" << std::endl;
    frameNum ++;
    
    // ウィンドウ生成
    cv::namedWindow("former image");
    cv::namedWindow("latter image");
    cv::namedWindow("modified latter image");
    
    // 繰り返し処理
    while (vr.hasNext()) {
        struct timeval frmStart, frmEnd;
        gettimeofday(&frmStart, NULL);
        
        // フレームをずらす
        forImg = latImg.clone();
        vr.readImg(latImg);
        
        // 後方フレームの修正と出力
        cv::Mat latImgMod;
        
        float normRotVec = cv::norm(Rotation::RotMat2RotVec(curRotMat));
        float normRotThre = 0.2;
        if (normRotThre < normRotVec) {
            std::cout << "OpticalFlow Method" << std::endl;
            const State curState = {frameNum, curRotMat};
            curRotMat = est.getRotMat(forImg, latImg, curState);
            
        } else {
            std::cout << "Feature Matching Method" << std::endl;
            const State curState = {frameNum, curRotMat};
            curRotMat = estSub.getRotMat(forImg, latImg, curState);
        }
        accRotMat = accRotMat * curRotMat;
        tf.rotateImg(latImg, latImgMod, accRotMat);
        
        
        vw.writeImg(latImgMod);
        
        // 現在のフレームを表示
        cv::imshow("former image", forImg);
        cv::imshow("latter image", latImg);
        cv::imshow("modified latter image", latImgMod);
        
        // フレーム番号を更新して終了
        std::cout << frameNum << " th frame finished" << std::endl;
        frameNum ++;
        
        gettimeofday(&frmEnd, NULL);
        double frmTime = (double) (frmEnd.tv_sec - frmStart.tv_sec) +
        (frmEnd.tv_usec - frmStart.tv_usec) * 1e-6;
        std::cout << "frm time = " << frmTime << std::endl;
        
        cv::waitKey(1000);
        
    }
}

/*
void MainProcess::modVideo(VideoReader &vr, VideoWriter &vw)
{
    cv::Mat forImg, latImg;
    
    cv::namedWindow("former image");
    cv::namedWindow("latter image");
    cv::namedWindow("modified latter image");
    
    // 初期フレームを書き出す
    vr.readImg(latImg);
    vw.writeImg(latImg);
    
    std::cout << "0-th frame finished" << std::endl;
    
    for (int i = 1; vr.hasNext(); i++) {
        // 画像をシフト
        forImg = latImg.clone();
        vr.readImg(latImg);
        
       
        
        // 後画像を修正
        cv::Mat latImgMod;
 //       modifyLatImgJackInHead(forImg, latImg, latImgMod);
 //       modifyLatImgFeatureMatch(forImg, latImg, latImgMod);
        
//        modifyLatImgOpticalFlow(forImg, latImg, latImgMod);
        
        
        float normRotVec = cv::norm(Rotation::RotMat2RotVec(curRotMat));
        float normRotThre = 0.2;
        if (normRotThre < normRotVec) {
            std::cout << "OpticalFlow Method" << std::endl;
            modImgOpticalFlow(forImg, latImg, latImgMod);
        } else {
            std::cout << "Feature Matching Method" << std::endl;
            modImgFeatureMatch(forImg, latImg, latImgMod);
        }
        
        // modifyLatImgFeatureMatch(forImg, latImg, latImgMod);
        // modifyLatImgOpticalFlow(forImg, latImg, latImgMod);
        
        vw.writeImg(latImgMod);
        
        // 出力
        cv::imshow("former image", forImg);
        cv::imshow("latter image", latImg);
        cv::imshow("modified latter image", latImgMod);
        
        std::cout << i << "-th frame finished" << std::endl;
        
        cv::waitKey(0);
    }
}
*/

