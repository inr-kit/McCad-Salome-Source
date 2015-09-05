/****************************************************************************
** Meta object code from reading C++ file 'QMcCad_Application.h'
**
** Created: Wed Apr 30 11:17:05 2008
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
      14,   10, // methods
       0,    0, // properties
       0,    0, // enums/sets

 // slots: signature, parameters, type, tag, flags
      20,   19,   19,   19, 0x08,
      34,   19,   19,   19, 0x08,
      49,   19,   19,   19, 0x08,
      65,   19,   19,   19, 0x08,
      80,   19,   19,   19, 0x08,
      97,   19,   19,   19, 0x08,
     112,   19,   19,   19, 0x08,
     124,   19,   19,   19, 0x08,
     135,   19,   19,   19, 0x08,
     152,   19,   19,   19, 0x08,
     164,   19,   19,   19, 0x08,
     175,   19,   19,   19, 0x08,
     193,   19,   19,   19, 0x08,
     212,   19,   19,   19, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_QMcCad_Application[] = {
    "QMcCad_Application\0\0SlotFileNew()\0SlotFileOpen()\0SlotFileClose()\0"
    "SlotFileSave()\0SlotFileSaveAs()\0SlotHardcopy()\0SlotPrint()\0"
    "SlotQuit()\0OpenRecentFile()\0SlotAbout()\0SlotHelp()\0"
    "SlotUpdateMenus()\0SlotEditorClosed()\0SlotUpdateMainTitle()\0"
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
        case 0: SlotFileNew(); break;
        case 1: SlotFileOpen(); break;
        case 2: SlotFileClose(); break;
        case 3: SlotFileSave(); break;
        case 4: SlotFileSaveAs(); break;
        case 5: SlotHardcopy(); break;
        case 6: SlotPrint(); break;
        case 7: SlotQuit(); break;
        case 8: OpenRecentFile(); break;
        case 9: SlotAbout(); break;
        case 10: SlotHelp(); break;
        case 11: SlotUpdateMenus(); break;
        case 12: SlotEditorClosed(); break;
        case 13: SlotUpdateMainTitle(); break;
        }
        _id -= 14;
    }
    return _id;
}
