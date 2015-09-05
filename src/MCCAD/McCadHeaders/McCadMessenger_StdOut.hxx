#ifndef MCCADMESSENGER_STDOUT_HXX
#define MCCADMESSENGER_STDOUT_HXX

#include <McCadMessenger_ObserverInterface.hxx>

class McCadMessenger_StdOut: public McCadMessenger_ObserverInterface {
private:

public:
    McCadMessenger_StdOut();
    ~McCadMessenger_StdOut() {}
    void PrintMessage( std::string message,
                                   McCadMessenger_MessageType type = McCadMessenger_DefaultMsg );
};

#endif // MCCADMESSENGER_STDOUT_HXX
