/****************************************************************************
** Meta object code from reading C++ file 'MCCADGUI.h'
**
** Created: Mon Aug 3 15:40:05 2015
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.4)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "MCCADGUI.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'MCCADGUI.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.4. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_MCCADGUI[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
      17,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: signature, parameters, type, tag, flags
      10,    9,    9,    9, 0x05,

 // slots: signature, parameters, type, tag, flags
      27,    9,   22,    9, 0x0a,
      57,    9,   22,    9, 0x0a,
      85,    9,    9,    9, 0x0a,
     110,    9,    9,    9, 0x0a,
     129,    9,    9,    9, 0x09,
     142,    9,    9,    9, 0x09,
     154,    9,    9,    9, 0x09,
     168,    9,    9,    9, 0x09,
     185,    9,    9,    9, 0x09,
     210,    9,    9,    9, 0x09,
     231,    9,    9,    9, 0x09,
     246,    9,    9,    9, 0x09,
     263,    9,    9,    9, 0x09,
     281,    9,    9,    9, 0x09,
     299,    9,    9,    9, 0x09,
     318,    9,    9,    9, 0x09,

       0        // eod
};

static const char qt_meta_stringdata_MCCADGUI[] = {
    "MCCADGUI\0\0selectChg()\0bool\0"
    "deactivateModule(SUIT_Study*)\0"
    "activateModule(SUIT_Study*)\0"
    "studyClosed(SUIT_Study*)\0selectionChanged()\0"
    "OnUnitTest()\0onProcess()\0onOperation()\0"
    "onImportExport()\0onDisplayGEOMObjInPVIS()\0"
    "OnDisplayerCommand()\0OnExperiment()\0"
    "onEditMaterial()\0onSetProproties()\0"
    "onImportGEOMObj()\0onImportSMESHObj()\0"
    "onTransform()\0"
};

void MCCADGUI::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        MCCADGUI *_t = static_cast<MCCADGUI *>(_o);
        switch (_id) {
        case 0: _t->selectChg(); break;
        case 1: { bool _r = _t->deactivateModule((*reinterpret_cast< SUIT_Study*(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = _r; }  break;
        case 2: { bool _r = _t->activateModule((*reinterpret_cast< SUIT_Study*(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = _r; }  break;
        case 3: _t->studyClosed((*reinterpret_cast< SUIT_Study*(*)>(_a[1]))); break;
        case 4: _t->selectionChanged(); break;
        case 5: _t->OnUnitTest(); break;
        case 6: _t->onProcess(); break;
        case 7: _t->onOperation(); break;
        case 8: _t->onImportExport(); break;
        case 9: _t->onDisplayGEOMObjInPVIS(); break;
        case 10: _t->OnDisplayerCommand(); break;
        case 11: _t->OnExperiment(); break;
        case 12: _t->onEditMaterial(); break;
        case 13: _t->onSetProproties(); break;
        case 14: _t->onImportGEOMObj(); break;
        case 15: _t->onImportSMESHObj(); break;
        case 16: _t->onTransform(); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData MCCADGUI::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject MCCADGUI::staticMetaObject = {
    { &SalomeApp_Module::staticMetaObject, qt_meta_stringdata_MCCADGUI,
      qt_meta_data_MCCADGUI, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &MCCADGUI::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *MCCADGUI::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *MCCADGUI::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_MCCADGUI))
        return static_cast<void*>(const_cast< MCCADGUI*>(this));
    return SalomeApp_Module::qt_metacast(_clname);
}

int MCCADGUI::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = SalomeApp_Module::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 17)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 17;
    }
    return _id;
}

// SIGNAL 0
void MCCADGUI::selectChg()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}
QT_END_MOC_NAMESPACE
