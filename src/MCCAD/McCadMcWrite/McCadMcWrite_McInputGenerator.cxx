#include <McCadMcWrite_McInputGenerator.ixx>

McCadMcWrite_McInputGenerator::McCadMcWrite_McInputGenerator()
{
  myHeader = new TColStd_HSequenceOfAsciiString;
}

void McCadMcWrite_McInputGenerator::MakeHeader() const
{

}

void McCadMcWrite_McInputGenerator::AddHeaderText(const TCollection_AsciiString& theText)
{
  myHeader->Append(theText);
}

void McCadMcWrite_McInputGenerator::PrintHeader(Standard_OStream& theStream)
{
}

void McCadMcWrite_McInputGenerator::PrintAll(Standard_OStream& theStream)
{
}

