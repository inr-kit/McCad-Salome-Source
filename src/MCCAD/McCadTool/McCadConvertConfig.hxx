#ifndef MCCADCONVERTCONFIG_HXX
#define MCCADCONVERTCONFIG_HXX

#include <string>
#include <TCollection_AsciiString.hxx>
#include <map>



enum McCadGeom_Unit {
    McCadGeom_CM = 0,
    McCadGeom_MM = 1,
    McCadGeom_Inch = 2
};

using namespace std;

class  McCadConvertConfig
{

public:

    void* operator new(size_t,void* anAddress)
      {
        return anAddress;
      }
    void* operator new(size_t size)
      {
        return Standard::Allocate(size);
      }
    void  operator delete(void *anAddress)
      {
        if (anAddress) Standard::Free((Standard_Address&)anAddress);
      }

    McCadConvertConfig();

private:

    static bool m_bWriteCollisionFile;           /**< Whether write collision file or not */
    static bool m_bVoidGenerate;                 /**< Whether generate the void */

    static double m_dTolerance;                  /**< Tolerence, the value less than tolerence will be treat as zero*/
    static double m_dMinIptSolidVol;             /**< The minimum volume of input solid */
    static double m_dMaxDiscLength;              /**< The maximum length of cell discription */
    static double m_dMinDecomFaceArea;
    static double m_dMinVoidVol;
    static double m_dAngleTolerance;             /**< The angle toleranve between two direction */

    static unsigned int m_iVoidDecomposeDepth;   /**< The depth of void decomposition */
    static unsigned int m_iInitCellNum;          /**< Initial cell number */
    static unsigned int m_iInitSurfNum;          /**< Initial surface number */
    static unsigned int m_iXResolution;          /**< Resolution of X axis */
    static unsigned int m_iYResolution;          /**< Resolution of Y axis */
    static double m_iRResolution;                /**< R direction */

    static unsigned int m_iMaxSmplPntNum;        /**< Max sample point number */
    static unsigned int m_iMinSmplPntNum;        /**< Min sample point number */

    static TCollection_AsciiString m_strInputFileName;   /**< Input file name */
    static TCollection_AsciiString m_strMatFile;         /**< Material file */
    static TCollection_AsciiString m_strDirectory;       /**< Working directory */

    static McCadGeom_Unit m_units;                       /**< Units of CAD model*/
    static map<TCollection_AsciiString,Standard_Integer> m_SurfSeq;

public:

Standard_EXPORT    static void MissmatchMessage(TCollection_AsciiString &keyword, TCollection_AsciiString &parameter);
Standard_EXPORT    static void SetSurfSequ();
Standard_EXPORT    void AssignPrmt(const TCollection_AsciiString StringLine);

    Standard_EXPORT static Standard_Integer GetSurfSequNum(TCollection_AsciiString SurfSymb);

    Standard_EXPORT static bool ReadPrmt(const TCollection_AsciiString InputFileName);
    Standard_EXPORT static bool WriteCollisionFile();
    Standard_EXPORT static bool GenerateVoid();

    Standard_EXPORT static double GetTolerence();
    Standard_EXPORT static double GetAngleTolerance();
    Standard_EXPORT static double GetMinIptSolidVod();
    Standard_EXPORT static double GetMaxDiscLength();
    Standard_EXPORT static double GetMinDecomFaceArea();
    Standard_EXPORT static double GetMinVoidVol();

    Standard_EXPORT static unsigned int GetVoidDecomposeDepth();
    Standard_EXPORT static unsigned int GetInitCellNum();
    Standard_EXPORT static unsigned int GetInitSurfNum();
    Standard_EXPORT static unsigned int GetXResolution();
    Standard_EXPORT static unsigned int GetYResolution();
    Standard_EXPORT static unsigned int GetRResolution();
    Standard_EXPORT static unsigned int GetMaxSmplPntNum();
    Standard_EXPORT static unsigned int GetMinSmplPntNum();

    Standard_EXPORT static TCollection_AsciiString GetInputFileName();
    Standard_EXPORT static TCollection_AsciiString GetMatFile();
    Standard_EXPORT static TCollection_AsciiString GetDirectory();

    Standard_EXPORT static void SetMinMaxSmplPntNum(int iMinPnt,int iMaxPnt);

    Standard_EXPORT static void SetWriteCollisionFile(const bool & bWriteCollisionFile);
    Standard_EXPORT static void SetGenerateVoid(const bool & bVoidGenerate);

    Standard_EXPORT static void SetTolerence(const double & dTolerance);
    Standard_EXPORT static void SetAngleTolerance(const double & dAngleTolerance);
    Standard_EXPORT static void SetMinIptSolidVod(const double & dMinIptSolidVol);
    Standard_EXPORT static void SetMaxDiscLength(const double & dMaxDiscLength );
    Standard_EXPORT static void SetMinDecomFaceArea(const double & dMinDecomFaceArea);
    Standard_EXPORT static void SetMinVoidVol(const double & dMinVoidVol );

    Standard_EXPORT static void SetVoidDecomposeDepth(const unsigned int & iVoidDecomposeDepth);
    Standard_EXPORT static void SetInitCellNum(const unsigned int & iInitCellNum);
    Standard_EXPORT static void SetInitSurfNum(const unsigned int & iInitSurfNum);
    Standard_EXPORT static void SetXResolution(const unsigned int & iXResolution);
    Standard_EXPORT static void SetYResolution(const unsigned int & iYResolution);
    Standard_EXPORT static void SetRResolution(const unsigned int & iRResolution);
    Standard_EXPORT static void SetMaxSmplPntNum(const unsigned int & iMaxSmplPntNum);
    Standard_EXPORT static void SetMinSmplPntNum(const unsigned int & iMinSmplPntNum);

    Standard_EXPORT static void SetInputFileName(const TCollection_AsciiString &strInputFileName);
    Standard_EXPORT static void SetMatFile(const TCollection_AsciiString &strMatFile);
    Standard_EXPORT static void SetDirectory(const TCollection_AsciiString &strDirectory);
};

#endif // MCCADVOIDPRMT_HXX
