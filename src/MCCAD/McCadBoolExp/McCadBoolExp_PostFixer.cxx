#include <McCadBoolExp_PostFixer.ixx>

#include <string>
#include <cstring>
#include <list>
#include <iterator>
#include <stack>

#include <McCadBoolExp_ExprParser.hxx>

using namespace std;

namespace
{
	inline bool isws(char c, char const * const wstr)
	{
		return (strchr(wstr, c) != NULL);
	}
}

template<typename Container> void stringtok(Container &l, string const &s,	char const * const ws = " \t\n")
{
	const string::size_type S = s.size();
	string::size_type i = 0;

	while (i < S)
	{
		// eat leading whitespace
		while ((i < S) && (isws(s[i], ws)))
			++i;
		if (i == S)
			return; // nothing left but WS

		// find end of word
		string::size_type j = i+1;
		while ((j < S) && (!isws(s[j], ws)))
			++j;

		// add word
		l.push_back(s.substr(i, j-i));

		// set up for next loop
		i = j+1;
	}
}

McCadBoolExp_PostFixer::McCadBoolExp_PostFixer()
{
	myIsDone = Standard_False;
	myPostFixCSG = new TColStd_HSequenceOfAsciiString;
}

McCadBoolExp_PostFixer::McCadBoolExp_PostFixer(	const TCollection_AsciiString& theCSG)
{
	myIsDone = Standard_False;
	myPostFixCSG = new TColStd_HSequenceOfAsciiString;
	Init(theCSG);
}

void McCadBoolExp_PostFixer::Init(const TCollection_AsciiString& theCSG)
{
	//cout << "POSTFIXER : IN : " << theCSG << endl;
	string s = theCSG.ToCString();
	string::iterator ittr;
	ittr = s.begin();
	string srp = " ";

	while (ittr != s.end() )
	{
		if ((isdigit(*ittr) || *ittr == '-' || *ittr=='.') && ittr != s.end())
			while ((isdigit(*ittr) || *ittr == '-' || *ittr=='.' || *ittr=='E' || *ittr=='e' ) && ittr != s.end())
				srp +=*ittr++;//append all digits (real, and or exp) to srp
		else
		{//make some space between real values and operator
			srp +="  ";
			srp +=*ittr++;
			srp +="  ";
		}
		srp +="  ";
	}

	list<string> tmpls;
	list<string>::iterator illl;
	stringtok(tmpls, srp); //break string to tokens and save to tmpls

	if (tmpls.size() == 0)
	{
		cout << "Error :: empty CSG expression." << endl;
		NotDone();
		return;
	}

	myInPutCSG = new TColStd_HSequenceOfAsciiString;
	myInFixCSG = new TColStd_HSequenceOfAsciiString;

	for (illl = tmpls.begin(); illl != tmpls.end(); ++illl)
	{
//qiu 		char str[(*illl).size()];
//qiu 		strcpy(str, (*illl).c_str());
//qiu 		TCollection_AsciiString aTerm(str);
		TCollection_AsciiString aTerm((*illl).c_str());
		myInPutCSG->Append(aTerm);
	}
	/*cout << "in McCadBoolExp_PostFixer.cxx\n";
	for(Standard_Integer i=1; i<=myInPutCSG->Length(); i++)
		cout << myInPutCSG->Value(i).ToCString() << " ";
	cout << endl;*/

	McCadBoolExp_ExprParser aParser;
	aParser.Init(myInPutCSG);
	if (!aParser.IsDone())
	{
		cout << "Error :: Parsing CSG expression failed." << endl;
		NotDone();
		return;
	}
	Handle(TColStd_HSequenceOfAsciiString) theInFixCSG = aParser.GetInFix();
	myInFixCSG->Clear();
	myInFixCSG->Append(theInFixCSG);

	/*for(Standard_Integer i=1; i<=myInFixCSG->Length(); i++)
		cout << myInFixCSG->Value(i).ToCString() << " ";
	cout << endl;*/

	Handle(TColStd_HSequenceOfAsciiString) thePostFixCSG = PostFix(myInFixCSG);
	if (thePostFixCSG->Length() == 0)
	{
		cout << "Error :: Postfixing CSG expression failed." << endl;
		NotDone();
		return;
	}

	myPostFixCSG->Append(thePostFixCSG);

	/*for(Standard_Integer i=1; i<=myPostFixCSG->Length(); i++)
		cout << myPostFixCSG->Value(i).ToCString() << " ";
	cout << endl;*/

	Done();
}

Standard_Boolean McCadBoolExp_PostFixer::IsDone() const
{
	return myIsDone;
}

void McCadBoolExp_PostFixer::Done()
{
	myIsDone = Standard_True;
}

void McCadBoolExp_PostFixer::NotDone()
{
	myIsDone = Standard_False;
}

Standard_Integer McCadBoolExp_PostFixer::Priority(
		const TCollection_AsciiString& theToken) const
{
	if (theToken == "(")
		return 4;
	else if (theToken == "#")
		return 3; // complement
	else if (theToken == "|")
		return 2; // intersection
	else if (theToken == ":")
		return 1; // union
	else
		return 0;
}

Handle(TColStd_HSequenceOfAsciiString) McCadBoolExp_PostFixer::PostFix(const Handle(TColStd_HSequenceOfAsciiString)& theInFix)
{
	Handle(TColStd_HSequenceOfAsciiString) thePostFix = new TColStd_HSequenceOfAsciiString;

	stack<TCollection_AsciiString> save;

	for ( int it = 1; it <= theInFix->Length(); it++ )
	{
		TCollection_AsciiString aTerm = theInFix->Value(it);
		if( (aTerm != ")") && (aTerm != "(") && (aTerm != "#") && (aTerm != "|") && (aTerm != ":"))
		{ // an operand
			thePostFix->Append(aTerm);
		}
		else if (aTerm == ")")
		{
			// a nested expression
			while (true)
			{
				TCollection_AsciiString anItem = save.top();
				save.pop();
				if (anItem == "(")
					break;
				thePostFix->Append(anItem);
			}
		}
		else
		{
			// an operator
			while (!save.empty() && Priority(save.top()) > Priority(aTerm))
			{
				if (save.top() == "(" && aTerm != ")")
				// Don't pop parenthesis unless current term is parenthesis
				break;
				thePostFix->Append(save.top());
				save.pop();
			}
			save.push(aTerm);
		}
	}
	while (!save.empty())
	{
		// flush the stack
		thePostFix->Append(save.top());
		save.pop();
	}
	return thePostFix;
}

Handle(TColStd_HSequenceOfAsciiString) McCadBoolExp_PostFixer::GetPostFix() const
{
	if(IsDone()) return myPostFixCSG;
	else
	{
		cout << "Warning:: Null PostFix Epression !!" << endl;
		return NULL;
	}
}

Handle(TColStd_HSequenceOfAsciiString) McCadBoolExp_PostFixer::GetInFix() const
{
	return myInFixCSG;
}

Handle(TColStd_HSequenceOfAsciiString) McCadBoolExp_PostFixer::GetInput() const
{
	return myInPutCSG;
}

void McCadBoolExp_PostFixer::PrintInFix(Standard_OStream& theStream)
{
	for (int it = 1; it <= myInFixCSG->Length(); it++)
	{
		theStream << (myInFixCSG->Value(it)).ToCString() << "  ";
	}
	theStream << endl;
}

void McCadBoolExp_PostFixer::PrintPostFix(Standard_OStream& theStream)
{
	for (int it = 1; it <= myPostFixCSG->Length(); it++)
	{
		theStream << (myPostFixCSG->Value(it)).ToCString() << "  ";
	}
	theStream << endl;
}

