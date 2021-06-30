#ifndef THKGAMMA_H
#define THKGAMMA_H
#include <TEveLine.h>
#include <TEveStraightLineSet.h>
/*
Classes to allow modification of usual TEveLine behaviour
*/
/// When a THKGamma is selected so are its parents AND its daughters
class THKGamma: 
public TEveLine
{
protected:
    float theta,phi,momentum,x,y,z;
public:
    THKGamma(const char* n = "THKGamma")
    : TEveLine( n ) 
    { };        
    /// When this item is selected, select also its parent.
    void FillImpliedSelectedSet(Set_t& impSelSet);
    /// Standard creation method for a straight line of a given length/theta/phi and starting
    /// at a particular position with a certain colour.
    void Create(TString title,Color_t color,double pos[3],double length,double Momentum,double theta,double phi);
    void Describe();
    
private:
    ClassDef(THKGamma, 1)  
};

#endif
