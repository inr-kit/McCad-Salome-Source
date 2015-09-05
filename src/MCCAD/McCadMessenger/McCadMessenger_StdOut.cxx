#include <McCadMessenger_StdOut.hxx>
#include <iostream>


McCadMessenger_StdOut::McCadMessenger_StdOut(){

}

void McCadMessenger_StdOut::PrintMessage(std::string message, McCadMessenger_MessageType type){
    std::cout << " McCad :: " << message << std::endl;
}
