#include <McCadGUI_Messenger.hxx>

#ifndef MCCAD_NONGUI
McCadGUI_Messenger::McCadGUI_Messenger(QMcCad_Application* theApp)
{
	myApp = theApp;
	myMsgType = McCadGUI_DefaultMsg;
	myMessage = "";
}
#endif
	
//ctor
McCadGUI_Messenger::McCadGUI_Messenger()
{
#ifndef MCCAD_NONGUI
	GetApplication();
	myMsgType = McCadGUI_DefaultMsg;
	myMessage = "";
	GetApplication();
#else
	myMsgType = McCadGUI_CoutMsg;
#endif
}

// dtor
McCadGUI_Messenger::~McCadGUI_Messenger()
{
	if(myMessage != "")
	{
		myMsgType = McCadGUI_DefaultMsg;
		Broadcast();
	}

}

// append operator
void McCadGUI_Messenger::operator<<(TCollection_AsciiString opStr)
{
	Append(opStr);
}

//append
void McCadGUI_Messenger::Append(TCollection_AsciiString apStr)
{
	myMessage += apStr;
}


// standard message
void McCadGUI_Messenger::Message()
{
	myMsgType = McCadGUI_DefaultMsg;
	Broadcast();
}

void McCadGUI_Messenger::Message(TCollection_AsciiString msg)
{
	myMessage = msg;
	Message();
}


// warning
void McCadGUI_Messenger::Warning()
{
	myMsgType = McCadGUI_WarningMsg;
//	cout << myMessage.ToCString() << endl;
	Broadcast();
}


void McCadGUI_Messenger::Warning(TCollection_AsciiString msg)
{
	myMessage = msg;
	Warning();
}


// error message
void McCadGUI_Messenger::Error()
{
	myMsgType = McCadGUI_ErrorMsg;
//	cout << myMessage.ToCString() << endl;
	Broadcast();
}

void McCadGUI_Messenger::Error(TCollection_AsciiString msg)
{
	myMessage = msg;
	Error();
}


// only output to shell
void McCadGUI_Messenger::Cout()
{
	myMsgType = McCadGUI_CoutMsg;
	Broadcast();
}

void McCadGUI_Messenger::Cout(TCollection_AsciiString msg)
{
	myMessage = msg;
	Cout();
}


// make printout font bold
void McCadGUI_Messenger::Heading()
{
	myMsgType = McCadGUI_HeadingMsg;
	Broadcast();
}

void McCadGUI_Messenger::Heading(TCollection_AsciiString msg)
{
	myMessage = msg;
	Heading();
}


// set output type
void McCadGUI_Messenger::SetOutType(McCadGUI_MessageType theOutType)
{
	myMsgType = theOutType;
}

// get output type
McCadGUI_MessageType McCadGUI_Messenger::OutType()
{
	return myMsgType;
}

/////////////////////////////////////////////
// PRIVAT

// find application window
void McCadGUI_Messenger::GetApplication()
{
#ifndef MCCAD_NONGUI
	myApp = QMcCad_Application::GetAppMainWin();
#endif

/*	if(myApp == NULL)
		cout << "_#_McCadGUI_Messenger.cxx :: Application Window not found!!!\n";*/
}


// submit message to QApplication
void McCadGUI_Messenger::Broadcast()
{
#ifndef MCCAD_NONGUI
	if(myApp == NULL)
		myMsgType = McCadGUI_CoutMsg;

	if(myMsgType != McCadGUI_CoutMsg)
	{
            myApp->Mcout(myMessage.ToCString(), myMsgType);
	}
	else
#endif
		cout << myMessage.ToCString() << endl;

	myMessage = ""; // flush message buffer
}
