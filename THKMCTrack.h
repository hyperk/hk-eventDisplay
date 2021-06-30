#ifndef THKMCTRACK_H
#define THKMCTRACK_H
#include <TEveLine.h>
#include <TEveStraightLineSet.h>

class THKMCTrack: public TEveLine
{
public:
      THKMCTrack(const char* n = "THKMCTrack")
    : TEveLine( n ) 
    { }; 
    float start[3],stop[3],mass,momentum,energy,theta,phi;
    bool drawn;
    void SetValues(float Start[3],float Stop[3],float mass,float momentum,
    	float energy,float theta,float phi,bool draw);
    void Describe();
     ClassDef(THKMCTrack, 0)  
};
#endif
