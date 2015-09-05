/****************************************************************************
** Meta object code from reading C++ file 'MCCADGUI_GenPartDialog.h'
**
** Created: Wed Feb 27 09:41:14 2013
**      by: The Qt Meta Object Compiler version 62 (Qt 4.6.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "MCCADGUI_GenPartDialog.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'MCCADGUI_GenPartDialog.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.6.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_MCCADGUI_GenPartDialog[] = {

 // content:
       4,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

       0        // eod
};

static const char qt_meta_stringdata_MCCADGUI_GenPartDialog[] = {
    "MCCADGUI_GenPartDialog\0"
};

const QMetaObject MCCADGUI_GenPartDialog::staticMetaObject = {
    { &LightApp_Dialog::staticMetaObject, qt_meta_stringdata_MCCADGUI_GenPartDialog,
      qt_meta_data_MCCADGUI_GenPartDialog, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &MCCADGUI_GenPartDialog::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *MCCADGUI_GenPartDialog::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *MCCADGUI_GenPartDialog::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_MCCADGUI_GenPartDialog))
        return static_cast<void*>(const_cast< MCCADGUI_GenPartDialog*>(this));
    if (!strcmp(_clname, "Ui::GenPartDialog"))
        return static_cast< Ui::GenPartDialog*>(const_cast< MCCADGUI_GenPartDialog*>(this));
    return LightApp_Dialog::qt_metacast(_clname);
}

int MCCADGUI_GenPartDialog::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = LightApp_Dialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    return _id;
}
QT_END_MOC_NAMESPACE
