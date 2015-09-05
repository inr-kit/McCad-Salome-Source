#include <McCadMessenger_Singleton.hxx>
#include <iostream>

McCadMessenger_Singleton* McCadMessenger_Singleton::m_instance = NULL;

void McCadMessenger_Singleton::Register(McCadMessenger_ObserverInterface* observer){
    m_observers.push_back(observer);
}

void McCadMessenger_Singleton::Message( std::string msg,
//qiu                      McCadMessenger_MessageType msgType = McCadMessenger_DefaultMsg){
						   McCadMessenger_MessageType msgType ){

    std::vector<McCadMessenger_ObserverInterface*>::iterator it;

    for( it = m_observers.begin(); it != m_observers.end(); it++){
        McCadMessenger_ObserverInterface* observer = (*it);
        observer->PrintMessage(msg, msgType);
    }

}
