/****************************************************************************
** Meta object code from reading C++ file 'QMcCad_ActionFactory.h'
**
** Created: Fri Nov 14 10:18:23 2008
**      by: The Qt Meta Object Compiler version 59 (Qt 4.1.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "QMcCad_ActionFactory.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'QMcCad_ActionFactory.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 59
#error "This file was generated using the moc from 4.1.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

static const uint qt_meta_data_QMcCad_ActionFactory[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
       0,    0, // properties
       0,    0, // enums/sets

       0        // eod
};

static const char qt_meta_stringdata_QMcCad_ActionFactory[] = {
    "QMcCad_ActionFactory\0"
};

const QMetaObject QMcCad_ActionFactory::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_QMcCad_ActionFactory,
      qt_meta_data_QMcCad_ActionFactory, 0 }
};

const QMetaObject *QMcCad_ActionFactory::metaObject() const
{
    return &staticMetaObject;
}

void *QMcCad_ActionFactory::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QMcCad_ActionFactory))
	return static_cast<void*>(const_cast<QMcCad_ActionFactory*>(this));
    return QObject::qt_metacast(_clname);
}

int QMcCad_ActionFactory::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    return _id;
}
