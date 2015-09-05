/****************************************************************************
** Meta object code from reading C++ file 'QMcCad_Application.h'
**
** Created: Wed Dec 10 12:42:36 2008
**      by: The Qt Meta Object Compiler version 59 (Qt 4.1.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "QMcCad_Application.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'QMcCad_Application.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 59
#error "This file was generated using the moc from 4.1.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

static const uint qt_meta_data_QMcCad_Application[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
      19,   10, // methods
       0,    0, // properties
       0,    0, // enums/sets

 // signals: signature, parameters, type, tag, flags
      20,   19,   19,   19, 0x05,

 // slots: signature, parameters, type, tag, flags
      46,   19,   19,   19, 0x08,
      60,   19,   19,   19, 0x08,
      75,   19,   19,   19, 0x08,
      91,   19,   19,   19, 0x08,
     106,   19,   19,   19, 0x08,
     123,   19,   19,   19, 0x08,
     138,   19,   19,   19, 0x08,
     150,   19,   19,   19, 0x08,
     161,   19,   19,   19, 0x08,
     190,   19,   19,   19, 0x08,
     214,   19,   19,   19, 0x08,
     263,  261,   19,   19, 0x08,
     295,   19,   19,   19, 0x08,
     312,   19,   19,   19, 0x08,
     324,   19,   19,   19, 0x08,
     335,   19,   19,   19, 0x08,
     353,   19,   19,   19, 0x08,
     375,   19,   19,   19, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_QMcCad_Application[] = {
    "QMcCad_Application\0\0SignalResetTransparency()\0SlotFileNew()\0"
    "SlotFileOpen()\0SlotFileClose()\0SlotFileSave()\0SlotFileSaveAs()\0"
    "SlotHardcopy()\0SlotPrint()\0SlotQuit()\0SlotTransparencyChanged(int)\0"
    "SlotApplyTransparency()\0SlotMaterialSelected(Graphic3d_NameOfMaterial)\0"
    ",\0SlotUpdataProgress(int,QString)\0OpenRecentFile()\0SlotAbout()\0"
    "SlotHelp()\0SlotUpdateMenus()\0SlotEditorClosed(int)\0"
    "SlotUpdateMainTitle()\0"
};

const QMetaObject QMcCad_Application::staticMetaObject = {
    { &QMainWindow::staticMetaObject, qt_meta_stringdata_QMcCad_Application,
      qt_meta_data_QMcCad_Application, 0 }
};

const QMetaObject *QMcCad_Application::metaObject() const
{
    return &staticMetaObject;
}

void *QMcCad_Application::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QMcCad_Application))
	return static_cast<void*>(const_cast<QMcCad_Application*>(this));
    if (!strcmp(_clname, "McCadAEV_Application"))
	return static_cast<McCadAEV_Application*>(const_cast<QMcCad_Application*>(this));
    return QMainWindow::qt_metacast(_clname);
}

int QMcCad_Application::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: SignalResetTransparency(); break;
        case 1: SlotFileNew(); break;
        case 2: SlotFileOpen(); break;
        case 3: SlotFileClose(); break;
        case 4: SlotFileSave(); break;
        case 5: SlotFileSaveAs(); break;
        case 6: SlotHardcopy(); break;
        case 7: SlotPrint(); break;
        case 8: SlotQuit(); break;
        case 9: SlotTransparencyChanged(*reinterpret_cast< int(*)>(_a[1])); break;
        case 10: SlotApplyTransparency(); break;
        case 11: SlotMaterialSelected(*reinterpret_cast< Graphic3d_NameOfMaterial(*)>(_a[1])); break;
        case 12: SlotUpdataProgress(*reinterpret_cast< int(*)>(_a[1]),*reinterpret_cast< QString(*)>(_a[2])); break;
        case 13: OpenRecentFile(); break;
        case 14: SlotAbout(); break;
        case 15: SlotHelp(); break;
        case 16: SlotUpdateMenus(); break;
        case 17: SlotEditorClosed(*reinterpret_cast< int(*)>(_a[1])); break;
        case 18: SlotUpdateMainTitle(); break;
        }
        _id -= 19;
    }
    return _id;
}

// SIGNAL 0
void QMcCad_Application::SignalResetTransparency()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}
