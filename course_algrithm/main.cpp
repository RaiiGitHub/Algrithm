//
//  main.cpp
//  course_algrithm
//
//  Created by Raii on 29/04/2018.
//  Copyright © 2018 Raii. All rights reserved.
//

#include <iostream>
#include <vector>
#include "zokp.hpp"

class Algrithm
{
public:
    Algrithm(){}
    virtual ~Algrithm(){}
public:
    void sayHelloToProfessor()
    {
        std::cout<<"It's an honour to be here, my respected professor, as one of your students, saying hello to you.\nKnapsack problem Solved(Using Random Experimental Datas):\n";
    }
};



int main(int argc, const char * argv[]) {
    // insert code here...
    Algrithm ag;
    ag.sayHelloToProfessor();
    
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

    return 0;
}
