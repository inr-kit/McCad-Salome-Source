#ifndef MCCADMESSENGER_SINGLETON_HXX
#define MCCADMESSENGER_SINGLETON_HXX

/**
  Singleton class for framework wide messages
  Inherit the abstract interface McCadMessenger_ObserverInterface
  and use the register function of this class to register a new observer
*/

#include <McCadMessenger_ObserverInterface.hxx>
#include <McCadMessenger_MessageType.hxx>
#include <vector>
#include <string>


//qiu add Standard_EXPORT 
#ifndef _Standard_HeaderFile
#include <Standard.hxx>
#endif
#ifndef _Standard_Macro_HeaderFile
#include <Standard_Macro.hxx>
#endif

class McCadMessenger_Singleton
{

public:
    //! returns static instance
Standard_EXPORT     static McCadMessenger_Singleton* Instance(){
        if(m_instance == 0)
            m_instance = new McCadMessenger_Singleton;

        return m_instance;
    }

    //! register new printing mechanism
     void Register(McCadMessenger_ObserverInterface* observer);

    //! send message to all registered observers to print (std out, log file, status window,...)
Standard_EXPORT     void Message( std::string msg,
                         McCadMessenger_MessageType msgType = McCadMessenger_DefaultMsg);

private:
    McCadMessenger_Singleton() {}
Standard_EXPORT     static McCadMessenger_Singleton *m_instance;
    std::vector<McCadMessenger_ObserverInterface*> m_observers;
};

#endif
