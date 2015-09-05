/****************************************************************************
** Meta object code from reading C++ file 'MCCADGUI_Dialogs.h'
**
** Created: Mon Aug 3 15:58:18 2015
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.4)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "MCCADGUI_Dialogs.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'MCCADGUI_Dialogs.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.4. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_MCCADGUI_MCardDialog[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
      11,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      22,   21,   21,   21, 0x0a,
      36,   21,   21,   21, 0x0a,
      50,   21,   21,   21, 0x0a,
      67,   21,   21,   21, 0x0a,
      74,   21,   21,   21, 0x0a,
      84,   21,   21,   21, 0x0a,
      95,   21,   21,   21, 0x0a,
     111,   21,   21,   21, 0x0a,
     125,   21,   21,   21, 0x0a,
     151,  139,   21,   21, 0x0a,
     194,   21,   21,   21, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_MCCADGUI_MCardDialog[] = {
    "MCCADGUI_MCardDialog\0\0AddMaterial()\0"
    "DelMaterial()\0ModifyMaterial()\0onOK()\0"
    "onApply()\0onCancel()\0onColorButton()\0"
    "SaveXMLFile()\0LoadXMLFile()\0item,colume\0"
    "ShowSelectedMaterial(QTreeWidgetItem*,int)\0"
    "onMatEdited()\0"
};

void MCCADGUI_MCardDialog::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        MCCADGUI_MCardDialog *_t = static_cast<MCCADGUI_MCardDialog *>(_o);
        switch (_id) {
        case 0: _t->AddMaterial(); break;
        case 1: _t->DelMaterial(); break;
        case 2: _t->ModifyMaterial(); break;
        case 3: _t->onOK(); break;
        case 4: _t->onApply(); break;
        case 5: _t->onCancel(); break;
        case 6: _t->onColorButton(); break;
        case 7: _t->SaveXMLFile(); break;
        case 8: _t->LoadXMLFile(); break;
        case 9: _t->ShowSelectedMaterial((*reinterpret_cast< QTreeWidgetItem*(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 10: _t->onMatEdited(); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData MCCADGUI_MCardDialog::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject MCCADGUI_MCardDialog::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_MCCADGUI_MCardDialog,
      qt_meta_data_MCCADGUI_MCardDialog, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &MCCADGUI_MCardDialog::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *MCCADGUI_MCardDialog::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *MCCADGUI_MCardDialog::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_MCCADGUI_MCardDialog))
        return static_cast<void*>(const_cast< MCCADGUI_MCardDialog*>(this));
    if (!strcmp(_clname, "Ui::MCCADGUI_MCardDialog"))
        return static_cast< Ui::MCCADGUI_MCardDialog*>(const_cast< MCCADGUI_MCardDialog*>(this));
    return QDialog::qt_metacast(_clname);
}

int MCCADGUI_MCardDialog::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 11)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 11;
    }
    return _id;
}
static const uint qt_meta_data_MCCADGUI_SetPropertyDialog[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       5,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      28,   27,   27,   27, 0x08,
      38,   27,   27,   27, 0x08,
      48,   27,   27,   27, 0x08,
      65,   27,   27,   27, 0x08,
      81,   27,   27,   27, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_MCCADGUI_SetPropertyDialog[] = {
    "MCCADGUI_SetPropertyDialog\0\0onClose()\0"
    "onApply()\0onEditMaterial()\0onColorButton()\0"
    "onSelectChg()\0"
};

void MCCADGUI_SetPropertyDialog::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        MCCADGUI_SetPropertyDialog *_t = static_cast<MCCADGUI_SetPropertyDialog *>(_o);
        switch (_id) {
        case 0: _t->onClose(); break;
        case 1: _t->onApply(); break;
        case 2: _t->onEditMaterial(); break;
        case 3: _t->onColorButton(); break;
        case 4: _t->onSelectChg(); break;
        default: ;
        }
    }
    Q_UNUSED(_a);
}

const QMetaObjectExtraData MCCADGUI_SetPropertyDialog::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject MCCADGUI_SetPropertyDialog::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_MCCADGUI_SetPropertyDialog,
      qt_meta_data_MCCADGUI_SetPropertyDialog, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &MCCADGUI_SetPropertyDialog::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *MCCADGUI_SetPropertyDialog::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *MCCADGUI_SetPropertyDialog::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_MCCADGUI_SetPropertyDialog))
        return static_cast<void*>(const_cast< MCCADGUI_SetPropertyDialog*>(this));
    if (!strcmp(_clname, "Ui::MCCADGUI_SetPropertyDialog"))
        return static_cast< Ui::MCCADGUI_SetPropertyDialog*>(const_cast< MCCADGUI_SetPropertyDialog*>(this));
    return QDialog::qt_metacast(_clname);
}

int MCCADGUI_SetPropertyDialog::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 5)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 5;
    }
    return _id;
}
static const uint qt_meta_data_MCCADGUI_ImportObjDialog[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      26,   25,   25,   25, 0x08,
      33,   25,   25,   25, 0x08,
      44,   25,   25,   25, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_MCCADGUI_ImportObjDialog[] = {
    "MCCADGUI_ImportObjDialog\0\0onOK()\0"
    "onCancel()\0onSelectChg()\0"
};

void MCCADGUI_ImportObjDialog::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        MCCADGUI_ImportObjDialog *_t = static_cast<MCCADGUI_ImportObjDialog *>(_o);
        switch (_id) {
        case 0: _t->onOK(); break;
        case 1: _t->onCancel(); break;
        case 2: _t->onSelectChg(); break;
        default: ;
        }
    }
    Q_UNUSED(_a);
}

const QMetaObjectExtraData MCCADGUI_ImportObjDialog::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject MCCADGUI_ImportObjDialog::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_MCCADGUI_ImportObjDialog,
      qt_meta_data_MCCADGUI_ImportObjDialog, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &MCCADGUI_ImportObjDialog::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *MCCADGUI_ImportObjDialog::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *MCCADGUI_ImportObjDialog::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_MCCADGUI_ImportObjDialog))
        return static_cast<void*>(const_cast< MCCADGUI_ImportObjDialog*>(this));
    if (!strcmp(_clname, "Ui::MCCADGUI_ImportObjDialog"))
        return static_cast< Ui::MCCADGUI_ImportObjDialog*>(const_cast< MCCADGUI_ImportObjDialog*>(this));
    return QDialog::qt_metacast(_clname);
}

int MCCADGUI_ImportObjDialog::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 3)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 3;
    }
    return _id;
}
static const uint qt_meta_data_MCCADGUI_TransparencyDlg[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      26,   25,   25,   25, 0x08,
      42,   25,   25,   25, 0x08,
      63,   25,   25,   25, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_MCCADGUI_TransparencyDlg[] = {
    "MCCADGUI_TransparencyDlg\0\0ClickOnCancel()\0"
    "ValueHasChanged(int)\0accept()\0"
};

void MCCADGUI_TransparencyDlg::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        MCCADGUI_TransparencyDlg *_t = static_cast<MCCADGUI_TransparencyDlg *>(_o);
        switch (_id) {
        case 0: _t->ClickOnCancel(); break;
        case 1: _t->ValueHasChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 2: _t->accept(); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData MCCADGUI_TransparencyDlg::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject MCCADGUI_TransparencyDlg::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_MCCADGUI_TransparencyDlg,
      qt_meta_data_MCCADGUI_TransparencyDlg, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &MCCADGUI_TransparencyDlg::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *MCCADGUI_TransparencyDlg::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *MCCADGUI_TransparencyDlg::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_MCCADGUI_TransparencyDlg))
        return static_cast<void*>(const_cast< MCCADGUI_TransparencyDlg*>(this));
    return QDialog::qt_metacast(_clname);
}

int MCCADGUI_TransparencyDlg::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 3)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 3;
    }
    return _id;
}
static const uint qt_meta_data_MCCADGUI_TransformDialog[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      26,   25,   25,   25, 0x08,
      36,   25,   25,   25, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_MCCADGUI_TransformDialog[] = {
    "MCCADGUI_TransformDialog\0\0onClose()\0"
    "onApply()\0"
};

void MCCADGUI_TransformDialog::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        MCCADGUI_TransformDialog *_t = static_cast<MCCADGUI_TransformDialog *>(_o);
        switch (_id) {
        case 0: _t->onClose(); break;
        case 1: _t->onApply(); break;
        default: ;
        }
    }
    Q_UNUSED(_a);
}

const QMetaObjectExtraData MCCADGUI_TransformDialog::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject MCCADGUI_TransformDialog::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_MCCADGUI_TransformDialog,
      qt_meta_data_MCCADGUI_TransformDialog, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &MCCADGUI_TransformDialog::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *MCCADGUI_TransformDialog::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *MCCADGUI_TransformDialog::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_MCCADGUI_TransformDialog))
        return static_cast<void*>(const_cast< MCCADGUI_TransformDialog*>(this));
    if (!strcmp(_clname, "Ui::MCCADGUI_TransformDialog"))
        return static_cast< Ui::MCCADGUI_TransformDialog*>(const_cast< MCCADGUI_TransformDialog*>(this));
    return QDialog::qt_metacast(_clname);
}

int MCCADGUI_TransformDialog::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 2)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 2;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
