/****************************************************************************
** Meta object code from reading C++ file 'doctor.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../Doctor/doctor.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'doctor.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_Doctor_t {
    QByteArrayData data[12];
    char stringdata0[224];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Doctor_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Doctor_t qt_meta_stringdata_Doctor = {
    {
QT_MOC_LITERAL(0, 0, 6), // "Doctor"
QT_MOC_LITERAL(1, 7, 31), // "on_patientNewpushButton_clicked"
QT_MOC_LITERAL(2, 39, 0), // ""
QT_MOC_LITERAL(3, 40, 39), // "on_patientGenderMaleradioButt..."
QT_MOC_LITERAL(4, 80, 41), // "on_patientGenderFemaleradioBu..."
QT_MOC_LITERAL(5, 122, 39), // "on_patientGenderNoneradioButt..."
QT_MOC_LITERAL(6, 162, 11), // "receiveData"
QT_MOC_LITERAL(7, 174, 8), // "sendData"
QT_MOC_LITERAL(8, 183, 8), // "goOnSend"
QT_MOC_LITERAL(9, 192, 8), // "numBytes"
QT_MOC_LITERAL(10, 201, 8), // "sendFile"
QT_MOC_LITERAL(11, 210, 13) // "serverConnect"

    },
    "Doctor\0on_patientNewpushButton_clicked\0"
    "\0on_patientGenderMaleradioButton_clicked\0"
    "on_patientGenderFemaleradioButton_clicked\0"
    "on_patientGenderNoneradioButton_clicked\0"
    "receiveData\0sendData\0goOnSend\0numBytes\0"
    "sendFile\0serverConnect"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Doctor[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       9,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   59,    2, 0x08 /* Private */,
       3,    0,   60,    2, 0x08 /* Private */,
       4,    0,   61,    2, 0x08 /* Private */,
       5,    0,   62,    2, 0x08 /* Private */,
       6,    0,   63,    2, 0x08 /* Private */,
       7,    0,   64,    2, 0x08 /* Private */,
       8,    1,   65,    2, 0x08 /* Private */,
      10,    0,   68,    2, 0x08 /* Private */,
      11,    0,   69,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::LongLong,    9,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void Doctor::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<Doctor *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->on_patientNewpushButton_clicked(); break;
        case 1: _t->on_patientGenderMaleradioButton_clicked(); break;
        case 2: _t->on_patientGenderFemaleradioButton_clicked(); break;
        case 3: _t->on_patientGenderNoneradioButton_clicked(); break;
        case 4: _t->receiveData(); break;
        case 5: _t->sendData(); break;
        case 6: _t->goOnSend((*reinterpret_cast< qint64(*)>(_a[1]))); break;
        case 7: _t->sendFile(); break;
        case 8: _t->serverConnect(); break;
        default: ;
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject Doctor::staticMetaObject = { {
    QMetaObject::SuperData::link<QWidget::staticMetaObject>(),
    qt_meta_stringdata_Doctor.data,
    qt_meta_data_Doctor,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *Doctor::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Doctor::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_Doctor.stringdata0))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int Doctor::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
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
QT_WARNING_POP
QT_END_MOC_NAMESPACE
