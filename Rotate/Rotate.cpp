//
//  Rotate.cpp
//  OmnidirectionalImage
//
//  Created by masakazu nakazawa on 2015/12/27.
//  Copyright © 2015年 masakazu. All rights reserved.
//

#include "Rotate.hpp"
#include <math.h>

Rotate::Rotate(const cv::Size* frameSize, cv::Mat* img):
frameSize(frameSize), img(img)
{
    setThetaScale();
}

Rotate::~Rotate(){
    
}

void Rotate::orth2ang(int u, int v, double &theta, double &phi)
{
    theta = (u - frameSize->width/2.0) * 2.0*M_PI/frameSize->width;
    phi = (-1.0) * (v - frameSize->height/2.0) * M_PI/frameSize->height;
}

void Rotate::ang2orth(double theta, double phi, int &u, int &v)
{
    u = (int) round(theta*frameSize->width/(2.0*M_PI) + frameSize->width/2.0);
    v = (int) round((-1.0)*phi*frameSize->height/M_PI + frameSize->height/2.0);
}

void Rotate::ang2orthd(double theta, double phi, double &u, double &v)
{
    u = theta*frameSize->width/(2.0*M_PI) + frameSize->width/2.0;
    v = (-1.0)*phi*frameSize->height/M_PI + frameSize->height/2.0;
}

void Rotate::rotateXOrthBilinearDot(double chi, int u, int v, cv::Vec3b& pixel)
{
    double theta, phi;
    double thetar, phir;
    
    orth2ang(u, v, theta, phi);
    
    rotateXAng(chi, theta, phi, thetar, phir);
    
    double ur, vr;
    
    ang2orthd(thetar, phir, ur, vr);
    
    pixel =
            (ceil(ur) - ur) * (ceil(vr) - vr) *
            img->at<cv::Vec3b>((int)ceil(vr), (int)ceil(ur)) +
            (ceil(ur) - ur) * (vr - floor(vr)) *
            img->at<cv::Vec3b>((int)floor(vr), (int)ceil(ur)) +
            (ur - floor(ur)) * (ceil(vr) - vr) *
            img->at<cv::Vec3b>((int)ceil(vr), (int)floor(ur)) +
            (ur - floor(ur)) * (vr - floor(vr)) *
            img->at<cv::Vec3b>((int)floor(vr), (int)floor(ur));
}

void Rotate::rotateXOrthNearDot(double chi, int u, int v, cv::Vec3b& pixel)
{
    double theta, phi;
    double thetar, phir;
    
    orth2ang(u, v, theta, phi);
    
    rotateXAng(chi, theta, phi, thetar, phir);
    
    int ur, vr;
    
    ang2orth(thetar, phir, ur, vr);
    
    pixel = img->at<cv::Vec3b>(vr, ur);
}

void Rotate::rotateXOrth(double chi, cv::Mat& rotImg)
{
    for (int u=0; u<frameSize->width; u++) {
        for (int v=0; v<frameSize->height; v++) {
            cv::Vec3b tmpPixel;
            
            rotateXOrthNearDot((-1.0) * chi, u, v, tmpPixel);
//            rotateXOrthBilinearDot((-1.0)*chi, u, v, tmpPixel);
            rotImg.at<cv::Vec3b>(v, u) = tmpPixel;
        }
    }
}

void Rotate::rotateYOrthDot(double chi, int u, int v, int &ur, int &vr)
{
    
}

void Rotate::rotateYOrth(double chi, cv::Mat &rotImg)
{
    int delta = (int) round(normalizeTheta(chi) * thetaScale);
    
    int rest = frameSize->width - delta;
    
    cv::Mat imgLeft(*img, cv::Rect(0, 0, rest, frameSize->height));
    cv::Mat imgRight(*img, cv::Rect(rest, 0, delta, frameSize->height));
                     
    cv::Mat rotLeft(rotImg, cv::Rect(0, 0, delta, frameSize->height));
    cv::Mat rotRight(rotImg, cv::Rect(delta, 0, rest, frameSize->height));
    
    imgLeft.copyTo(rotRight);
    imgRight.copyTo(rotLeft);
}

void Rotate::rotateXAng(double chi, double theta, double phi,
                        double& thetar, double& phir)
{
    thetar = atan(sin(theta)*cos(phi) /
                  (sin(chi)*sin(phi) + cos(chi)*cos(theta)*cos(phi)));
    phir = asin(cos(chi)*sin(phi)-sin(chi)*cos(theta)*cos(phi));
    
    if (0 <= theta && thetar < 0) thetar += M_PI;
    if (theta < 0 && 0 <= thetar) thetar -= M_PI;
}

double Rotate::normalizeTheta(double rawTheta)
{
    while (rawTheta < 0) rawTheta += 2.0*M_PI;
    
    while (2*M_PI < rawTheta) rawTheta -= 2.0*M_PI;
    
    return rawTheta;
}

void Rotate::setThetaScale()
{
    thetaScale = (frameSize->width / 2.0) / M_PI;
}

void Rotate::setImage(cv::Mat *newImg)
{
    img = newImg;
}

