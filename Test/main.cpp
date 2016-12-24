//
//  main.cpp
//  Test
//
//  Created by masakazu nakazawa on 2016/10/21.
//  Copyright © 2016年 masakazu. All rights reserved.
//

#include <iostream>

#include <opencv2/core.hpp>

#include "Transform.hpp"
#include "Perspective.hpp"
#include "OpticalFlow.hpp"
#include "Quaternion.hpp"
#include "Rotation.hpp"

static inline void drawCross(cv::Mat& img, cv::Point& center,
                             const cv::Scalar& color, const float d)
{
    cv::line(img, cv::Point( center.x - d, center.y - d ),
             cv::Point( center.x + d, center.y + d ), color, 1, cv::LINE_AA, 0);
    cv::line( img, cv::Point( center.x + d, center.y - d ),
             cv::Point( center.x - d, center.y + d ), color, 1, cv::LINE_AA, 0);
}


static inline cv::Point calcPoint(cv::Point2f& center, double R, double angle)
{
    return center + cv::Point2f((float)cos(angle), (float)-sin(angle))*(float)R;
}

int main(int argc, const char * argv[])
{
    cv::Mat img(500, 500, CV_8UC3);
    cv::KalmanFilter KF(2, 1, 0);
    cv::Mat state(2, 1, CV_32F); /* (phi, delta_phi) */
    cv::Mat processNoise(2, 1, CV_32F);
    cv::Mat measurement = cv::Mat::zeros(1, 1, CV_32F);
    char code = (char)-1;
    
    for(;;)
    {
        randn( state, cv::Scalar::all(0), cv::Scalar::all(0.1) );
        KF.transitionMatrix = (cv::Mat_<float>(2, 2) << 1, 1, 0, 1);
        
        setIdentity(KF.measurementMatrix);
        setIdentity(KF.processNoiseCov, cv::Scalar::all(1e-5));
        setIdentity(KF.measurementNoiseCov, cv::Scalar::all(1e-1));
        setIdentity(KF.errorCovPost, cv::Scalar::all(1));
        
        randn(KF.statePost, cv::Scalar::all(0), cv::Scalar::all(0.1));
        
        for(;;)
        {
            cv::Point2f center(img.cols*0.5f, img.rows*0.5f);
            float R = img.cols/3.f;
            double stateAngle = state.at<float>(0);
            cv::Point statePt = calcPoint(center, R, stateAngle);
            
            cv::Mat prediction = KF.predict();
            double predictAngle = prediction.at<float>(0);
            cv::Point predictPt = calcPoint(center, R, predictAngle);
            
            randn( measurement, cv::Scalar::all(0), cv::Scalar::all(KF.measurementNoiseCov.at<float>(0)));
            
            // generate measurement
            measurement += KF.measurementMatrix*state;
            
            double measAngle = measurement.at<float>(0);
            cv::Point measPt = calcPoint(center, R, measAngle);
            
            // plot points
            img = cv::Scalar::all(0);
            drawCross(img, statePt, cv::Scalar(255,255,255), 3 );
            drawCross(img, measPt, cv::Scalar(0,0,255), 3 );
            drawCross(img, predictPt, cv::Scalar(0,255,0), 3 );
            line( img, statePt, measPt, cv::Scalar(0,0,255), 3, cv::LINE_AA, 0 );
            line( img, statePt, predictPt, cv::Scalar(0,255,255), 3, cv::LINE_AA, 0 );
            
            if(cv::theRNG().uniform(0,4) != 0)
                KF.correct(measurement);
            
            randn( processNoise, cv::Scalar(0), cv::Scalar::all(sqrt(KF.processNoiseCov.at<float>(0, 0))));
            state = KF.transitionMatrix*state + processNoise;
            
            imshow( "Kalman", img );
            code = (char)cv::waitKey(100);
            
            if( code > 0 )
                break;
        }
        if( code == 27 || code == 'q' || code == 'Q' )
            break;
    }

    
    /*
    // 回転ベクトル
    float angle = M_PI * 5.0/6.0;
    cv::Vec3f axisVec(1.0, 2.0, 1.0);
    cv::Vec3f rotVec = angle * axisVec / cv::norm(axisVec);
    std::cout << "rot vet = " << rotVec << std::endl;
    
    // 回転ベクトル->回転行列
    cv::Mat rotMat = Rotation::RotVec2RotMat(rotVec);
    std::cout << "rot mat = " << std::endl << rotMat << std::endl;
    
    cv::Vec3f rotVecRet = Rotation::RotMat2RotVec(rotMat);
    std::cout << "ret rot vet = " << rotVecRet << std::endl;
    
    // 回転行列->四元数
    Quaternion quat = Rotation::RotMat2Quat(rotMat);
    std::cout << "unit quat = " << quat << std::endl;
    
    // 四元数->回転行列
    cv::Mat rotMatRet = Rotation::Quat2RotMat(quat);
    std::cout << "ret rot mat = " << std::endl << rotMatRet << std::endl;
    
    const int loopCount = 100000;
    float array[loopCount];

    clock_t start = clock();
    
    for (int i = 0; i < loopCount; i++) {
        array[i] = 0;
    }
    
    for (int i = 0; i < loopCount; i++) {
        array[i] = array[i] + 1;
    }
    
    clock_t end = clock();
    
    std::cout << "time = " << (end - start) << " msec." << std::endl;
    
    
    float angle1 = M_PI * 5.0 / 6.0;
    cv::Vec3f axis1(1.0, 0.0, 0.0);
    Quaternion quat1(angle1, axis1);
    
    float angle2 = M_PI / 2.0;
    cv::Vec3f axis2(1.0, 0.0, 0.0);
    Quaternion quat2(angle2, axis2);
    
    std::cout << quat2 * quat1 << std::endl;
*/
    
    return 0;
}
