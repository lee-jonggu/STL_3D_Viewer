/****************************************************************************
** Meta object code from reading C++ file 'nurse.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../Nurse/nurse.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'nurse.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_Nurse_t {
    QByteArrayData data[8];
    char stringdata0[179];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Nurse_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Nurse_t qt_meta_stringdata_Nurse = {
    {
QT_MOC_LITERAL(0, 0, 5), // "Nurse"
QT_MOC_LITERAL(1, 6, 31), // "on_patientNewpushButton_clicked"
QT_MOC_LITERAL(2, 38, 0), // ""
QT_MOC_LITERAL(3, 39, 38), // "on_patientGenderMalepushButto..."
QT_MOC_LITERAL(4, 78, 40), // "on_patientGenderFemalepushBut..."
QT_MOC_LITERAL(5, 119, 38), // "on_patientGenderNonepushButto..."
QT_MOC_LITERAL(6, 158, 11), // "receiveData"
QT_MOC_LITERAL(7, 170, 8) // "sendData"

    },
    "Nurse\0on_patientNewpushButton_clicked\0"
    "\0on_patientGenderMalepushButton_clicked\0"
    "on_patientGenderFemalepushButton_clicked\0"
    "on_patientGenderNonepushButton_clicked\0"
    "receiveData\0sendData"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Nurse[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       6,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   44,    2, 0x08 /* Private */,
       3,    0,   45,    2, 0x08 /* Private */,
       4,    0,   46,    2, 0x08 /* Private */,
       5,    0,   47,    2, 0x08 /* Private */,
       6,    0,   48,    2, 0x08 /* Private */,
       7,    0,   49,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void Nurse::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<Nurse *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->on_patientNewpushButton_clicked(); break;
        case 1: _t->on_patientGenderMalepushButton_clicked(); break;
        case 2: _t->on_patientGenderFemalepushButton_clicked(); break;
        case 3: _t->on_patientGenderNonepushButton_clicked(); break;
        case 4: _t->receiveData(); break;
        case 5: _t->sendData(); break;
        default: ;
        }
    }
    Q_UNUSED(_a);
}

QT_INIT_METAOBJECT const QMetaObject Nurse::staticMetaObject = { {
    QMetaObject::SuperData::link<QWidget::staticMetaObject>(),
    qt_meta_stringdata_Nurse.data,
    qt_meta_data_Nurse,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *Nurse::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Nurse::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_Nurse.stringdata0))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int Nurse::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 6)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 6;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 6)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 6;
    }
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
