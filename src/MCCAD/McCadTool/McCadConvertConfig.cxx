#include "McCadConvertConfig.hxx"
//qiu add SALOME definition
#ifndef SALOME
#include <McCadMessenger_Singleton.hxx>
#endif
#include <QtXml/QDomComment>
#include <QFile>

bool McCadConvertConfig::m_bWriteCollisionFile =false;
bool McCadConvertConfig::m_bVoidGenerate = true;

double McCadConvertConfig::m_dTolerance = 1.0e-7;
double McCadConvertConfig::m_dMinIptSolidVol = 1.0;
double McCadConvertConfig::m_dMaxDiscLength = 200;
double McCadConvertConfig::m_dMinDecomFaceArea = 50;
double McCadConvertConfig::m_dMinVoidVol = 1.0;
double McCadConvertConfig::m_dAngleTolerance = 1.0e-3;

unsigned int McCadConvertConfig::m_iVoidDecomposeDepth = 10;
unsigned int McCadConvertConfig::m_iInitCellNum = 0;
unsigned int McCadConvertConfig::m_iInitSurfNum = 0;
double McCadConvertConfig::m_fXResolution = 0.001;
double McCadConvertConfig::m_fYResolution = 0.001;
double McCadConvertConfig::m_fRResolution = 0.0314;

unsigned int McCadConvertConfig::m_iMaxSmplPntNum = 50;
unsigned int McCadConvertConfig::m_iMinSmplPntNum = 20;

TCollection_AsciiString McCadConvertConfig::m_strInputFileName = "";
TCollection_AsciiString McCadConvertConfig::m_strMatFile = "";
TCollection_AsciiString McCadConvertConfig::m_strDirectory = "";
McCadGeom_Unit McCadConvertConfig::m_units;
map<TCollection_AsciiString,Standard_Integer> McCadConvertConfig::m_SurfSeq;

McCadConvertConfig::McCadConvertConfig()
{
}

void McCadConvertConfig::SetSurfSequ()
{
    // xiugai lei
    m_SurfSeq.clear();
    m_SurfSeq.insert(pair<TCollection_AsciiString,Standard_Integer>("PX",1));
    m_SurfSeq.insert(pair<TCollection_AsciiString,Standard_Integer>("PY",2));
    m_SurfSeq.insert(pair<TCollection_AsciiString,Standard_Integer>("PZ",3));
    m_SurfSeq.insert(pair<TCollection_AsciiString,Standard_Integer>("P",4));
    m_SurfSeq.insert(pair<TCollection_AsciiString,Standard_Integer>("SO",5));
    m_SurfSeq.insert(pair<TCollection_AsciiString,Standard_Integer>("SX",6));
    m_SurfSeq.insert(pair<TCollection_AsciiString,Standard_Integer>("SY",7));
    m_SurfSeq.insert(pair<TCollection_AsciiString,Standard_Integer>("SZ",8));
    m_SurfSeq.insert(pair<TCollection_AsciiString,Standard_Integer>("S ",9));
    m_SurfSeq.insert(pair<TCollection_AsciiString,Standard_Integer>("CX",10));
    m_SurfSeq.insert(pair<TCollection_AsciiString,Standard_Integer>("CY",11));
    m_SurfSeq.insert(pair<TCollection_AsciiString,Standard_Integer>("CZ",12));
    m_SurfSeq.insert(pair<TCollection_AsciiString,Standard_Integer>("C/X",13));
    m_SurfSeq.insert(pair<TCollection_AsciiString,Standard_Integer>("C/Y",14));
    m_SurfSeq.insert(pair<TCollection_AsciiString,Standard_Integer>("C/Z",15));
    m_SurfSeq.insert(pair<TCollection_AsciiString,Standard_Integer>("KX",16));
    m_SurfSeq.insert(pair<TCollection_AsciiString,Standard_Integer>("KY",17));
    m_SurfSeq.insert(pair<TCollection_AsciiString,Standard_Integer>("KZ",18));
    m_SurfSeq.insert(pair<TCollection_AsciiString,Standard_Integer>("K/X",19));
    m_SurfSeq.insert(pair<TCollection_AsciiString,Standard_Integer>("K/Y",20));
    m_SurfSeq.insert(pair<TCollection_AsciiString,Standard_Integer>("K/Z",21));
    m_SurfSeq.insert(pair<TCollection_AsciiString,Standard_Integer>("TX",22));
    m_SurfSeq.insert(pair<TCollection_AsciiString,Standard_Integer>("TY",23));
    m_SurfSeq.insert(pair<TCollection_AsciiString,Standard_Integer>("TZ",24));
    m_SurfSeq.insert(pair<TCollection_AsciiString,Standard_Integer>("GQ",25));
}


/*qiu static*/ Standard_Integer McCadConvertConfig::GetSurfSequNum(TCollection_AsciiString SurfSymb)
{
    return m_SurfSeq[SurfSymb];
}

/*void McCadVoidPrmt::ReadPrmt(const TCollection_AsciiString InputFileName)
{
    McCadMessenger_Singleton* msgr = McCadMessenger_Singleton::Instance();
    TCollection_AsciiString message;

    QString fileName(InputFileName.ToCString());
    QFile file(fileName);
    if(!file.open(QFile::ReadOnly|QFile::Text))
    {
        message.AssignCat("Cannot open parameter XML file : ");
        msgr->Message( message.ToCString(), McCadMessenger_ErrorMsg );
        file.close();
        return;
    }

    QDomDocument doc;
    QString errStr;
    int errLine, errCol;
    if (doc.setContent(&file,false,&errStr,&errLine,&errCol))
    {
        message.AssignCat("Cannot open parameter XML file : ");
        msgr->Message( message.ToCString(), McCadMessenger_ErrorMsg );
        file.close();
        return;
    }

    QDomElement root = doc.documentElement();

    bWriteCollisionFile = root.attributeNode("WriteCollisionFile").nodeValue().toInt();

    iVoidDecomposeDepth = root.attributeNode("VoidDocomposeDepth").nodeValue().toInt();
    iInitCellNum = root.attributeNode("InitCellNum").nodeValue().toInt();
    iInitSurfNum = root.attributeNode("InitSurfNum").nodeValue().toInt();

    iXResolution = root.attributeNode("XResolution").nodeValue().toInt();
    iYResolution = root.attributeNode("YResolution").nodeValue().toInt();

    iMaxSmplPntNum = root.attributeNode("MaxSamplePoints").nodeValue().toInt();
    iMinSmplPntNum = root.attributeNode("MinSamplePoints").nodeValue().toInt();

    dMinIptSolidVol = root.attributeNode("MinInputSolidVolume").nodeValue().toDouble();
    dMaxDiscLength = root.attributeNode("MaxDescriptionLength").nodeValue().toDouble();

    dTolerance = root.attributeNode("Tolerance").nodeValue().toDouble();

    strInputFileName = root.attributeNode("InputModelName").nodeValue().toStdString();
    strMatFile = root.attributeNode("MaterialFile").nodeValue().toStdString();
    strDirectory = root.attributeNode("WorkingDirectory").nodeValue().toStdString();

}*/

bool McCadConvertConfig::ReadPrmt(const TCollection_AsciiString InputFileName)
{
#ifndef SALOME
    McCadMessenger_Singleton* msgr = McCadMessenger_Singleton::Instance();
#endif
    TCollection_AsciiString message("_#_McCadConvertTools_InputFileParser :: ");

    // open parameter file
    ifstream inFile(InputFileName.ToCString());
    if (!inFile) {
#ifndef SALOME
        message.AssignCat("Cannot open parameter file : ");
        message.AssignCat(InputFileName);
        msgr->Message( message.ToCString(), McCadMessenger_ErrorMsg );
#else
        cout << message.ToCString()<<endl;
#endif
        return false;
    }

    m_strInputFileName = InputFileName;

    // read file line by line
    while(inFile) {
        char str[255];
        inFile.getline(str,255);
        TCollection_AsciiString iString(str);

        // skip empty lines
        if(iString.IsEmpty())
            continue;

        // skip comment lines
        iString.LeftAdjust();
        iString.RightAdjust();
        if(iString.Value(1) == '#')
            continue;

        // turn tabs into spaces
        iString.ChangeAll('\t', ' ', Standard_False);

        // directories to add
        if( iString.Search(" ") < 0 ) {
                iString += TCollection_AsciiString(" ");
                m_strDirectory += iString;
        }
        //codeword with parameter
        else {
            int i = iString.Search(" ");
            TCollection_AsciiString numString = iString.Split(i);
            numString.LeftAdjust();
            numString.RightAdjust();
            iString.LeftAdjust();
            iString.RightAdjust();
            iString.UpperCase();

            // search for codewords
            if(iString.IsEqual("WRITECOLLISIONFILE")) {
                if( numString.IsEqual("1")    || numString.IsEqual("true") ||
                    numString.IsEqual("True") || numString.IsEqual("TRUE")  )
                {
                    m_bWriteCollisionFile = true;
                }
                else
                    m_bWriteCollisionFile = false;
            }
            else if(iString.IsEqual("MINIMUMINPUTSOLIDVOLUME")) {
                if(!numString.IsRealValue())
                    MissmatchMessage(iString, numString);
                else
                    m_dMinIptSolidVol = numString.RealValue();
            }
            else if(iString.IsEqual("MINIMUMVOIDVOLUME")) {
                if(!numString.IsRealValue())
                    MissmatchMessage(iString,numString);
                else {
                    m_dMinVoidVol = numString.RealValue();
                }
            }
            else if(iString.IsEqual("MINIMUMSIZEOFDECOMPOSITIONFACEAREA")) {
                if(!numString.IsRealValue())
                    MissmatchMessage(iString,numString);
                else
                    m_dMinDecomFaceArea = numString.RealValue();
            }
            else if(iString.IsEqual("MAXCELLEXPRESSIONLENGTH")) {
                if(!numString.IsRealValue())
                    MissmatchMessage(iString,numString);
                else
                    m_dMaxDiscLength = numString.RealValue();
            }
            else if(iString.IsEqual("MAXDECOMPOSEDEPTH")) {
                if(!numString.IsIntegerValue() && !numString.IsRealValue())
                    MissmatchMessage(iString,numString);
                else
                    m_iVoidDecomposeDepth = numString.IntegerValue();
            }
            else if(iString.IsEqual("XRESOLUTION")) {
                if(!numString.IsIntegerValue() && !numString.IsRealValue())
                    MissmatchMessage(iString,numString);
                else
                    m_fXResolution = numString.RealValue();
            }
            else if(iString.IsEqual("YRESOLUTION")) {
                if(!numString.IsIntegerValue() && !numString.IsRealValue())
                    MissmatchMessage(iString,numString);
                else
                   m_fYResolution = numString.RealValue();
            }
            else if(iString.IsEqual("RRESOLUTION")) {
                if(!numString.IsIntegerValue() && !numString.IsRealValue())
                    MissmatchMessage(iString,numString);
                else
                   m_fRResolution = numString.RealValue();
            }
            else if(iString.IsEqual("MINIMUMNUMBEROFSAMPLEPOINTS")) {
                if(!numString.IsIntegerValue() && !numString.IsRealValue())
                    MissmatchMessage(iString,numString);
                else
                    m_iMinSmplPntNum = numString.IntegerValue();
            }
            else if(iString.IsEqual("MAXIMUMNUMBEROFSAMPLEPOINTS")) {
                if(!numString.IsIntegerValue() && !numString.IsRealValue())
                    MissmatchMessage(iString,numString);
                else
                    m_iMaxSmplPntNum = numString.IntegerValue();
            }
            else if(iString.IsEqual("TOLERANCE")) {
                if(!numString.IsRealValue())
                    MissmatchMessage(iString,numString);
                else
                    m_dTolerance = numString.RealValue();
            }
            else if(iString.IsEqual("ANGLETOLERANCE")) {
                if(!numString.IsRealValue())
                    MissmatchMessage(iString,numString);
                else
                    m_dAngleTolerance = numString.RealValue();
            }
            else if(iString.IsEqual("MDFILE")) {
                    m_strMatFile = numString;
            }
            else if(iString.IsEqual("INITSURFNB")) {
                if(!numString.IsIntegerValue())
                    MissmatchMessage(iString,numString);
                else
                   m_iInitSurfNum = numString.IntegerValue();
            }
            else if(iString.IsEqual("INITCELLNB")) {
                if(!numString.IsIntegerValue())
                    MissmatchMessage(iString,numString);
                else
                    m_iInitCellNum = numString.IntegerValue();
            }
            else if(iString.IsEqual("VOIDGENERATE")) {
                if( numString.IsEqual("1")    || numString.IsEqual("true") ||
                    numString.IsEqual("True") || numString.IsEqual("TRUE")  )
                {
                    m_bVoidGenerate = true;
                }
                else
                    m_bVoidGenerate = false;
            }
            else if(iString.IsEqual("UNITS")) {
                if(numString.IsEqual("CM") || numString.IsEqual("cm"))
                    m_units = McCadGeom_CM;
                if(numString.IsEqual("MM") || numString.IsEqual("mm"))
                    m_units = McCadGeom_MM;
                if(numString.IsEqual("INCH") || numString.IsEqual("inch"))
                    m_units = McCadGeom_Inch;
            }
            else{
                TCollection_AsciiString message("_#_McCadIOHelper_InputFileParser :: Unknown keyword : ");
                message.AssignCat(iString);
#ifndef SALOME
                msgr->Message(message.ToCString(), McCadMessenger_WarningMsg);
#else
        cout <<message.ToCString() <<endl;
#endif
            }
        }
    }
    SetSurfSequ();
    return true;
}

void McCadConvertConfig::MissmatchMessage(TCollection_AsciiString &keyword, TCollection_AsciiString &parameter)
{
#ifndef SALOME
    McCadMessenger_Singleton* msgr = McCadMessenger_Singleton::Instance();
#endif
    TCollection_AsciiString message("_#_McCadConvertTools_InputFileParser :: ");
    message.AssignCat("unexpected data type for keyword \'");
    message.AssignCat(keyword);
    message.AssignCat("\' ( ");
    message.AssignCat(parameter);
    message.AssignCat(" ) ");
#ifndef SALOME
    msgr->Message(message.ToCString());
#else
        cout <<message.ToCString() <<endl;
#endif
}


void McCadConvertConfig::SetMinMaxSmplPntNum(int iMinPnt,int iMaxPnt)
{
    m_iMinSmplPntNum = iMinPnt;
    m_iMaxSmplPntNum = iMaxPnt;
}

//qiu add Set** functions


bool McCadConvertConfig::WriteCollisionFile(){return m_bWriteCollisionFile;}
bool McCadConvertConfig::GenerateVoid(){return m_bVoidGenerate;}

double McCadConvertConfig::GetTolerence(){return m_dTolerance;}
double McCadConvertConfig::GetAngleTolerance(){return m_dAngleTolerance;}
double McCadConvertConfig::GetMinIptSolidVod(){return m_dMinIptSolidVol;}
double McCadConvertConfig::GetMaxDiscLength(){return m_dMaxDiscLength;}
double McCadConvertConfig::GetMinDecomFaceArea(){return m_dMinDecomFaceArea;}
double McCadConvertConfig::GetMinVoidVol(){return m_dMinVoidVol;}

unsigned int McCadConvertConfig::GetVoidDecomposeDepth(){return m_iVoidDecomposeDepth;}
unsigned int McCadConvertConfig::GetInitCellNum(){return m_iInitCellNum;}
unsigned int McCadConvertConfig::GetInitSurfNum(){return m_iInitSurfNum;}
double McCadConvertConfig::GetXResolution(){return m_fXResolution;}
double McCadConvertConfig::GetYResolution(){return m_fYResolution;}
double McCadConvertConfig::GetRResolution(){return m_fRResolution;}
unsigned int McCadConvertConfig::GetMaxSmplPntNum(){return m_iMaxSmplPntNum;}
unsigned int McCadConvertConfig::GetMinSmplPntNum(){return m_iMinSmplPntNum;}

TCollection_AsciiString McCadConvertConfig::GetInputFileName(){return m_strInputFileName;}
TCollection_AsciiString McCadConvertConfig::GetMatFile(){return m_strMatFile;}
TCollection_AsciiString McCadConvertConfig::GetDirectory(){return m_strDirectory;}



void McCadConvertConfig::SetWriteCollisionFile(const bool & bWriteCollisionFile){ m_bWriteCollisionFile = bWriteCollisionFile;}
void McCadConvertConfig::SetGenerateVoid(const bool & bVoidGenerate){ m_bVoidGenerate = bVoidGenerate;}

void McCadConvertConfig::SetTolerence(const double & dTolerance){ m_dTolerance = dTolerance;}
void McCadConvertConfig::SetAngleTolerance(const double & dAngleTolerance){ m_dAngleTolerance = dAngleTolerance;}
void McCadConvertConfig::SetMinIptSolidVod(const double & dMinIptSolidVol){ m_dMinIptSolidVol = dMinIptSolidVol;}
void McCadConvertConfig::SetMaxDiscLength(const double & dMaxDiscLength ){ m_dMaxDiscLength = dMaxDiscLength;}
void McCadConvertConfig::SetMinDecomFaceArea(const double & dMinDecomFaceArea){ m_dMinDecomFaceArea = dMinDecomFaceArea;}
void McCadConvertConfig::SetMinVoidVol(const double & dMinVoidVol ){ m_dMinVoidVol = dMinVoidVol;}

void McCadConvertConfig::SetVoidDecomposeDepth(const unsigned int & iVoidDecomposeDepth){ m_iVoidDecomposeDepth = iVoidDecomposeDepth;}
void McCadConvertConfig::SetInitCellNum(const unsigned int & iInitCellNum){ m_iInitCellNum = iInitCellNum;}
void McCadConvertConfig::SetInitSurfNum(const unsigned int & iInitSurfNum){ m_iInitSurfNum = iInitSurfNum;}
void McCadConvertConfig::SetXResolution(const double & fXResolution){ m_fXResolution = fXResolution;}
void McCadConvertConfig::SetYResolution(const double & fYResolution){ m_fYResolution = fYResolution;}
void McCadConvertConfig::SetRResolution(const double & fRResolution){ m_fRResolution = fRResolution;}
void McCadConvertConfig::SetMaxSmplPntNum(const unsigned int & iMaxSmplPntNum){ m_iMaxSmplPntNum = iMaxSmplPntNum;}
void McCadConvertConfig::SetMinSmplPntNum(const unsigned int & iMinSmplPntNum){ m_iMinSmplPntNum = iMinSmplPntNum;}

void McCadConvertConfig::SetInputFileName(const TCollection_AsciiString &strInputFileName){ m_strInputFileName = strInputFileName;}
void McCadConvertConfig::SetMatFile(const TCollection_AsciiString &strMatFile){ m_strMatFile = strMatFile ;}
void McCadConvertConfig::SetDirectory(const TCollection_AsciiString &strDirectory){ m_strDirectory = strDirectory;}
