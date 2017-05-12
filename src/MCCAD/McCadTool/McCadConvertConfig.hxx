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

class McCadConvertConfig
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
    static double m_fXResolution;                /**< Resolution of X axis */
    static double m_fYResolution;                /**< Resolution of Y axis */
    static double m_fRResolution;                /**< R direction */

    static unsigned int m_iMaxSmplPntNum;        /**< Max sample point number */
    static unsigned int m_iMinSmplPntNum;        /**< Min sample point number */

    static TCollection_AsciiString m_strInputFileName;   /**< Input file name */
    static TCollection_AsciiString m_strMatFile;         /**< Material file */
    static TCollection_AsciiString m_strDirectory;       /**< Working directory */

    static McCadGeom_Unit m_units;                       /**< Units of CAD model*/
    static map<TCollection_AsciiString,Standard_Integer> m_SurfSeq;

public:
//qiu add Standard_EXPORT
    Standard_EXPORT static void MissmatchMessage(TCollection_AsciiString &keyword, TCollection_AsciiString &parameter);
    Standard_EXPORT static void SetSurfSequ();
    Standard_EXPORT void AssignPrmt(const TCollection_AsciiString StringLine);

    Standard_EXPORT static Standard_Integer GetSurfSequNum(TCollection_AsciiString SurfSymb);

    Standard_EXPORT static bool ReadPrmt(const TCollection_AsciiString InputFileName);
//qiu use this way to avoid error in Windows version
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
    //qiu    Standard_EXPORT static unsigned int GetXResolution();
        Standard_EXPORT static double GetXResolution();
    //qiu    Standard_EXPORT static unsigned int GetYResolution();
        Standard_EXPORT static double GetYResolution();
    //qiu    Standard_EXPORT static unsigned int GetRResolution();
        Standard_EXPORT static double GetRResolution();
        Standard_EXPORT static unsigned int GetMaxSmplPntNum();
        Standard_EXPORT static unsigned int GetMinSmplPntNum();

        Standard_EXPORT static TCollection_AsciiString GetInputFileName();
        Standard_EXPORT static TCollection_AsciiString GetMatFile();
        Standard_EXPORT static TCollection_AsciiString GetDirectory();
/*
    Standard_EXPORT static bool WriteCollisionFile(){return m_bWriteCollisionFile;};
    Standard_EXPORT static bool GenerateVoid(){return m_bVoidGenerate;};

    Standard_EXPORT static double GetTolerence(){return m_dTolerance;};
    Standard_EXPORT static double GetAngleTolerance(){return m_dAngleTolerance;};
    Standard_EXPORT static double GetMinIptSolidVod(){return m_dMinIptSolidVol;};
    Standard_EXPORT static double GetMaxDiscLength(){return m_dMaxDiscLength;};
    Standard_EXPORT static double GetMinDecomFaceArea(){return m_dMinDecomFaceArea;};
    Standard_EXPORT static double GetMinVoidVol(){return m_dMinVoidVol;};

    Standard_EXPORT static unsigned int GetVoidDecomposeDepth(){return m_iVoidDecomposeDepth;};
    Standard_EXPORT static unsigned int GetInitCellNum(){return m_iInitCellNum;};
    Standard_EXPORT static unsigned int GetInitSurfNum(){return m_iInitSurfNum;};
    Standard_EXPORT static double GetXResolution(){return m_fXResolution;};
    Standard_EXPORT static double GetYResolution(){return m_fYResolution;};
    Standard_EXPORT static double GetRResolution(){return m_fRResolution;};
    Standard_EXPORT static unsigned int GetMaxSmplPntNum(){return m_iMaxSmplPntNum;};
    Standard_EXPORT static unsigned int GetMinSmplPntNum(){return m_iMinSmplPntNum;};

    Standard_EXPORT static TCollection_AsciiString GetInputFileName(){return m_strInputFileName;};
    Standard_EXPORT static TCollection_AsciiString GetMatFile(){return m_strMatFile;};
    Standard_EXPORT static TCollection_AsciiString GetDirectory(){return m_strDirectory;};
    */

    Standard_EXPORT static void SetMinMaxSmplPntNum(int iMinPnt,int iMaxPnt);

    //qiu add Set*** functions
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
    Standard_EXPORT static void SetXResolution(const double & fXResolution);
    Standard_EXPORT static void SetYResolution(const double & fYResolution);
    Standard_EXPORT static void SetRResolution(const double & fRResolution);
    Standard_EXPORT static void SetMaxSmplPntNum(const unsigned int & iMaxSmplPntNum);
    Standard_EXPORT static void SetMinSmplPntNum(const unsigned int & iMinSmplPntNum);

    Standard_EXPORT static void SetInputFileName(const TCollection_AsciiString &strInputFileName);
    Standard_EXPORT static void SetMatFile(const TCollection_AsciiString &strMatFile);
    Standard_EXPORT static void SetDirectory(const TCollection_AsciiString &strDirectory);

};

#endif // MCCADVOIDPRMT_HXX
