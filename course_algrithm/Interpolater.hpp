//
//  Interpolater.hpp
//  course_algrithm
//
//  Created by Raii on 04/05/2018.
//  Copyright © 2018 Raii. All rights reserved.
//

#ifndef Interpolater_hpp
#define Interpolater_hpp

#include <stdio.h>
#include <vector>
#include <math.h>
#include "math.hpp"
//question 5.
//inner-interpolating(linear regression) & minimize-2-dimensions-multiplication


#define SOLVE_MXN(N)\
void getCMatrixM2DM_m8n##N(float*& c)\
{\
    size_t m = m_vDatas.size();\
    size_t n = N;\
    if( 8 != m)\
        return;\
    c = new float[n];\
    memset(c,0,sizeof(float)*n);\
    float** _A = NULL;\
    getM2DM_AMatrix(_A,n);\
    float* _m = new float[m*n];\
    for(int i = 0; i < m; ++i )\
    {\
        for(int j =0; j < n; ++j )\
            _m[i*n+j] = _A[i][j];\
    }\
    matrix::Matrix<float,8,N> matA(_m);\
    delete []_m;\
    _m = NULL;\
    for(int i = 0; i < m; ++i)\
    {\
        delete []_A[i];\
    }\
    delete []_A;\
    float* vY = NULL;\
    getM2DM_YVector(vY);\
    matrix::Vector<float,8> Aplus(vY);\
    matrix::SquareMatrix<float, N> matSquareA = matA.transpose() * matA;\
    matrix::Matrix<float,N,8> matAPlus = matSquareA.I() * matA.transpose();\
    matrix::Vector<float,N> CVector = matAPlus * Aplus;\
    CVector.copyToRaw(c);\
}\
float getPredictMassWithM2DM_m8n##N(const float x)\
{\
    float* c = NULL;\
    getCMatrixM2DM_m8n##N(c);\
    float y = 0;\
    for(int i = 0; i < N; ++i )\
        y += c[i]*pow(x,i);\
    delete []c;\
    c = NULL;\
    return y;\
}

class AGInterpolater
{
public:
    AGInterpolater(void){}
    virtual ~AGInterpolater(void){}
    
    struct AGPoint
    {
        AGPoint():nTime(0),fMass(0.f){}
        AGPoint(int _n,float _f):nTime(_n),fMass(_f){}
        int nTime;//xi
        float fMass;//yi
    };
protected:
    float getZigmaXY()
    {
        float _zigma = 0.f;
        for(auto& d:m_vDatas)
        {
            _zigma += d.nTime * d.fMass;
        }
        return _zigma;
    }
    float getAverageX()
    {
        float _x = 0.f;
        for(auto& d:m_vDatas)
        {
            _x += d.nTime;
        }
        _x /= m_vDatas.size();
        return _x;
    }
    float getAverageY()
    {
        float _y = 0.f;
        for(auto& d:m_vDatas)
        {
            _y += d.fMass;
        }
        _y /= m_vDatas.size();
        return _y;
    }
    float getZigmaXSquare()
    {
        float _x = 0.f;
        for(auto& d:m_vDatas)
        {
            _x += d.nTime*d.nTime;
        }
        return _x;
    }
    
public:
    void setupDefaultData()
    {
        addData(3,57.6);
        addData(6,41.9);
        addData(9,31.0);
        addData(12,22.7);
        addData(15,16.6);
        addData(18,12.2);
        addData(21,8.9);
        addData(24,6.5);
    }
    void addData(int nTime, float fMass) {m_vDatas.push_back(AGPoint(nTime,fMass));}
    
    //y = bx + a in the linear regression formula.
    //start
    void getLinearRegression(float& fA, float& fB)
    {
        size_t n = m_vDatas.size();
        float xaver = getAverageX();
        float yaver = getAverageY();
        float zxs   = getZigmaXSquare();
        float zxy   = getZigmaXY();
        fB = (zxy- n*xaver*yaver)/(zxs-n*xaver*xaver);
        fA = yaver - fB*xaver;
    }
    float getPredictMassWithLR(const float x)
    {
        float fA = 0.f;
        float fB = 0.f;
        getLinearRegression(fA,fB);
        return fB*x + fA;
    }
    //end
    
    //minimize-2-dimensions-multiplication
    //F(x) = c1+c2x+c3x^2+...+cnx^(n-1)
    //here will use as max as x^3, using m = 8, n = 4(m > n)
    //A=[ 1 x1 x1^2 x1^3
    //    1 x2 x2^2 x2^3
    //    ...
    //    1 x8 x8^2 x8^3 ]
    
    void getM2DM_AMatrix(float**& matA, int n )
    {
        //Note: n < m.
        size_t m = m_vDatas.size();
        assert(n < m);
        matA = new float*[m];
        for(int i = 0; i < m; ++i )
        {
            matA[i] = new float[n];
            memset(matA[i],0, sizeof(float)*n);
        }
        for(int i = 0; i < m; ++i )
        {
            int x = m_vDatas[i].nTime;
            for(int j = 0; j < n; ++j )
            {
                //aij
                matA[i][j] = pow(x,j);
            }
        }
    }
    
    void getM2DM_YVector(float*& vY)
    {
        size_t m = m_vDatas.size();
        vY = new float[m];
        for(int i = 0; i < m; ++i )
        {
            vY[i] = m_vDatas[i].fMass;
        }
    }
    SOLVE_MXN(7);
    SOLVE_MXN(6);
    SOLVE_MXN(5);
    SOLVE_MXN(4);
    SOLVE_MXN(3);
    //end.
    
protected:
    std::vector<AGPoint> m_vDatas;
    float m_fLRA;
    float m_fLRB;
};

#define INTERPOLATE_VAL 19

#define OUTPUT_INTERPOLATE_RESULT(N)\
{\
float* c = NULL;\
agip.getCMatrixM2DM_m8n##N(c);\
std::cout<<"(m=8,n="<<N<<"): y = ";\
for(int i = 0; i<N; ++i)\
std::cout<<c[i]<<"x^"<<i<<(i==N-1?"":"+");\
delete []c;\
c = NULL;\
std::cout<<"\nf("<<INTERPOLATE_VAL<<")="<<agip.getPredictMassWithM2DM_m8n##N(INTERPOLATE_VAL)<<std::endl;\
}\


void solveInterpolateProblem()
{
    AGInterpolater agip;
    agip.setupDefaultData();
    std::cout<<"Interplating problem Solved:\n";
    std::cout<<"Using Inner-Interpolating method: Linear regression\n";
    float a = 0;
    float b = 0;
    agip.getLinearRegression(a, b);
    std::cout<<"y="<<b<<"x+"<<a<<std::endl;
    std::cout<<"f("<<INTERPOLATE_VAL<<")="<<agip.getPredictMassWithLR(INTERPOLATE_VAL)<<std::endl;
    std::cout<<"Using minimize-2-dimensions-multiplication method\n";
    
    OUTPUT_INTERPOLATE_RESULT(7);
    OUTPUT_INTERPOLATE_RESULT(6);
    OUTPUT_INTERPOLATE_RESULT(5);
    OUTPUT_INTERPOLATE_RESULT(4);
    OUTPUT_INTERPOLATE_RESULT(3);

    //conclusion: N = 4 is the best.

}
#endif /* Interpolater_hpp */
