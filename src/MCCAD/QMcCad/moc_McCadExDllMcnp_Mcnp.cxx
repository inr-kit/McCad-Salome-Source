/****************************************************************************
** Meta object code from reading C++ file 'McCadExDllMcnp_Mcnp.hxx'
**
** Created: Tue Aug 12 16:54:49 2008
**      by: The Qt Meta Object Compiler version 59 (Qt 4.1.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../inc/McCadExDllMcnp_Mcnp.hxx"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'McCadExDllMcnp_Mcnp.hxx' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 59
#error "This file was generated using the moc from 4.1.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

static const uint qt_meta_data_McCadExDllMcnp_Mcnp[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   10, // methods
       0,    0, // properties
       0,    0, // enums/sets

 // signals: signature, parameters, type, tag, flags
      21,   20,   20,   20, 0x05,

 // slots: signature, parameters, type, tag, flags
      38,   20,   20,   20, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_McCadExDllMcnp_Mcnp[] = {
    "McCadExDllMcnp_Mcnp\0\0ExportFinished()\0ContinueExportFromDocument()\0"
};

const QMetaObject McCadExDllMcnp_Mcnp::staticMetaObject = {
    { &McCadEXPlug_ExchangePlugin::staticMetaObject, qt_meta_stringdata_McCadExDllMcnp_Mcnp,
      qt_meta_data_McCadExDllMcnp_Mcnp, 0 }
};

const QMetaObject *McCadExDllMcnp_Mcnp::metaObject() const
{
    return &staticMetaObject;
}

void *McCadExDllMcnp_Mcnp::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_McCadExDllMcnp_Mcnp))
	return static_cast<void*>(const_cast<McCadExDllMcnp_Mcnp*>(this));
    if (!strcmp(_clname, "QWidget"))
	return static_cast<QWidget*>(const_cast<McCadExDllMcnp_Mcnp*>(this));
    return McCadEXPlug_ExchangePlugin::qt_metacast(_clname);
}

int McCadExDllMcnp_Mcnp::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = McCadEXPlug_ExchangePlugin::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: ExportFinished(); break;
        case 1: ContinueExportFromDocument(); break;
        }
        _id -= 2;
    }
    return _id;
}

// SIGNAL 0
void McCadExDllMcnp_Mcnp::ExportFinished()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}
