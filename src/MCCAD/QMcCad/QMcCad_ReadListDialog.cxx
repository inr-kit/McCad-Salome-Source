#include <QMcCad_ReadListDialog.hxx>
#include <QFileDialog>
#include <fstream>
#include <McCadMessenger_Singleton.hxx>

QMcCad_ReadListDialog::QMcCad_ReadListDialog()
{
    myUI.setupUi(this);
    myFileSelected = false;

    myIsDone = true;


    QPushButton *okButton = myUI.buttonBox->button(QDialogButtonBox::Ok);
    okButton->setDisabled(true);

    connect(myUI.loadButton, SIGNAL(clicked()), this, SLOT(SlotOpenFile()));
    connect(myUI.buttonBox, SIGNAL(accepted()), this, SLOT(SlotDyeSolids()));
}


bool QMcCad_ReadListDialog::FileSelected()
{
    return myFileSelected;
}


TCollection_AsciiString QMcCad_ReadListDialog::GetFileName()
{
    return myFileName;
}


double QMcCad_ReadListDialog::GetLowerBound()
{
    return myLowerBound;
}


double QMcCad_ReadListDialog::GetUpperBound()
{
    return myUpperBound;
}

list<int> QMcCad_ReadListDialog::GetCellNumberList()
{
    return myCellNumberList;
}

int QMcCad_ReadListDialog::GetNbColors()
{
    TCollection_AsciiString nbAStr(myUI.nbColorsEdit->text().toAscii().data());
    if(!nbAStr.IsIntegerValue())
        return 12;

    return nbAStr.IntegerValue();
}

list<double> QMcCad_ReadListDialog::GetValueList()
{
    return myValueList;
}

void QMcCad_ReadListDialog::SlotDyeSolids()
{

    QString upperQStr = myUI.maximumEdit->text();
  // read bounds
    TCollection_AsciiString upperAStr(upperQStr.toAscii().data());
    TCollection_AsciiString lowerAStr(myUI.minimumEdit->text().toAscii().data());

    myUpperBound = upperAStr.RealValue();
    myLowerBound = lowerAStr.RealValue();
}



void QMcCad_ReadListDialog::SlotOpenFile()
{
    QFileDialog* fileDialog = new QFileDialog(this);

    QString qFileName = fileDialog->getOpenFileName();
    myFileName = TCollection_AsciiString(qFileName.toAscii().data());

    if(!this->ParseFile())
    {
        cout << "_#_QMcCad_ReadListDialog :: File Parsing Error!!!\n";
        myIsDone = false;
    }
    else
        myIsDone = true;

    TCollection_AsciiString upperAStr(myUpperBound);
    TCollection_AsciiString lowerAStr(myLowerBound);
    QString upperValueStr(upperAStr.ToCString());
    QString lowerValueStr(lowerAStr.ToCString());
    myUI.maximumEdit->setText(upperValueStr);
    myUI.minimumEdit->setText(lowerValueStr);

    QPushButton *okButton = myUI.buttonBox->button(QDialogButtonBox::Ok);
    okButton->setDisabled(false);
}

bool QMcCad_ReadListDialog::Done()
{
    return myIsDone;
}


// private functions
bool QMcCad_ReadListDialog::ParseFile()
{
    McCadMessenger_Singleton *msgr = McCadMessenger_Singleton::Instance();

    ifstream fileReader;
    fileReader.open(myFileName.ToCString());

    myCellNumberList.clear();
    myValueList.clear();

    while(!fileReader.eof())
    {
        char lineArray[256];
        fileReader.getline(lineArray, 256);
        TCollection_AsciiString line(lineArray);

        // skip empty lines
        line.LeftAdjust();
        line.RightAdjust();
        if(line.Length() < 3)
            continue;

        TCollection_AsciiString cellNumberString;
        TCollection_AsciiString valueString;

        cellNumberString = line.Token(" \t", 1);
        valueString = line.Token(" \t", 3);

        if(!cellNumberString.IsIntegerValue())
        {
            msgr->Message("_#_QMcCad_ReadListDialog :: Error : integer value expected\n",
                          McCadMessenger_ErrorMsg);
            return false;
        }
        if(!valueString.IsRealValue())
        {
            msgr->Message("_#_QMcCad_ReadListDialog :: Error : real value expected\n",
                          McCadMessenger_ErrorMsg);
            return false;
        }

        myCellNumberList.push_back(cellNumberString.IntegerValue());
        myValueList.push_back(valueString.RealValue());
    }


    // find bounds
    double lower(100.0), upper(0.0);
    list<double>::iterator valueIt = myValueList.begin();
    for(; valueIt != myValueList.end(); valueIt++)
    {
        double currentValue = *valueIt;
        if(currentValue < lower)
            lower = currentValue;
        if(currentValue > upper)
            upper = currentValue;
    }

    myUpperBound = upper;
    myLowerBound = lower;

    return true;
}

