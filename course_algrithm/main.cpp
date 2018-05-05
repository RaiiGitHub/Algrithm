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
#include "Interpolater.hpp"
#include "spip.hpp"

class Algrithm
{
public:
    Algrithm(){}
    virtual ~Algrithm(){}
public:
    void sayHelloToProfessor()
    {
        std::cout<<"It's an honor to be here, my respected professor, as one of your students, saying hello to you.\n";
    }
};

int main(int argc, const char * argv[]) {
    // insert code here...
    Algrithm ag;
    ag.sayHelloToProfessor();
    
    //solveKnapsackProblem();
    solveInterpolateProblem();
    solveSurfaceInterpolateProblem();
    return 0;
}
