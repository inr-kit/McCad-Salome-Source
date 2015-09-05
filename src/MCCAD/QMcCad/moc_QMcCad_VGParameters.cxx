/****************************************************************************
** Meta object code from reading C++ file 'QMcCad_VGParameters.h'
**
** Created: Fri Jul 18 13:41:18 2008
**      by: The Qt Meta Object Compiler version 59 (Qt 4.1.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "QMcCad_VGParameters.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'QMcCad_VGParameters.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 59
#error "This file was generated using the moc from 4.1.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

static const uint qt_meta_data_QMcCad_VGParameters[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
       0,    0, // properties
       0,    0, // enums/sets

       0        // eod
};

static const char qt_meta_stringdata_QMcCad_VGParameters[] = {
    "QMcCad_VGParameters\0"
};

const QMetaObject QMcCad_VGParameters::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_QMcCad_VGParameters,
      qt_meta_data_QMcCad_VGParameters, 0 }
};

const QMetaObject *QMcCad_VGParameters::metaObject() const
{
    return &staticMetaObject;
}

void *QMcCad_VGParameters::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QMcCad_VGParameters))
	return static_cast<void*>(const_cast<QMcCad_VGParameters*>(this));
    return QDialog::qt_metacast(_clname);
}

int QMcCad_VGParameters::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    return _id;
}
