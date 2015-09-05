/****************************************************************************
** Meta object code from reading C++ file 'CADProperties.h'
**
** Created: Wed Dec 10 16:13:31 2008
**      by: The Qt Meta Object Compiler version 59 (Qt 4.1.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "CADProperties.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'CADProperties.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 59
#error "This file was generated using the moc from 4.1.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

static const uint qt_meta_data_CADProperties[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   10, // methods
       0,    0, // properties
       0,    0, // enums/sets

 // signals: signature, parameters, type, tag, flags
      15,   14,   14,   14, 0x05,

 // slots: signature, parameters, type, tag, flags
      58,   14,   14,   14, 0x08,
      72,   14,   14,   14, 0x08,
      99,   14,   14,   14, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_CADProperties[] = {
    "CADProperties\0\0MaterialSelected(Graphic3d_NameOfMaterial)\0"
    "ResetSlider()\0SlotItemSelectionChanged()\0"
    "SlotMaterialSelected(QString)\0"
};

const QMetaObject CADProperties::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_CADProperties,
      qt_meta_data_CADProperties, 0 }
};

const QMetaObject *CADProperties::metaObject() const
{
    return &staticMetaObject;
}

void *CADProperties::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_CADProperties))
	return static_cast<void*>(const_cast<CADProperties*>(this));
    return QWidget::qt_metacast(_clname);
}

int CADProperties::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: MaterialSelected(*reinterpret_cast< Graphic3d_NameOfMaterial(*)>(_a[1])); break;
        case 1: ResetSlider(); break;
        case 2: SlotItemSelectionChanged(); break;
        case 3: SlotMaterialSelected(*reinterpret_cast< QString(*)>(_a[1])); break;
        }
        _id -= 4;
    }
    return _id;
}

// SIGNAL 0
void CADProperties::MaterialSelected(Graphic3d_NameOfMaterial _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
