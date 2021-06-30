#ifndef THKCERENKOV2D_H
#define THKCERENKOV2D_H
#include "THKGamma.h"
#include <TEveLine.h>
#include <TEveStraightLineSet.h>
class THKCerenkov2D: public TEveStraightLineSet
{
public:
      THKCerenkov2D(const char* n = "THKCerenkov2D")
    : TEveStraightLineSet( n ) 
    { }; 
    float start[3],momentum,theta,phi;
    void SetValues(double Start[3],double momentum,double theta,double phi);
    void Describe();
     ClassDef(THKCerenkov2D, 0)  
};
#endif
