#ifndef MCCADMATHTOOL_HXX
#define MCCADMATHTOOL_HXX

#include <Standard.hxx>

#include <TCollection_AsciiString.hxx>
#include <QString>

#define MAX2(X, Y)	(Abs(X) > Abs(Y)? Abs(X) : Abs(Y))
#define MAX3(X, Y, Z)	(MAX2(MAX2(X,Y) , Z) )

class McCadMathTool
{
public:

    void* operator new(size_t,void* anAddress)
      {
        return anAddress;
      }
    void* operator new(size_t size)
      {
        return Standard::Allocate(size);
      }
    void  operator delete(void *anAddress)
      {
        if (anAddress) Standard::Free((Standard_Address&)anAddress);
      }

public:
    McCadMathTool();

    /**< Change the float data to interger */
    Standard_EXPORT static void Integer(Standard_Real &theValue);
    /**< If the value less than the default tolerance, set it zero */
    Standard_EXPORT static Standard_Boolean IsEqualZero(Standard_Real theValue);
    /**< If the value less than tolerance set it zero */
    Standard_EXPORT static void ZeroValue(Standard_Real &theValue, Standard_Real dTolerance);
    /**< Reture the max one of the three input values */
    Standard_EXPORT static Standard_Real MaxValue(Standard_Real value1, Standard_Real value2, Standard_Real value3);

};

#endif // MCCADMATHTOOL_HXX
