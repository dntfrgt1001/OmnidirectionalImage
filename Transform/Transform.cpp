//
//  Transform.cpp
//  OmnidirectionalImage
//
//  Created by masakazu nakazawa on 2016/01/10.
//  Copyright © 2016年 masakazu. All rights reserved.
//

#include "Transform.hpp"

Transform::Transform(const cv::Size& frameSize):
frameSize(frameSize)
{
}

int Transform::theta2u(double theta) const
{
    return (int)round(theta*frameSize.width/(2.0*M_PI) + frameSize.width/2.0);
}

int Transform::phi2v(double phi) const
{
    return (int)round((-1.0)*phi*frameSize.height/M_PI + frameSize.height/2.0);
}

double Transform::u2theta(int u) const
{
    return (u - frameSize.width/2.0) * 2.0*M_PI/frameSize.width;
}

double Transform::v2phi(int v) const
{
    return (-1.0) * (v - frameSize.height/2.0) * M_PI/frameSize.height;
}

void Transform::ang2orth(double theta, double phi, int &u, int &v) const
{
    u = theta2u(theta);
    v = phi2v(phi);
}

void Transform::orth2ang(int u, int v, double &theta, double &phi) const
{
    theta = u2theta(u);
    phi = v2phi(v);
}

int Transform::dtheta2u(double theta) const
{
    return (int) round(theta * frameSize.width / (2.0 * M_PI));
}

int Transform::dphi2v(double phi) const
{
    return (int) round(phi * frameSize.height / M_PI);
}

double Transform::normalizeTheta(double rawTheta) const
{
    while (rawTheta < 0)        rawTheta += 2.0*M_PI;
    while (2*M_PI < rawTheta)   rawTheta -= 2.0*M_PI;
    
    return rawTheta;
}

int Transform::normalizeU(int rawU) const
{
    while (rawU < 0)                rawU += frameSize.width;
    while (frameSize.width < rawU) rawU -= frameSize.width;
    
    return rawU;
}