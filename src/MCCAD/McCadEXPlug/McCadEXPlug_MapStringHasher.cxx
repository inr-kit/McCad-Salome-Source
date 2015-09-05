#include <McCadEXPlug_MapStringHasher.ixx>

Standard_Integer McCadEXPlug_MapStringHasher::HashCode(const Standard_CString Key,const Standard_Integer Upper) 
{
 if(Key == NULL) return 0;
  
  Standard_CString charPtr   = Key;
  Standard_Integer aHashCode = 0,  alen,  i  = 0,  pos = 0,  count,  *tmphash;
  Standard_Character tabchar[20];
  
  alen = strlen(Key);
  
  while(i < alen) {
    for (count = 0,pos = i;count < (Standard_Integer)sizeof(int); count++) {
      if (pos + count >= alen)  tabchar[count] = '\0';
      else tabchar[count] = charPtr[pos + count];
      i++;
    }
    tmphash = (int *)tabchar;   
    aHashCode = aHashCode ^ *tmphash;
  }
  return aHashCode;
}

Standard_Boolean McCadEXPlug_MapStringHasher::IsEqual(const Standard_CString K1,const Standard_CString K2) 
{
  if (strcmp(K1, K2) == 0) return(Standard_True);
  else                     return(Standard_False);
}

