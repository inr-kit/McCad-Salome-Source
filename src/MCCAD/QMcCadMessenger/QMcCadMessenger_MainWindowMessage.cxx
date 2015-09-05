#include <QMcCadMessenger_MainWindowMessage.hxx>

QMcCadMessenger_MainWindowMessage::QMcCadMessenger_MainWindowMessage(){

}


QMcCadMessenger_MainWindowMessage::~QMcCadMessenger_MainWindowMessage(){

}


void QMcCadMessenger_MainWindowMessage::PrintMessage(std::string message, McCadMessenger_MessageType type){
//qiu remove gui, might be dangerous!     QMcCad_Application::GetAppMainWin()->Mcout(message, type);
}
