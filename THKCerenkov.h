#ifndef THKCERENKOV_H
#define THKCERENKOV_H
#include "THKGamma.h"
#include <TEveLine.h>
#include <TEveStraightLineSet.h>
/// When a THKCerenkov is selected so are its parents
class THKCerenkov: 
public THKGamma
{
public:
    THKCerenkov(const char* n = "THKCerenkov")
    : THKGamma( n ) 
    { };        
    /// When this item is selected, select also its parent.
    void FillImpliedSelectedSet(Set_t& impSelSet);
    void Describe();
private:
    ClassDef(THKCerenkov, 0)  
};
#endif
