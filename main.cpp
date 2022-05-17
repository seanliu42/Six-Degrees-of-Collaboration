/*
 * main.cpp
 *
 * COMP 15 Project 2: Six Degrees of Collaboration
 * 
 * by Sean Liu, 12/5/2021
 */

#include "SixDegrees.h"

using namespace std;

int main(int argc, char *argv[])
{
    SixDegrees program = SixDegrees(argc, argv);
    
    program.run();
    
    return 0;
}
