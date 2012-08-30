/****************************************************************************
** Meta object code from reading C++ file 'volumedescriptordialog.h'
**
** Created: Thu Aug 30 18:19:24 2012
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "volumedescriptordialog.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'volumedescriptordialog.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_VolumeDescriptorDialog[] = {

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
      24,   23,   23,   23, 0x08,
      50,   23,   23,   23, 0x08,
      80,   74,   23,   23, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_VolumeDescriptorDialog[] = {
    "VolumeDescriptorDialog\0\0"
    "on_pushButton_2_clicked()\0"
    "on_pushButton_clicked()\0index\0"
    "packageTypeChanged(int)\0"
};

void VolumeDescriptorDialog::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        VolumeDescriptorDialog *_t = static_cast<VolumeDescriptorDialog *>(_o);
        switch (_id) {
        case 0: _t->on_pushButton_2_clicked(); break;
        case 1: _t->on_pushButton_clicked(); break;
        case 2: _t->packageTypeChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData VolumeDescriptorDialog::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject VolumeDescriptorDialog::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_VolumeDescriptorDialog,
      qt_meta_data_VolumeDescriptorDialog, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &VolumeDescriptorDialog::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *VolumeDescriptorDialog::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *VolumeDescriptorDialog::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_VolumeDescriptorDialog))
        return static_cast<void*>(const_cast< VolumeDescriptorDialog*>(this));
    return QDialog::qt_metacast(_clname);
}

int VolumeDescriptorDialog::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
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
QT_END_MOC_NAMESPACE
