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
    QByteArrayData data[13];
    char stringdata0[243];
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
QT_MOC_LITERAL(3, 40, 38), // "on_patientGenderMalepushButto..."
QT_MOC_LITERAL(4, 79, 40), // "on_patientGenderFemalepushBut..."
QT_MOC_LITERAL(5, 120, 38), // "on_patientGenderNonepushButto..."
QT_MOC_LITERAL(6, 159, 11), // "receiveData"
QT_MOC_LITERAL(7, 171, 8), // "sendData"
QT_MOC_LITERAL(8, 180, 8), // "goOnSend"
QT_MOC_LITERAL(9, 189, 8), // "numBytes"
QT_MOC_LITERAL(10, 198, 8), // "sendFile"
QT_MOC_LITERAL(11, 207, 13), // "serverConnect"
QT_MOC_LITERAL(12, 221, 21) // "on_loadButton_clicked"

    },
    "Doctor\0on_patientNewpushButton_clicked\0"
    "\0on_patientGenderMalepushButton_clicked\0"
    "on_patientGenderFemalepushButton_clicked\0"
    "on_patientGenderNonepushButton_clicked\0"
    "receiveData\0sendData\0goOnSend\0numBytes\0"
    "sendFile\0serverConnect\0on_loadButton_clicked"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Doctor[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
      10,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   64,    2, 0x08 /* Private */,
       3,    0,   65,    2, 0x08 /* Private */,
       4,    0,   66,    2, 0x08 /* Private */,
       5,    0,   67,    2, 0x08 /* Private */,
       6,    0,   68,    2, 0x08 /* Private */,
       7,    0,   69,    2, 0x08 /* Private */,
       8,    1,   70,    2, 0x08 /* Private */,
      10,    0,   73,    2, 0x08 /* Private */,
      11,    0,   74,    2, 0x08 /* Private */,
      12,    0,   75,    2, 0x08 /* Private */,

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
        case 1: _t->on_patientGenderMalepushButton_clicked(); break;
        case 2: _t->on_patientGenderFemalepushButton_clicked(); break;
        case 3: _t->on_patientGenderNonepushButton_clicked(); break;
        case 4: _t->receiveData(); break;
        case 5: _t->sendData(); break;
        case 6: _t->goOnSend((*reinterpret_cast< qint64(*)>(_a[1]))); break;
        case 7: _t->sendFile(); break;
        case 8: _t->serverConnect(); break;
        case 9: _t->on_loadButton_clicked(); break;
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
        if (_id < 10)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 10;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 10)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 10;
    }
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
