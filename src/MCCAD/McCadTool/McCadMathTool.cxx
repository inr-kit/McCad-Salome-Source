#include "McCadMathTool.hxx"
#include "McCadConvertConfig.hxx"

#include <math.h>

McCadMathTool::McCadMathTool()
{
}



/** ********************************************************************
* @brief  Get the integer from a input float value
*
* @param  Standard_Real theValue  The input value
* @return Standard_Real theValue  The modified value
*
* @date 04/01/2014
* @author  Lei Lu
***********************************************************************/
void McCadMathTool::Integer(Standard_Real &theValue)
{
    if(theValue >= 0.0)
    {
        theValue = ceil(theValue);
    }
    else
    {
        theValue = floor(theValue);
    }
}



/** ********************************************************************
* @brief  According the tolenrence, judge the value is zero or not
*
* @param  Standard_Real theValue
* @return Standard_Boolean
*
* @date 04/01/2014
* @author  Lei Lu
***********************************************************************/
Standard_Boolean McCadMathTool::IsEqualZero(Standard_Real theValue)
{
    if( Abs(theValue) < McCadConvertConfig::GetTolerence() )
    {
        return Standard_True;
    }
    else
    {
        return Standard_False;
    }
}


/** ********************************************************************
* @brief  Set the value to be zero when it less than tolerance
*
* @param  theValue(Standard_Real &), dTolerance (Standard_Real)
* @return theValue(Standard_Real &)
*
* @date 04/01/2014
* @author  Lei Lu
***********************************************************************/
void McCadMathTool::ZeroValue(Standard_Real &theValue, Standard_Real dTolerance)
{
     if(Abs(theValue) < dTolerance)
     {
        theValue = 0.0;
     }
}




/** ********************************************************************
* @brief  Return the maximum value from the tree values
*
* @param  value1(Standard_Real &), value2(Standard_Real &),value3(Standard_Real &)
* @return maximum value(Standard_Real &)
*
* @date 12/05/2015
* @author  Lei Lu
***********************************************************************/
//qiu 'static' should not be used on member functions defined at file scope
Standard_EXPORT /*static*/ Standard_Real McCadMathTool::MaxValue(Standard_Real value1,
                                              Standard_Real value2,
                                              Standard_Real value3)
{
    return MAX3(value1,value2,value3);
}

