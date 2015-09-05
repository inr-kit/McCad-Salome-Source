#ifndef QMCCADMESSENGER_MAINWINDOWMESSAGE_HXX
#define QMCCADMESSENGER_MAINWINDOWMESSAGE_HXX

#include <McCadMessenger_ObserverInterface.hxx>
#include <QMcCad_Application.h>

class QMcCadMessenger_MainWindowMessage : public McCadMessenger_ObserverInterface{
public:

    QMcCadMessenger_MainWindowMessage();
    ~QMcCadMessenger_MainWindowMessage();
    void PrintMessage(std::string message, McCadMessenger_MessageType type);
private:

};


#endif // QMCCADMESSENGER_MAINWINDOWMESSAGE_HXX
