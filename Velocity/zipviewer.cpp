#include "zipviewer.h"
#include "ui_zipviewer.h"
#include <QtCore/private/qzipreader_p.h>
#include <QFileDialog>
#include <QMessageBox>
#include <QFile>

ZipViewer::ZipViewer(const QString &zipPath, const QString &fileName, QWidget *parent)
    : QDialog(parent), ui(new Ui::ZipViewer), zipPath(zipPath)
{
    ui->setupUi(this);
    setWindowTitle("ZIP Viewer - " + fileName);
    
    // Set up table
    ui->tableWidget->setColumnCount(3);
    ui->tableWidget->setHorizontalHeaderLabels({"File Path", "Size", "Compressed"});
    ui->tableWidget->horizontalHeader()->setStretchLastSection(false);
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableWidget->setSortingEnabled(true);
    
    // Connect buttons
    connect(ui->btnExtractSelected, &QPushButton::clicked, this, &ZipViewer::onExtractSelected);
    connect(ui->btnExtractAll, &QPushButton::clicked, this, &ZipViewer::onExtractAll);
    connect(ui->btnClose, &QPushButton::clicked, this, &QDialog::close);
    
    loadZipContents();
}

ZipViewer::~ZipViewer()
{
    delete ui;
}

void ZipViewer::loadZipContents()
{
    QZipReader zip(zipPath);
    
    if (!zip.exists())
    {
        QMessageBox::warning(this, "Error", "Unable to open ZIP file.");
        return;
    }
    
    // Disable sorting while populating
    ui->tableWidget->setSortingEnabled(false);
    
    QList<QZipReader::FileInfo> files = zip.fileInfoList();
    ui->tableWidget->setRowCount(files.size());
    
    int row = 0;
    for (const auto &file : files)
    {
        // Skip directories
        if (file.isDir)
        {
            ui->tableWidget->setRowCount(ui->tableWidget->rowCount() - 1);
            continue;
        }
        
        // File path
        QTableWidgetItem *pathItem = new QTableWidgetItem(file.filePath);
        ui->tableWidget->setItem(row, 0, pathItem);
        
        // Uncompressed size
        QString sizeStr = QString::number(file.size) + " bytes";
        if (file.size > 1024 * 1024)
            sizeStr = QString::number(file.size / 1024.0 / 1024.0, 'f', 2) + " MB";
        else if (file.size > 1024)
            sizeStr = QString::number(file.size / 1024.0, 'f', 2) + " KB";
        
        NumericSizeItem *sizeItem = new NumericSizeItem(sizeStr, file.size);
        sizeItem->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
        sizeItem->setData(Qt::UserRole, static_cast<qulonglong>(file.size)); // Store numeric value for sorting
        ui->tableWidget->setItem(row, 1, sizeItem);
        
        // Compressed indicator
        QTableWidgetItem *compItem = new QTableWidgetItem(file.isFile ? "Yes" : "-");
        compItem->setTextAlignment(Qt::AlignCenter);
        ui->tableWidget->setItem(row, 2, compItem);
        
        row++;
    }
    
    ui->labelInfo->setText(QString("Total files: %1").arg(row));
    
    // Re-enable sorting after populating
    ui->tableWidget->setSortingEnabled(true);
    
    zip.close();
}

void ZipViewer::onExtractSelected()
{
    QList<QTableWidgetItem*> selected = ui->tableWidget->selectedItems();
    if (selected.isEmpty())
    {
        QMessageBox::information(this, "No Selection", "Please select files to extract.");
        return;
    }
    
    QString outputDir = QFileDialog::getExistingDirectory(this, "Select Output Directory");
    if (outputDir.isEmpty())
        return;
    
    QZipReader zip(zipPath);
    int extractedCount = 0;
    
    // Get unique rows
    QSet<int> rows;
    for (auto *item : selected)
        rows.insert(item->row());
    
    for (int row : rows)
    {
        QString filePath = ui->tableWidget->item(row, 0)->text();
        QByteArray data = zip.fileData(filePath);
        
        QString outputPath = outputDir + "/" + QFileInfo(filePath).fileName();
        QFile outFile(outputPath);
        if (outFile.open(QIODevice::WriteOnly | QIODevice::Truncate))
        {
            outFile.write(data);
            outFile.close();
            extractedCount++;
        }
    }
    
    zip.close();
    QMessageBox::information(this, "Extraction Complete", 
                            QString("Extracted %1 file(s) to:\n%2").arg(extractedCount).arg(outputDir));
}

void ZipViewer::onExtractAll()
{
    QString outputDir = QFileDialog::getExistingDirectory(this, "Select Output Directory");
    if (outputDir.isEmpty())
        return;
    
    QZipReader zip(zipPath);
    if (!zip.extractAll(outputDir))
    {
        QMessageBox::warning(this, "Error", "Failed to extract all files.");
        zip.close();
        return;
    }
    
    zip.close();
    QMessageBox::information(this, "Extraction Complete", 
                            QString("All files extracted to:\n%1").arg(outputDir));
}
