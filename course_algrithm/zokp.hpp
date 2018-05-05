//
//  zokp.hpp
//  course_algrithm
//
//  Created by Raii on 29/04/2018.
//  Copyright © 2018 Raii. All rights reserved.
//

#ifndef zokp_hpp
#define zokp_hpp

#include <stdio.h>
#include <vector>
#include <map>
#include <math.h>

/*
 0-1 knapsack problem
 Given c>0, wi>0, vi>0, 0<=i<=n，to find a
 0-1 vector(x1, x2, ..., xn), satisfied：
 max sum_{i=1 to n} (vi*xi),and satisfied：
 (1) sum_{i=1 to n} (wi*xi) <= c
 (2) xi∈{0, 1},1<=i<=n
 
 there'are 3 stragtegies to solve this problem:
 1. most valueable first.
 2. lightest first.
 3. max value/density first.
 
 k level improved theory:
 based on stragtegy 3, build up several combines(depended on k) and select out the solve vector.
 
 p.s
 0. to simulate here, I will write a random-data-generater to generate test datas for the sample.
 
 1. using an dp(dynamic programming) method(in a calcuable and limited data sample) to calculate the actual solve vector.
 
 2. further more, it can be solved by google's open-ai library: tensor-flow
 */

//AG is short for 'algrithm'

template<typename T>
class AGSingleton
{
public:
    virtual ~AGSingleton(void){}
    static T* getSingleton()
    {
        static T ins;
        return &ins;
    }
protected:
    AGSingleton(void){}

};

class AGSampleDataGenerater
    :public AGSingleton<AGSampleDataGenerater>
{
public:
    struct ProductAttr
    {
        int nWeight;//the type here was set as int,in KG.
        int nValue;//the type here was set as int.
        float fDV;
        ProductAttr():nWeight(0),nValue(0),fDV(0.f){}
        float getDV()const{
            if( nWeight == 0)
                return 0.f;
            return (float)nValue / (float)nWeight;
        }
        //to sort by dv.
        bool operator < (const ProductAttr& rhs) const
        {
            return rhs.fDV < fDV;//Desc sorting by density/value.
        }
    };
    typedef std::vector<ProductAttr> GroupPA;
    typedef std::vector<GroupPA> GroupDatas;
    AGSampleDataGenerater(void):m_nKnapsackCapacity(11){}
    virtual ~AGSampleDataGenerater(void){}
protected:
    GroupDatas m_vGroupDatas;
    std::map<int,std::vector<int>> m_mSolveResults;
    int m_nKnapsackCapacity;
public:
    void Init(int nSampeAmount = 600, int nProductAmount = 10, int nKnapsackCapacity = 11)
    {
        //the single weight in samples is no more than the input fKnapsackCapactiy.
        //consider a const random range for the values.
        //reserver 2 precise bits
        const int cMaxValue = 100;//100 yuan as max.
        srand((unsigned int)time(NULL));//seed for random numbers.
        m_nKnapsackCapacity = nKnapsackCapacity;
        for(int i = 0; i < nSampeAmount; ++i)
        {
            GroupPA vPa;
            m_vGroupDatas.push_back(vPa);
            std::vector<ProductAttr>& _vPa = m_vGroupDatas.back();
            for(int j = 0; j < nProductAmount; ++j)
            {
                const int _tmpW = nKnapsackCapacity+1;
                const int _tmpV = cMaxValue+1;
                int nItemWeight = 0;
                int nItemValue = 0;
                
                while(0 == nItemWeight)
                    nItemWeight = rand()%_tmpW;
                
                while(0== nItemValue)
                    nItemValue = rand()%_tmpV;
                
                ProductAttr pa;
                pa.nWeight =  nItemWeight;
                pa.nValue = nItemValue;
                pa.fDV = pa.getDV();
                _vPa.push_back(pa);
            }
        }
    }
    
    void sort()
    {
        for(auto& v:m_vGroupDatas)//using operator < as defualt sorting funtion.
            std::sort(v.begin(),v.end());
    }
    bool getGroupData(const int nGroupIndex, GroupPA& data)
    {
        if( nGroupIndex < m_vGroupDatas.size())
        {
            data = m_vGroupDatas[nGroupIndex];
            return true;
        }
        return false;
    }
    
    bool setSolveResult(const int nGroupIndex, std::vector<int>& vResult)
    {
        if( nGroupIndex < m_vGroupDatas.size())
        {
            m_mSolveResults[nGroupIndex] = vResult;
            return true;
        }
        return false;
    }
    
    bool getSolveResult(const int nGroupIndex, std::vector<int>& vResult) const
    {
        auto it = m_mSolveResults.find(nGroupIndex);
        if( it != m_mSolveResults.end())
        {
            vResult = it->second;
            return true;
        }
        return false;
    }
    
    int getSolveValue(const int nGroupIndex)
    {
        std::vector<int> v;
        int nValue = 0;
        if( getSolveResult(nGroupIndex, v))
        {
            const GroupPA& gpa = m_vGroupDatas[nGroupIndex];
            
            for(int i = 0; i < v.size(); ++i)
            {
                if( 1 == v[i] )
                    nValue += gpa[i].nValue;
            }
        }
        return nValue;
    }
    
    int getSolveWeight(const int nGroupIndex)
    {
        std::vector<int> v;
        int nWeight = 0;
        if( getSolveResult(nGroupIndex, v))
        {
            const GroupPA& gpa = m_vGroupDatas[nGroupIndex];
            
            for(int i = 0; i < v.size(); ++i)
            {
                if( 1 == v[i] )
                    nWeight += gpa[i].nWeight;
            }
        }
        return nWeight;
    }
    
    int getKnapsackCapacity()const{return m_nKnapsackCapacity;}
    
    int getSampleCount()const{return m_vGroupDatas.size();}
    
    int getGroupDataTotalWeight(const int nGroupIndex)
    {
        GroupPA data;
        if(getGroupData(nGroupIndex, data))
        {
            int nW = 0;
            for(auto& w:data)
                nW += w.nWeight;
            return nW;
        }
        return 0;
    }
    
    int getGroupDataTotalValue(const int nGroupIndex)
    {
        GroupPA data;
        if(getGroupData(nGroupIndex, data))
        {
            int nV = 0;
            for(auto& v:data)
                nV += v.nValue;
            return nV;
        }
        return 0;
    }
private:
    
};


class AGKnapsackDP
{
public:
    AGKnapsackDP(void){}
    virtual ~AGKnapsackDP(void){}
protected:
     void evaluateSetup(AGSampleDataGenerater::GroupPA& gpa,int**& pValueTable,const int n, const int c)
    {
        //bound is 0.
        pValueTable = new int*[n+1];//start from 1.
        for(int k = 0; k <= n; ++k )
        {
            pValueTable[k] = new int[c+1];
            memset(pValueTable[k],0,sizeof(int)*(c+1));
        }
        
        for(int i = 1; i <= n; ++i)
        {
            const int w = gpa[i-1].nWeight;//data index start from 0
            const int v = gpa[i-1].nValue;
            for(int j = 1; j <= c; ++j )
            {
                if( j < w)//couldn't be put in.
                    pValueTable[i][j] = pValueTable[i-1][j];
                else//could be put in.
                {
                    if(pValueTable[i-1][j]>pValueTable[i-1][j-w]+v)
                        pValueTable[i][j] = pValueTable[i-1][j];
                    else
                        pValueTable[i][j] = pValueTable[i-1][j-w]+v;
                }
            }
        }
    }
    
    void evaluateUseTable(AGSampleDataGenerater::GroupPA& gpa,std::vector<int>& vResult,int**& pValueTable, const int n, const int c)
    {
        if( gpa.size() != vResult.size() )
        {
            vResult.resize(gpa.size());//remove repeate calcultations.
            for(auto& v:vResult) v = -1;
            evaluateSetup(gpa, pValueTable, n, c);
        }
        
        if( 1 <= n && pValueTable && -1 == vResult[n-1])
        {
            const int w = gpa[n-1].nWeight;//data index start from 0
            const int v = gpa[n-1].nValue;
            
            if(pValueTable[n][c] == pValueTable[n-1][c])
            {
                //not selected.
                vResult[n-1] = 0;
                evaluateUseTable(gpa,vResult,pValueTable,n-1,c);
            }
            else if( c - w >= 0 && pValueTable[n][c] == pValueTable[n-1][c-w]+v)
            {
                //selected.
                vResult[n-1] = 1;
                evaluateUseTable(gpa,vResult,pValueTable,n-1,c-w);
            }
        }
    }
    template<typename T=int>
     T max( const T& a, const T& b) const
    {
        return (a>b)?a:b;
    }
    
    /*
     wn>C，  f(n,C) = f(n-1,C);
     wn<=C， f(n,C) = max(f(n-1,C), vn+f(n-1, C-wn) );
     given:
     f(i,0) = 0;
     f(0,i) = 0;
     f(0,0) = 0;
     */
    
    int evaluateUseRecursion(AGSampleDataGenerater::GroupPA& gpa,std::vector<int>& vResult,const int n, const int c)
    {
        if( gpa.size() != vResult.size() )
            vResult.resize(gpa.size());
        
        if( 0 == n || 0 >= c)
            return 0;//unvalid.
        
        const int w = gpa[n-1].nWeight;
        const int v = gpa[n-1].nValue;
        
        if( w > c)
        {
            //will not be selected.
            vResult[n-1] = 0;
            return evaluateUseRecursion(gpa,vResult,n-1,c);
        }
        
        std::vector<int> tmpResult1;
        std::vector<int> tmpResult2;
        tmpResult1.resize(vResult.size());
        tmpResult2.resize(vResult.size());
        
        tmpResult1.assign(vResult.begin(), vResult.end());
        tmpResult2.assign(vResult.begin(), vResult.end());
        
        //to solve the sub problem, but should never affect the original solving vector.
        int curVal = v + evaluateUseRecursion(gpa,tmpResult1,n-1,c-w);
        int preVal = evaluateUseRecursion(gpa,tmpResult2,n-1,c);
            
        if(preVal > curVal)
        {
            vResult.assign(tmpResult2.begin(), tmpResult2.end());
            vResult[n-1] = 0;
            return preVal;
        }
        
        vResult.assign(tmpResult1.begin(), tmpResult1.end());
        vResult[n-1] = 1;
        
        return curVal;
    }
    
public:
    bool solveUseTable(int nGroupIndex, std::vector<int>& resultVector, int& nMaxValue)
    {
        AGSampleDataGenerater::GroupPA gpa;
        if(!AGSampleDataGenerater::getSingleton()->getGroupData(nGroupIndex,gpa) )
            return false;
        
        resultVector.clear();
        nMaxValue = 0;
        const int n = gpa.size();
        const int c = AGSampleDataGenerater::getSingleton()->getKnapsackCapacity();
        int** pValueTable = NULL;
        evaluateUseTable(gpa,resultVector,pValueTable, n,c);
        if(pValueTable)
        {
            for(int i = 0; i <= n; ++i )
            {
                delete []pValueTable[i];
            }
            delete []pValueTable;
        }
        for(int i = 0; i < resultVector.size(); ++i )
        {
            if( 1 == resultVector[i] )
                nMaxValue += gpa[i].nValue;
                
        }
        return true;
    }
    
    bool solveUseRecursion(int nGroupIndex, std::vector<int>& resultVector, int& nMaxValue)
    {
        AGSampleDataGenerater::GroupPA gpa;
        if(!AGSampleDataGenerater::getSingleton()->getGroupData(nGroupIndex,gpa) )
            return false;
        
        resultVector.clear();
        nMaxValue = 0;
        const int n = gpa.size();
        const int c = AGSampleDataGenerater::getSingleton()->getKnapsackCapacity();
        nMaxValue = evaluateUseRecursion(gpa,resultVector,n,c);
        return true;
    }
};

class AGKnapsackKSetImprove
{
public:
    AGKnapsackKSetImprove(void){}
    virtual ~AGKnapsackKSetImprove(void){}
protected:
    bool solveFromIndexes(int nGroupIndex, std::vector<int>& resultVector,int& nMaxValue, const int* pSolveFromIndexes, const int nIndexNum)
    {
        if( NULL == pSolveFromIndexes )
            return false;
        
        AGSampleDataGenerater::GroupPA gpa;
        if(!AGSampleDataGenerater::getSingleton()->getGroupData(nGroupIndex,gpa) )
            return false;
        
        const int n = gpa.size();
        
        int c = AGSampleDataGenerater::getSingleton()->getKnapsackCapacity();
        nMaxValue = 0;
        resultVector.clear();
        resultVector.resize(gpa.size());
        for(auto& r:resultVector) r = 0;
        
        //first series.
        for(int i = 0; i < nIndexNum; ++i )
        {
            int _i = pSolveFromIndexes[i];
            if(_i >= n)
                return false;
            
            const AGSampleDataGenerater::ProductAttr& pa = gpa[_i];
            if( c - pa.nWeight >= 0 )
            {
                nMaxValue += pa.nValue;
                c -= pa.nWeight;
                resultVector[_i] = 1;
            }
            else
                break;
        }

        for(int i = 0; i < n; ++i )
        {
            bool _continue = false;
            for(int _i = 0; _i < nIndexNum;++_i)
            {
                if(pSolveFromIndexes[_i] == i)
                {
                     _continue = true;//already solved.
                    break;
                }
            }
            if(_continue)
                continue;
            
            const AGSampleDataGenerater::ProductAttr& pa = gpa[i];
            if( c - pa.nWeight >= 0 )
            {
                nMaxValue += pa.nValue;
                c -= pa.nWeight;
                resultVector[i] = 1;
            }
            else
                break;//done.
        }
        
        return true;
    }
public:
    bool solveWithK(int nGroupIndex, std::vector<int>& resultVector, const int k, int& nMaxValue)
    {
        //ensure the data-set is sorted.
        //k is up to 3.
        if( k > 3)
        {
            //supported k is up to 3 here.
            assert(false);
            return false;
        }
        AGSampleDataGenerater::getSingleton()->sort();
        AGSampleDataGenerater::GroupPA gpa;
        if(!AGSampleDataGenerater::getSingleton()->getGroupData(nGroupIndex,gpa) )
            return false;
        
        const int n = gpa.size();
        
        if( 0 == k )
        {
            int i = 0;
            return solveFromIndexes(nGroupIndex, resultVector, nMaxValue,&i,1);
        }
        else if( 1 ==k )
        {
            bool bSolved = false;
            for(int i = 0;i < n; ++i )
            {
                int _nMaxV = 0;
                std::vector<int> _rv;
                if( solveFromIndexes(nGroupIndex, _rv, _nMaxV,&i,1) )
                {
                    bSolved = true;
                    if(_nMaxV > nMaxValue )
                    {
                        nMaxValue = _nMaxV;
                        resultVector.assign(_rv.begin(), _rv.end());
                    }
                }
            }
            return bSolved;
        }
        else if( 2 == k )
        {
            bool bSolved = false;
            for(int i = 0; i < n; ++i )
            {
                for(int j = i+1; j < n; ++j )
                {
                    int indexes[2] ={i,j};
                    int _nMaxV = 0;
                    std::vector<int> _rv;
                    if( solveFromIndexes(nGroupIndex, _rv, _nMaxV,indexes,2) )
                    {
                        bSolved = true;
                        if(_nMaxV > nMaxValue )
                        {
                            nMaxValue = _nMaxV;
                            resultVector.assign(_rv.begin(), _rv.end());
                        }
                    }
                }
            }
            return bSolved;
        }
        else if( 3 == k )
        {
            bool bSolved = false;
            for(int i = 0; i < n; ++i )
            {
                for(int j = i+1; j < n; ++j )
                {
                    for(int t = j+1; t < n; ++t )
                    {
                        int indexes[3] ={i,j,t};
                        int _nMaxV = 0;
                        std::vector<int> _rv;
                        if( solveFromIndexes(nGroupIndex, _rv, _nMaxV,indexes,3) )
                        {
                            bSolved = true;
                            if(_nMaxV > nMaxValue )
                            {
                                nMaxValue = _nMaxV;
                                resultVector.assign(_rv.begin(), _rv.end());
                            }
                        }
                    }
                }
            }
            return bSolved;
        }
        
        return true;
    }
};

void SolveKnapsackProblem()
{
    AGKnapsackDP agdp;
    AGKnapsackKSetImprove agksi;
    
    AGSampleDataGenerater::getSingleton()->Init(100,10,11);
    
    //sort to improve the solving speed.
    AGSampleDataGenerater::getSingleton()->sort();
    
    int nSampeCount = AGSampleDataGenerater::getSingleton()->getSampleCount();
    
    std::cout<<"---------------use fill-table(DP) method-------------\n";
    for(int i = 0; i < nSampeCount; ++i)
    {
        AGSampleDataGenerater::GroupPA gpa;
        AGSampleDataGenerater::getSingleton()->getGroupData(i, gpa);
        std::vector<int> v;
        int mv = 0;
        agdp.solveUseTable(i, v, mv);
        
        int nTotalSetWeight = AGSampleDataGenerater::getSingleton()->getGroupDataTotalWeight(i);
        int nTotalSetValue = AGSampleDataGenerater::getSingleton()->getGroupDataTotalValue(i);
        int nLimitTotal = AGSampleDataGenerater::getSingleton()->getKnapsackCapacity();
        
        std::cout<<"NO."<<i+1<<" Total value:"<<mv<<"/"<<nTotalSetValue<<",Vector:[";
        float w = 0.f;
        for(int j = 0; j < v.size(); ++j)
        {
            std::cout<<v[j];
            if( 1 == v[j] )
            {
                w += gpa[j].nWeight;
                std::cout<<"(v:"<<gpa[j].nValue<<",w:"<<gpa[j].nWeight<<")";
            }
            std::cout<<" ";
        }
        
        std::cout<<"], Total weight:"<<w<<"/"<<nLimitTotal<<"/"<<nTotalSetWeight<<"\n";
        
        //the reference solve result.(exactly)
        AGSampleDataGenerater::getSingleton()->setSolveResult(i, v);
    }
    
    std::cout<<"---------------use recursion method-------------\n";
    for(int i = 0; i < nSampeCount; ++i)
    {
        AGSampleDataGenerater::GroupPA gpa;
        AGSampleDataGenerater::getSingleton()->getGroupData(i, gpa);
        std::vector<int> v;
        int mv = 0;
        agdp.solveUseRecursion(i, v, mv);
        
        int nTotalSetWeight = AGSampleDataGenerater::getSingleton()->getGroupDataTotalWeight(i);
        int nTotalSetValue = AGSampleDataGenerater::getSingleton()->getGroupDataTotalValue(i);
        int nLimitTotal = AGSampleDataGenerater::getSingleton()->getKnapsackCapacity();
        
        std::cout<<"NO."<<i+1<<" Total value:"<<mv<<"/"<<nTotalSetValue<<",Vector:[";
        float w = 0.f;
        for(int j = 0; j < v.size(); ++j)
        {
            std::cout<<v[j];
            if( 1 == v[j] )
            {
                w += gpa[j].nWeight;
                std::cout<<"(v:"<<gpa[j].nValue<<",w:"<<gpa[j].nWeight<<")";
            }
            std::cout<<" ";
        }
        
        std::cout<<"], Total weight:"<<w<<"/"<<nLimitTotal<<"/"<<nTotalSetWeight<<"\n";
    }
    
    std::cout<<"---------------use K-improved-greedy method-----------------\n";
    for(int i = 0; i < nSampeCount; ++i)
    {
        AGSampleDataGenerater::GroupPA gpa;
        AGSampleDataGenerater::getSingleton()->getGroupData(i, gpa);
        for(int k = 0; k < 4; ++k )
        {
            //k is up to 3.
            std::vector<int> v;
            int nMaxV = 0;
            agksi.solveWithK(i, v, k, nMaxV);
            
            int nTotalSetWeight = AGSampleDataGenerater::getSingleton()->getGroupDataTotalWeight(i);
            int nTotalSetValue = AGSampleDataGenerater::getSingleton()->getGroupDataTotalValue(i);
            int nLimitTotal = AGSampleDataGenerater::getSingleton()->getKnapsackCapacity();
            int nStanderMaxV = AGSampleDataGenerater::getSingleton()->getSolveValue(i);
            
            float fPrecise = fabsf(1.f - fabsf(float(nMaxV - nStanderMaxV) / nStanderMaxV))*100.f;
            std::cout<<"NO."<<i+1<<"(K="<<k<<",Precise="<<fPrecise<<"%)Total value:"<<nMaxV<<"/"<<nStanderMaxV<<"/"<<nTotalSetValue<<"(total),Vector:[";
            float w = 0.f;
            for(int j = 0; j < v.size(); ++j)
            {
                std::cout<<v[j];
                if( 1 == v[j] )
                {
                    w += gpa[j].nWeight;
                    std::cout<<"(v:"<<gpa[j].nValue<<",w:"<<gpa[j].nWeight<<")";
                }
                std::cout<<" ";
            }
            
            std::cout<<"], Total weight:"<<w<<"/"<<nLimitTotal<<"/"<<nTotalSetWeight<<"\n";
            
        }
    }
}
#endif /* zokp_hpp */

