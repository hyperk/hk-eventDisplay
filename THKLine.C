#include "THKLine.h"
ClassImp(THKLine)
void THKLine::FillImpliedSelectedSet(Set_t& impSelSet)
{
        TEveElement::FillImpliedSelectedSet(impSelSet);
        
        for (List_ci i=fChildren.begin(); i!=fChildren.end(); ++i)
        {
                impSelSet.insert(*i);
        }
}
