#include <McCadGUI_ProgressDialog.ixx>
#include <QApplication>

McCadGUI_ProgressDialog::McCadGUI_ProgressDialog(QWidget* theParent)
{
	if(theParent!=NULL)
	{
		myQProgress = new QProgressDialog(theParent);
		myQProgress->setMinimum(0);
		myQProgress->setMaximum(100);
		myQProgress->setMinimumDuration(500);
		myTotalSteps = 0;
		myLableText = "";
		myCancelButtonText= "Cancel";
	}
}

void McCadGUI_ProgressDialog::SetTotalSteps(const Standard_Integer theTotalSteps)
{
	myTotalSteps = theTotalSteps;
}

void McCadGUI_ProgressDialog::SetProgress(const Standard_Integer theProgressPosition)
{
    if(theProgressPosition)
        ;
}

void McCadGUI_ProgressDialog::SetLabelText(const TCollection_AsciiString& theLabelText)
{
	myLableText = theLabelText;
	if(myQProgress)
		myQProgress->setLabelText(theLabelText.ToCString());
}

void McCadGUI_ProgressDialog::SetCancelButtonText(const TCollection_AsciiString& theCancelButtonText)
{
	myCancelButtonText = theCancelButtonText;
}

void McCadGUI_ProgressDialog::Cancel()
{

}

Standard_Boolean McCadGUI_ProgressDialog::Show(Standard_Boolean force)
{
	if(myQProgress != NULL)
	{
		Standard_Real aPc = GetPosition(); //always within [0,1]
		int aVal = myQProgress->minimum() + aPc * (myQProgress->maximum() - myQProgress->minimum());
		myQProgress->setValue (aVal);
		QApplication::processEvents(); //to let redraw and keep GUI responsive
                return Standard_True;
	}
        return Standard_False;
}

Standard_Boolean McCadGUI_ProgressDialog::UserBreak()
{
	if(myQProgress != NULL)
		return myQProgress->wasCanceled();

        return Standard_False;
}

