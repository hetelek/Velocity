/****************************************************************************
** Meta object code from reading C++ file 'packageviewer.h'
**
** Created: Thu Aug 30 18:19:20 2012
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "packageviewer.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'packageviewer.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_PackageViewer[] = {

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
      15,   14,   14,   14, 0x08,
      35,   14,   14,   14, 0x08,
      65,   59,   14,   14, 0x08,
     107,   95,   14,   14, 0x08,
     161,   14,   14,   14, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_PackageViewer[] = {
    "PackageViewer\0\0on_btnFix_clicked()\0"
    "on_btnViewAll_clicked()\0point\0"
    "showRemoveContextMenu(QPoint)\0item,column\0"
    "on_treeWidget_itemDoubleClicked(QTreeWidgetItem*,int)\0"
    "on_btnProfileEditor_clicked()\0"
};

void PackageViewer::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        PackageViewer *_t = static_cast<PackageViewer *>(_o);
        switch (_id) {
        case 0: _t->on_btnFix_clicked(); break;
        case 1: _t->on_btnViewAll_clicked(); break;
        case 2: _t->showRemoveContextMenu((*reinterpret_cast< QPoint(*)>(_a[1]))); break;
        case 3: _t->on_treeWidget_itemDoubleClicked((*reinterpret_cast< QTreeWidgetItem*(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 4: _t->on_btnProfileEditor_clicked(); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData PackageViewer::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject PackageViewer::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_PackageViewer,
      qt_meta_data_PackageViewer, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &PackageViewer::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *PackageViewer::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *PackageViewer::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_PackageViewer))
        return static_cast<void*>(const_cast< PackageViewer*>(this));
    return QDialog::qt_metacast(_clname);
}

int PackageViewer::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
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
QT_END_MOC_NAMESPACE
