#ifndef STRBDIALOG_H
#define STRBDIALOG_H

// qt
#include <QDialog>
#include "qthelpers.h"

// xbox360
#include <XboxInternals/AvatarAsset/AvatarAsset.h>
#include <XboxInternals/AvatarAsset/AssetHelpers.h>
#include <XboxInternals/AvatarAsset/AvatarAssetDefinitions.h>

namespace Ui
{
class StrbDialog;
}

class StrbDialog : public QDialog
{
    Q_OBJECT

public:
    explicit StrbDialog(AvatarAsset *asset, QWidget *parent = 0);
    ~StrbDialog();

private:
    Ui::StrbDialog *ui;
    AvatarAsset *asset;
};

#endif // STRBDIALOG_H


