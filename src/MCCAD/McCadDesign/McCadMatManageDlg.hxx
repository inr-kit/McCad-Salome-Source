/*
 * McCadMatManageDlg.hxx
 *
 *  Created on: Oct 22, 2013
 *      Author: Lei
 */

#ifndef MCCADMATMANAGEDLG_HXX
#define MCCADMATMANAGEDLG_HXX

#include <ui_QMcCadGeomeTree_MCardDialog.h>
#include <QMcCad_Editor.h>

using namespace std;

class McCadMatManageDlg : public QDialog
{    
    Q_OBJECT

public:
    McCadMatManageDlg(QWidget* theParent = NULL);
    void LoadMatData();
    void SetCurrentEditor(QMcCad_Editor* theEditor);
    bool CheckInput(QString &strErrorMsg);
    void ClearData();
    void SetEditGroup(QString theGroupName);
    void LoadMaterials(QString strSelectedGrp);
    void DisplayMaterial(int &iItem);
    void Refresh();

    void SaveXMLFile(QString &strFileName);
    void LoadXMLFile(QString &strFileName);

public slots:
    void AddMaterial();
    void DelMaterial();
    void ModifyMaterial();
    void SaveMaterial();
    void ShowSelectedMaterial(QTreeWidgetItem*item, int colume);

private:

    Ui::MCardDialog m_MatDialog;
    QMcCad_Editor* m_Editor;

    unsigned int  m_ID;
    double        m_Density;
    QString       m_Name;
    QString       m_Card;

    QString       m_EditGroupName;          // The group name of being edited group

    Standard_Integer m_NumOfMat;            // Number of Materials
    vector<Standard_Integer> m_MatIDList;   // The list stores the ID of materials.
};

#endif // MCCADMATMANAGEDLG_HXX
