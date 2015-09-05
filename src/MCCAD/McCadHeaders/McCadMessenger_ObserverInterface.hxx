#ifndef MCCADMESSENGER_OBSERVERINTERFACE_HXX
#define MCCADMESSENGER_OBSERVERINTERFACE_HXX

/*
  Interface for observers for the McCadMessenger_Singleton class
*/

#include <string>
#include <McCadMessenger_MessageType.hxx>

class McCadMessenger_ObserverInterface{
private:

public:
    virtual ~McCadMessenger_ObserverInterface(){}
    virtual void PrintMessage( std::string message,
                               McCadMessenger_MessageType type = McCadMessenger_DefaultMsg ) = 0;
};

#endif // MCCADMESSENGER_OBSERVERINTERFACE_HXX
