#include "strbdialog.h"
#include "ui_strbdialog.h"

StrbDialog::StrbDialog(AvatarAsset *asset, QWidget *parent) : QDialog(parent), ui(new Ui::StrbDialog), asset(asset)
{
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    ui->setupUi(this);
    ui->treeWidget->header()->resizeSection(0, 180);
    QtHelpers::GenAdjustWidgetAppearanceToOS(this);

    // load the metadata
    try
    {
        AssetMetadata metadata = asset->GetAssetMetadata();
        ui->lblGender->setText("Asset Gender: " + QString::fromStdString(AssetHelpers::AssetGenderToString(metadata.gender)));
        ui->lblBinType->setText("Binary Type: " + QString::fromStdString(AssetHelpers::BinaryAssetTypeToString(metadata.type)));
        ui->lblDetails->setText("Type Details: 0x" + QString::number(metadata.assetTypeDetails, 16).toUpper());
        ui->lblSubCat->setText("Sub-Category: " + QString::fromStdString(AssetHelpers::AssetSubcategoryToString(metadata.category)));
        ui->lblSkeletonV->setText("Skeleton Version: " + QString::fromStdString(AssetHelpers::SkeletonVersionToString(metadata.skeletonVersion)));
    }
    catch(string error)
    {
        ui->groupBox->setTitle("Metadata - Not Found");
        ui->groupBox->setEnabled(false);
    }

    // load the animation
    try
    {
        struct Animation animation = asset->GetAnimation();
        ui->lblDuration->setText("Duration: " + QString::number(animation.duration) + " seconds");
        ui->lblFPS->setText("Frames Per Second: " + QString::number(animation.framesPerSecond));
        ui->lblFrameCnt->setText("Frame Count: " + QString::number(animation.frameCount));
    }
    catch (string error)
    {
        ui->groupBox_2->setTitle("Animation - Not Found");
        ui->groupBox_2->setEnabled(false);
    }

    // load all the blocks
    for (DWORD i = 0; i < asset->blocks.size(); i++)
    {
        QTreeWidgetItem *item = new QTreeWidgetItem(ui->treeWidget);
        item->setText(0, QString::fromStdString(AssetHelpers::BlockIDToString(asset->blocks.at(i).id)));
        item->setText(1, "0x" + QString::number(asset->blocks.at(i).dataAddress, 16).toUpper());
        item->setText(2, "0x" + QString::number(asset->blocks.at(i).dataLength, 16).toUpper());

        ui->treeWidget->insertTopLevelItem(ui->treeWidget->topLevelItemCount(), item);
    }
}

StrbDialog::~StrbDialog()
{
    delete asset;
    delete ui;
}
