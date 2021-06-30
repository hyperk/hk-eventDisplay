#ifndef THKLINE_H
#define THKLINE_H
#include "THKGamma.h"
/// When a THKLine is selected, so are its daughters
class THKLine: 
public THKGamma
{
public:
    THKLine(const char* n = "THKLine")
    : THKGamma( n ) 
    { }; 
    /// When this item is selected, select all its daughters.
    void FillImpliedSelectedSet(Set_t& impSelSet);
private:
    ClassDef(THKLine, 0)  
};
#endif
