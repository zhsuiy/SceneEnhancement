/****************************************************************************
** Meta object code from reading C++ file 'DisplaySceneGLWidget.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.6.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../DisplaySceneGLWidget.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'DisplaySceneGLWidget.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.6.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_DisplaySceneGLWidget_t {
    QByteArrayData data[11];
    char stringdata0[191];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_DisplaySceneGLWidget_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_DisplaySceneGLWidget_t qt_meta_stringdata_DisplaySceneGLWidget = {
    {
QT_MOC_LITERAL(0, 0, 20), // "DisplaySceneGLWidget"
QT_MOC_LITERAL(1, 21, 12), // "UpdateConfig"
QT_MOC_LITERAL(2, 34, 0), // ""
QT_MOC_LITERAL(3, 35, 15), // "UpdateMaterials"
QT_MOC_LITERAL(4, 51, 17), // "UpdateDecorations"
QT_MOC_LITERAL(5, 69, 24), // "UpdateMaterialsByLearner"
QT_MOC_LITERAL(6, 94, 26), // "UpdateDecorationsByLearner"
QT_MOC_LITERAL(7, 121, 13), // "ToggleTexture"
QT_MOC_LITERAL(8, 135, 24), // "ToggleDisplayDecorations"
QT_MOC_LITERAL(9, 160, 20), // "RearrangeDecorations"
QT_MOC_LITERAL(10, 181, 9) // "SaveImage"

    },
    "DisplaySceneGLWidget\0UpdateConfig\0\0"
    "UpdateMaterials\0UpdateDecorations\0"
    "UpdateMaterialsByLearner\0"
    "UpdateDecorationsByLearner\0ToggleTexture\0"
    "ToggleDisplayDecorations\0RearrangeDecorations\0"
    "SaveImage"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_DisplaySceneGLWidget[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       9,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   59,    2, 0x0a /* Public */,
       3,    0,   60,    2, 0x0a /* Public */,
       4,    0,   61,    2, 0x0a /* Public */,
       5,    0,   62,    2, 0x0a /* Public */,
       6,    0,   63,    2, 0x0a /* Public */,
       7,    0,   64,    2, 0x0a /* Public */,
       8,    0,   65,    2, 0x0a /* Public */,
       9,    0,   66,    2, 0x0a /* Public */,
      10,    0,   67,    2, 0x0a /* Public */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void DisplaySceneGLWidget::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        DisplaySceneGLWidget *_t = static_cast<DisplaySceneGLWidget *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->UpdateConfig(); break;
        case 1: _t->UpdateMaterials(); break;
        case 2: _t->UpdateDecorations(); break;
        case 3: _t->UpdateMaterialsByLearner(); break;
        case 4: _t->UpdateDecorationsByLearner(); break;
        case 5: _t->ToggleTexture(); break;
        case 6: _t->ToggleDisplayDecorations(); break;
        case 7: _t->RearrangeDecorations(); break;
        case 8: _t->SaveImage(); break;
        default: ;
        }
    }
    Q_UNUSED(_a);
}

const QMetaObject DisplaySceneGLWidget::staticMetaObject = {
    { &QGLWidget::staticMetaObject, qt_meta_stringdata_DisplaySceneGLWidget.data,
      qt_meta_data_DisplaySceneGLWidget,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *DisplaySceneGLWidget::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *DisplaySceneGLWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_DisplaySceneGLWidget.stringdata0))
        return static_cast<void*>(const_cast< DisplaySceneGLWidget*>(this));
    if (!strcmp(_clname, "QOpenGLFunctions"))
        return static_cast< QOpenGLFunctions*>(const_cast< DisplaySceneGLWidget*>(this));
    return QGLWidget::qt_metacast(_clname);
}

int DisplaySceneGLWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QGLWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 9)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 9;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 9)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 9;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
