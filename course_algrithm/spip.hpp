//
//  spip.hpp
//  course_algrithm
//
//  Created by Raii on 05/05/2018.
//  Copyright © 2018 Raii. All rights reserved.
//

#ifndef spip_hpp
#define spip_hpp

#include <stdio.h>
#include <vector>
//spip is short for surface/space point interpolating problem.

//points that are already known.
#define pi 3.14159265324
#define DEGREE_TO_RADIAN(D) D*pi/180.f
#define RADIAN_TO_DEGREE(R) R*180.f/pi
#define RANDOM_ANGLE_RADIAN() DEGREE_TO_RADIAN(rand()%360)
#define RANDOM_POINT_WEIGHT 0.001f+rand()%1000/1000.f
#define RANDOM_POINT_Z 0.001f+rand()%1000/100.f
#define EQUAL_ZERO_WITH_TOLERANCE(A,B) 1.e-6f >= fabsf(A-B)

class AGSolvePointCenter
{
    friend class AGSurfacePointInterpolating;
public:
    AGSolvePointCenter(void){srand((unsigned int)time(NULL));}
    virtual ~AGSolvePointCenter(void){}
    struct SPoint
    {
        float x;
        float y;
        float z;
        float w;//weight
        SPoint():x(0.f),y(0.f), z(0.f),w(0.f){}
        SPoint(float _x,float _y, float _z, float _w):x(_x),y(_y),z(_z),w(_w){}
        float distanceSurface(const SPoint& pt) const
        {
            return sqrt(pow(pt.x - x, 2.f)+pow(pt.y -y, 2.f));
        }
    };
    typedef std::vector<SPoint> PointArray;
public:
    //to setup sevral random points for this expriment.
    //note: the range for the experiment datas will be from x,y=-100.f to x,y = 100.f
    void setupExprimentalRandomPoints(int nDataNum,int nMaxRange = 100)
    {
        for(int i = 0; i < nDataNum; ++i )
        {
            SPoint center(rand()%nMaxRange, rand()%nMaxRange, RANDOM_POINT_Z,RANDOM_POINT_WEIGHT);
            float r = rand()%10;
            while(0 == r)
                r = rand()%10;
            addPoint(getRandomPointInCircleRange(center, r));
        }
    }
    //get a random point in a specific circle range.
    SPoint getRandomPointInCircleRange(const SPoint& center, const float radius)
    {
        // zita = random(0,2pi)
        // x = radius * cos(zita)
        // y = radius * sin(zita);
        float zita = RANDOM_ANGLE_RADIAN();//a random radian
        return SPoint(radius*cos(zita),radius*sin(zita),RANDOM_POINT_Z,RANDOM_POINT_WEIGHT);
    }
    
    void addPoint(const SPoint& pt){m_vPoints.push_back(pt);}
    //search points with a circle range.
    //note: if the database is huge, this will need to be improved.
    //such as octree,quadtree searching strategy.
    size_t getRangePointsInCircle(const SPoint& center, const float radius, PointArray& points) const
    {
        for(auto& pt:m_vPoints)
        {
            if( pt.distanceSurface(center) <= radius )
                points.push_back(pt);
        }
        return points.size();
    }
    //simple data-size as well. see above.
    bool getPoint(AGSolvePointCenter::SPoint& pt)
    {
        for(auto& _pt:m_vPoints)
        {
            if( EQUAL_ZERO_WITH_TOLERANCE(_pt.distanceSurface(pt),0.f) )
            {
                pt.z = _pt.z;
                pt.w = _pt.w;
                return true;
            }
        }
        return false;
    }
    int getDataSize()const{return m_vPoints.size();}
    //other-shape search. here will not be implemented.
protected:
    PointArray m_vPoints;
};

class AGSurfacePointInterpolating
{
public:
    AGSurfacePointInterpolating(void){}
    virtual ~AGSurfacePointInterpolating(void){}
public:
    void init()
    {
        m_DataCenter.setupExprimentalRandomPoints(1000);//setup some datas.
    }

    bool doInterpolate(const float r,const AGSolvePointCenter::SPoint& center, AGSolvePointCenter::SPoint& pt, AGSolvePointCenter::PointArray& points)
    {
        //pt is the interpolate point.
        if(m_DataCenter.getPoint(pt))
            return false;//already exist.
        if( 0 == m_DataCenter.getRangePointsInCircle(center, r, points))
        {
            //has nothing, try to setup a random point.
            pt = m_DataCenter.getRandomPointInCircleRange(center, r);
            m_DataCenter.addPoint(pt);
            return true;
        }
        //do the interpolation
        pt.z = 0;
        pt.w = 0;
        for(auto& _pt:points)
        {
            pt.z += _pt.z*_pt.w;//use average with weight.
            pt.w += _pt.w;//use average weight.
        }
        pt.z /= points.size();
        pt.w /= points.size();
        return true;
    }
    
    AGSolvePointCenter& getDataCenter(){return m_DataCenter;}
protected:
    AGSolvePointCenter m_DataCenter;
};

void solveSurfaceInterpolateProblem()
{
    AGSurfacePointInterpolating agspip;
    agspip.init();
    float x = rand()%100;
    float y = rand()%100;
    AGSolvePointCenter::SPoint pt(x,y,0.f,0.f);
    bool bHasPoint = agspip.getDataCenter().getPoint(pt);
    std::cout<<"Surface interpolation: with random experimental datas\n";
    std::cout<<"Data size current:"<<agspip.getDataCenter().getDataSize()<<std::endl;
    std::cout<<"Point at ("<<x<<","<<y<<") "<<(bHasPoint?"exsit.":"not exsit.")<<std::endl;
    
    //do interpolate.
    std::cout<<"Here will do interpolating 500 times.\n";
    for(int i = 0; i < 500; ++i )
    {
        pt.x = rand()%100;//random again.
        AGSolvePointCenter::SPoint center(rand()%100, rand()%100, RANDOM_POINT_Z, RANDOM_POINT_WEIGHT);
        AGSolvePointCenter::PointArray points;
        float r = rand()%10;
        while(0 == r)
            r = rand()%10;
        
        if( agspip.doInterpolate(r, center, pt, points))
            std::cout<<"Interplate a point(x,y,z,w): ("<<pt.x<<","<<pt.y<<","<<pt.z<<","<<pt.w<<"), use the circle range: center("<<center.x<<","<<center.y<<"),radius="<<r<<", in range points: "<<points.size()<<"\n";
        else
            std::cout<<"the point is already exist: ("<<pt.x<<","<<pt.y<<","<<pt.z<<","<<pt.w<<")\n";
    }
    
}
#endif /* spip_hpp */
