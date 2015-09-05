#include <vector>
#include <iterator>
#include <string>
#include <TCollection_AsciiString.hxx>
#include <McCadBoolExp_ExprParser.ixx>

using namespace std;

static unsigned int ii;
static vector<TCollection_AsciiString> vg;

McCadBoolExp_ExprParser::McCadBoolExp_ExprParser()
{
	NotDone();
	myInputInFixCSG = new TColStd_HSequenceOfAsciiString;
	myInFixCSG = new TColStd_HSequenceOfAsciiString;
}

McCadBoolExp_ExprParser::McCadBoolExp_ExprParser(const Handle(TColStd_HSequenceOfAsciiString)& inputCSGSeq)
{
	NotDone();
	myInFixCSG = new TColStd_HSequenceOfAsciiString;
	Init(inputCSGSeq);
}

void McCadBoolExp_ExprParser::Init(const Handle(TColStd_HSequenceOfAsciiString)& inputCSGSeq)
{
	ii=0;
	vg.clear();
	myInFixCSG->Clear();

	myInputInFixCSG = inputCSGSeq;
	for ( int it = 1; it <= myInputInFixCSG->Length(); it++ )
	{
		vg.push_back(myInputInFixCSG->Value(it));
	}
	if (vg.size())
	{
		Done();
		Expression();
		if (!IsDone()) cout << "Error in the Boolean Expression!! " << endl;
	}
	else
	{
		cout << "Empty Boolean Expression!! " << endl;
		NotDone();
	}
}

Standard_Boolean McCadBoolExp_ExprParser::IsDone() const
{
	return myIsDone;
}

void McCadBoolExp_ExprParser::Done()
{
	myIsDone = Standard_True;
}

void McCadBoolExp_ExprParser::NotDone()
{
	myIsDone = Standard_False;
}

void McCadBoolExp_ExprParser::SetInFix(const Handle(TColStd_HSequenceOfAsciiString)& inputCSGSeq)
{
	Init(inputCSGSeq);
}

Handle(TColStd_HSequenceOfAsciiString) McCadBoolExp_ExprParser::GetInFix() const
{
	return myInFixCSG;
}

void McCadBoolExp_ExprParser::Expression()
{
	Term();
	if (ii < vg.size() && vg[ii] == ":")
	{
		//cout << " " << vg[ii] << " ";
		myInFixCSG->Append(vg[ii]);
		ii++;
		Expression();
	}
}

void McCadBoolExp_ExprParser::Term()
{
	Factor();
	if ((ii < vg.size()) && ((vg[ii] == "(" )|| (vg[ii] != ")" && vg[ii] != ":")))
	{
		if (vg[ii-1] !="#")
		{
			//cout << " | ";
			TCollection_AsciiString aTerm("|");
			myInFixCSG->Append(aTerm);
		}
		Term();
	}
}

void McCadBoolExp_ExprParser::Factor()
{
	if (vg[ii] == "(")
	{
		//cout << " " << vg[ii] << " ";
		myInFixCSG->Append(vg[ii]);
		ii++;
		Expression();
		if (ii < vg.size() && vg[ii] == ")")
		{
			//cout << " " << vg[ii] << " ";
			myInFixCSG->Append(vg[ii]);
			ii++;
		}
		else
		{
			cout<< "   " << vg[ii] << "  " << ii <<" Error#1: Boolean Expression Mismatch !!" << endl;
			NotDone();
		}
	}
	else if (((vg[ii] != ")" && vg[ii] != ":") || vg[ii] == "#") && ii	< vg.size())
	{
		//cout << " " << vg[ii];
		myInFixCSG->Append(vg[ii]);
		ii++;
	}
	else
	{
		cout<< "   " << vg[ii] << "  " << ii <<"  Error#2: Boolean Expression Mismatch !!" << endl;
		NotDone();
	}
}

