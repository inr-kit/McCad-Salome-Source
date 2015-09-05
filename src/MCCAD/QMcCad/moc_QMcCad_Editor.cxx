/****************************************************************************
** Meta object code from reading C++ file 'QMcCad_Editor.h'
**
** Created: Fri Nov 14 10:18:23 2008
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
      18,   10, // methods
       0,    0, // properties
       0,    0, // enums/sets

 // signals: signature, parameters, type, tag, flags
      15,   14,   14,   14, 0x05,
      35,   14,   14,   14, 0x05,
      58,   14,   14,   14, 0x05,
      82,   14,   14,   14, 0x05,
     100,   14,   14,   14, 0x05,
     109,   14,   14,   14, 0x05,
     126,  124,   14,   14, 0x05,

 // slots: signature, parameters, type, tag, flags
     154,   14,   14,   14, 0x0a,
     175,   14,   14,   14, 0x0a,
     195,   14,   14,   14, 0x0a,
     214,   14,   14,   14, 0x0a,
     233,   14,   14,   14, 0x0a,
     252,   14,   14,   14, 0x0a,
     272,   14,   14,   14, 0x0a,
     296,   14,   14,   14, 0x0a,
     321,   14,   14,   14, 0x0a,
     345,  343,   14,   14, 0x0a,
     371,  343,   14,   14, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_QMcCad_Editor[] = {
    "QMcCad_Editor\0\0decompositionDone()\0keyPressed(QKeyEvent*)\0"
    "keyReleased(QKeyEvent*)\0updateMainTitle()\0saveAs()\0destroyed(int)\0,\0"
    "updateProgress(int,QString)\0SlotDecomposeSolid()\0SlotGenerateVoids()\0"
    "SlotExplodeModel()\0SlotViewModified()\0SlotReadVGParams()\0"
    "updateProgress(int)\0SlotDecompositionDone()\0SlotVoidGenerationDone()\0"
    "SlotSetVGParameters()\0e\0keyPressEvent(QKeyEvent*)\0"
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
        case 0: decompositionDone(); break;
        case 1: keyPressed(*reinterpret_cast< QKeyEvent*(*)>(_a[1])); break;
        case 2: keyReleased(*reinterpret_cast< QKeyEvent*(*)>(_a[1])); break;
        case 3: updateMainTitle(); break;
        case 4: saveAs(); break;
        case 5: destroyed(*reinterpret_cast< int(*)>(_a[1])); break;
        case 6: updateProgress(*reinterpret_cast< int(*)>(_a[1]),*reinterpret_cast< QString(*)>(_a[2])); break;
        case 7: SlotDecomposeSolid(); break;
        case 8: SlotGenerateVoids(); break;
        case 9: SlotExplodeModel(); break;
        case 10: SlotViewModified(); break;
        case 11: SlotReadVGParams(); break;
        case 12: updateProgress(*reinterpret_cast< int(*)>(_a[1])); break;
        case 13: SlotDecompositionDone(); break;
        case 14: SlotVoidGenerationDone(); break;
        case 15: SlotSetVGParameters(); break;
        case 16: keyPressEvent(*reinterpret_cast< QKeyEvent*(*)>(_a[1])); break;
        case 17: keyReleaseEvent(*reinterpret_cast< QKeyEvent*(*)>(_a[1])); break;
        }
        _id -= 18;
    }
    return _id;
}

// SIGNAL 0
void QMcCad_Editor::decompositionDone()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}

// SIGNAL 1
void QMcCad_Editor::keyPressed(QKeyEvent * _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void QMcCad_Editor::keyReleased(QKeyEvent * _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void QMcCad_Editor::updateMainTitle()
{
    QMetaObject::activate(this, &staticMetaObject, 3, 0);
}

// SIGNAL 4
void QMcCad_Editor::saveAs()
{
    QMetaObject::activate(this, &staticMetaObject, 4, 0);
}

// SIGNAL 5
void QMcCad_Editor::destroyed(int _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 5, _a);
}

// SIGNAL 6
void QMcCad_Editor::updateProgress(int _t1, QString _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 6, _a);
}
