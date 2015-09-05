/****************************************************************************
** Meta object code from reading C++ file 'QMcCad_View.h'
**
** Created: Wed Apr 30 11:17:07 2008
**      by: The Qt Meta Object Compiler version 59 (Qt 4.1.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "QMcCad_View.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'QMcCad_View.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 59
#error "This file was generated using the moc from 4.1.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

static const uint qt_meta_data_QMcCad_View[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
      14,   10, // methods
       0,    0, // properties
       0,    0, // enums/sets

 // signals: signature, parameters, type, tag, flags
      13,   12,   12,   12, 0x05,
      28,   12,   12,   12, 0x05,

 // slots: signature, parameters, type, tag, flags
      45,   43,   12,   12, 0x0a,
      75,   43,   12,   12, 0x0a,
     107,   43,   12,   12, 0x0a,
     136,   12,   12,   12, 0x0a,
     156,   12,   12,   12, 0x0a,
     176,   12,   12,   12, 0x0a,
     203,   12,   12,   12, 0x0a,
     231,   43,   12,   12, 0x0a,
     256,   43,   12,   12, 0x0a,
     282,   43,   12,   12, 0x0a,
     310,   12,   12,   12, 0x0a,
     325,   12,   12,   12, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_QMcCad_View[] = {
    "QMcCad_View\0\0InitFinished()\0viewModified()\0e\0"
    "mousePressEvent(QMouseEvent*)\0mouseReleaseEvent(QMouseEvent*)\0"
    "mouseMoveEvent(QMouseEvent*)\0LeaveEvent(QEvent*)\0EnterEvent(QEvent*)\0"
    "FocusInEvent(QFocusEvent*)\0FocusOutEvent(QFocusEvent*)\0"
    "wheelEvent(QWheelEvent*)\0keyPressEvent(QKeyEvent*)\0"
    "keyReleaseEvent(QKeyEvent*)\0SlotInitTask()\0PreExecute()\0"
};

const QMetaObject QMcCad_View::staticMetaObject = {
    { &QGLWidget::staticMetaObject, qt_meta_stringdata_QMcCad_View,
      qt_meta_data_QMcCad_View, 0 }
};

const QMetaObject *QMcCad_View::metaObject() const
{
    return &staticMetaObject;
}

void *QMcCad_View::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QMcCad_View))
	return static_cast<void*>(const_cast<QMcCad_View*>(this));
    if (!strcmp(_clname, "McCadCom_CasView"))
	return static_cast<McCadCom_CasView*>(const_cast<QMcCad_View*>(this));
    return QGLWidget::qt_metacast(_clname);
}

int QMcCad_View::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QGLWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: InitFinished(); break;
        case 1: viewModified(); break;
        case 2: mousePressEvent(*reinterpret_cast< QMouseEvent*(*)>(_a[1])); break;
        case 3: mouseReleaseEvent(*reinterpret_cast< QMouseEvent*(*)>(_a[1])); break;
        case 4: mouseMoveEvent(*reinterpret_cast< QMouseEvent*(*)>(_a[1])); break;
        case 5: LeaveEvent(*reinterpret_cast< QEvent*(*)>(_a[1])); break;
        case 6: EnterEvent(*reinterpret_cast< QEvent*(*)>(_a[1])); break;
        case 7: FocusInEvent(*reinterpret_cast< QFocusEvent*(*)>(_a[1])); break;
        case 8: FocusOutEvent(*reinterpret_cast< QFocusEvent*(*)>(_a[1])); break;
        case 9: wheelEvent(*reinterpret_cast< QWheelEvent*(*)>(_a[1])); break;
        case 10: keyPressEvent(*reinterpret_cast< QKeyEvent*(*)>(_a[1])); break;
        case 11: keyReleaseEvent(*reinterpret_cast< QKeyEvent*(*)>(_a[1])); break;
        case 12: SlotInitTask(); break;
        case 13: PreExecute(); break;
        }
        _id -= 14;
    }
    return _id;
}

// SIGNAL 0
void QMcCad_View::InitFinished()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}

// SIGNAL 1
void QMcCad_View::viewModified()
{
    QMetaObject::activate(this, &staticMetaObject, 1, 0);
}
