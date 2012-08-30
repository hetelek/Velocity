/****************************************************************************
** Meta object code from reading C++ file 'profileeditor.h'
**
** Created: Thu Aug 30 18:19:21 2012
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "profileeditor.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'profileeditor.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_ProfileEditor[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
      14,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      15,   14,   14,   14, 0x08,
      51,   14,   14,   14, 0x08,
     101,   94,   14,   14, 0x08,
     137,   94,   14,   14, 0x08,
     175,   94,   14,   14, 0x08,
     210,   14,   14,   14, 0x08,
     247,   14,   14,   14, 0x08,
     290,   14,   14,   14, 0x08,
     321,   14,   14,   14, 0x08,
     348,   14,   14,   14, 0x08,
     377,   14,   14,   14, 0x08,
     409,   14,   14,   14, 0x08,
     441,  435,   14,   14, 0x08,
     482,  435,   14,   14, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_ProfileEditor[] = {
    "ProfileEditor\0\0on_gamesList_itemSelectionChanged()\0"
    "on_achievementsList_itemSelectionChanged()\0"
    "aReply\0replyFinishedBoxArt(QNetworkReply*)\0"
    "replyFinishedAwBoxArt(QNetworkReply*)\0"
    "replyFinishedAwImg(QNetworkReply*)\0"
    "on_aaGamelist_itemSelectionChanged()\0"
    "on_avatarAwardsList_itemSelectionChanged()\0"
    "on_btnUnlockAllAchvs_clicked()\0"
    "on_btnExtractGPD_clicked()\0"
    "on_btnExtractGPD_2_clicked()\0"
    "on_btnUnlockAllAwards_clicked()\0"
    "on_btnCreateAch_clicked()\0index\0"
    "on_cmbxAchState_currentIndexChanged(int)\0"
    "on_cmbxAwState_currentIndexChanged(int)\0"
};

void ProfileEditor::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        ProfileEditor *_t = static_cast<ProfileEditor *>(_o);
        switch (_id) {
        case 0: _t->on_gamesList_itemSelectionChanged(); break;
        case 1: _t->on_achievementsList_itemSelectionChanged(); break;
        case 2: _t->replyFinishedBoxArt((*reinterpret_cast< QNetworkReply*(*)>(_a[1]))); break;
        case 3: _t->replyFinishedAwBoxArt((*reinterpret_cast< QNetworkReply*(*)>(_a[1]))); break;
        case 4: _t->replyFinishedAwImg((*reinterpret_cast< QNetworkReply*(*)>(_a[1]))); break;
        case 5: _t->on_aaGamelist_itemSelectionChanged(); break;
        case 6: _t->on_avatarAwardsList_itemSelectionChanged(); break;
        case 7: _t->on_btnUnlockAllAchvs_clicked(); break;
        case 8: _t->on_btnExtractGPD_clicked(); break;
        case 9: _t->on_btnExtractGPD_2_clicked(); break;
        case 10: _t->on_btnUnlockAllAwards_clicked(); break;
        case 11: _t->on_btnCreateAch_clicked(); break;
        case 12: _t->on_cmbxAchState_currentIndexChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 13: _t->on_cmbxAwState_currentIndexChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData ProfileEditor::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject ProfileEditor::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_ProfileEditor,
      qt_meta_data_ProfileEditor, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &ProfileEditor::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *ProfileEditor::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *ProfileEditor::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_ProfileEditor))
        return static_cast<void*>(const_cast< ProfileEditor*>(this));
    return QDialog::qt_metacast(_clname);
}

int ProfileEditor::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 14)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 14;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
