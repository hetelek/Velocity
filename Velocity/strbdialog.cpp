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
        ui->lblGender->setText("Asset Gender: " + AssetHelpers::AssetGenderToString(metadata.gender));
        ui->lblBinType->setText("Binary Type: " + AssetHelpers::BinaryAssetTypeToString(metadata.type));
        ui->lblDetails->setText("Type Details: 0x" + QString::number(metadata.assetTypeDetails, 16).toUpper());
        ui->lblSubCat->setText("Sub-Category: " + AssetHelpers::AssetSubcategoryToString(metadata.category));
        ui->lblSkeletonV->setText("Skeleton Version: " + AssetHelpers::SkeletonVersionToString(metadata.skeletonVersion));
    }
    catch(const QString &error)
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
    catch (const QString &error)
    {
        ui->groupBox_2->setTitle("Animation - Not Found");
        ui->groupBox_2->setEnabled(false);
    }

    // load all the blocks
    const QVector<STRBBlock> blocks = asset->GetBlocks();
    for (int i = 0; i < blocks.size(); i++)
    {
        QTreeWidgetItem *item = new QTreeWidgetItem;
        item->setText(0, AssetHelpers::BlockIDToString(blocks.at(i).id));
        item->setText(1, "0x" + QString::number(blocks.at(i).dataAddress, 16).toUpper());
        item->setText(2, "0x" + QString::number(blocks.at(i).dataLength, 16).toUpper());

        ui->treeWidget->insertTopLevelItem(ui->treeWidget->topLevelItemCount(), item);
    }
}

StrbDialog::~StrbDialog()
{
    delete asset;
    delete ui;
}
