/****************************************************************************
** Meta object code from reading C++ file 'QMcCad_Editor.h'
**
** Created: Wed Apr 30 11:17:06 2008
**      by: The Qt Meta Object Compiler version 59 (Qt 4.1.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "QMcCad_Editor.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'QMcCad_Editor.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 59
#error "This file was generated using the moc from 4.1.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

static const uint qt_meta_data_QMcCad_Editor[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
      13,   10, // methods
       0,    0, // properties
       0,    0, // enums/sets

 // signals: signature, parameters, type, tag, flags
      15,   14,   14,   14, 0x05,
      38,   14,   14,   14, 0x05,
      62,   14,   14,   14, 0x05,

 // slots: signature, parameters, type, tag, flags
      80,   14,   14,   14, 0x0a,
     101,   14,   14,   14, 0x0a,
     121,   14,   14,   14, 0x0a,
     140,   14,   14,   14, 0x0a,
     159,   14,   14,   14, 0x0a,
     177,   14,   14,   14, 0x0a,
     196,   14,   14,   14, 0x0a,
     203,   14,   14,   14, 0x0a,
     214,  212,   14,   14, 0x0a,
     240,  212,   14,   14, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_QMcCad_Editor[] = {
    "QMcCad_Editor\0\0keyPressed(QKeyEvent*)\0keyReleased(QKeyEvent*)\0"
    "updateMainTitle()\0SlotDecomposeModel()\0SlotGenerateVoids()\0"
    "SlotConvertModel()\0SlotExplodeModel()\0SlotMergeModels()\0"
    "SlotViewModified()\0goOn()\0goOnVG()\0e\0keyPressEvent(QKeyEvent*)\0"
    "keyReleaseEvent(QKeyEvent*)\0"
};

const QMetaObject QMcCad_Editor::staticMetaObject = {
    { &QMainWindow::staticMetaObject, qt_meta_stringdata_QMcCad_Editor,
      qt_meta_data_QMcCad_Editor, 0 }
};

const QMetaObject *QMcCad_Editor::metaObject() const
{
    return &staticMetaObject;
}

void *QMcCad_Editor::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QMcCad_Editor))
	return static_cast<void*>(const_cast<QMcCad_Editor*>(this));
    if (!strcmp(_clname, "McCadAEV_Editor"))
	return static_cast<McCadAEV_Editor*>(const_cast<QMcCad_Editor*>(this));
    return QMainWindow::qt_metacast(_clname);
}

int QMcCad_Editor::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: keyPressed(*reinterpret_cast< QKeyEvent*(*)>(_a[1])); break;
        case 1: keyReleased(*reinterpret_cast< QKeyEvent*(*)>(_a[1])); break;
        case 2: updateMainTitle(); break;
        case 3: SlotDecomposeModel(); break;
        case 4: SlotGenerateVoids(); break;
        case 5: SlotConvertModel(); break;
        case 6: SlotExplodeModel(); break;
        case 7: SlotMergeModels(); break;
        case 8: SlotViewModified(); break;
        case 9: goOn(); break;
        case 10: goOnVG(); break;
        case 11: keyPressEvent(*reinterpret_cast< QKeyEvent*(*)>(_a[1])); break;
        case 12: keyReleaseEvent(*reinterpret_cast< QKeyEvent*(*)>(_a[1])); break;
        }
        _id -= 13;
    }
    return _id;
}

// SIGNAL 0
void QMcCad_Editor::keyPressed(QKeyEvent * _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void QMcCad_Editor::keyReleased(QKeyEvent * _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void QMcCad_Editor::updateMainTitle()
{
    QMetaObject::activate(this, &staticMetaObject, 2, 0);
}
