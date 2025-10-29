#include "isodialog.h"
#include "imagedialog.h"
#include "textdialog.h"
#include "xmldialog.h"
#include "TextEncoding/EncodingDetector.h"
#include <QPushButton>
#include <QTreeWidget>
#include <QLineEdit>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFileDialog>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QMimeData>
#include <QMessageBox>
#include <QHeaderView>
#include <QFile>
#include <QFileInfo>
#include <QDir>
#include <QTextStream>
#include <QPixmap>
#include <QBuffer>
#include <QStringDecoder>
#include <QStringConverter>
#include <fstream>
#include "XboxInternals/Iso/IsoImage.h"
#include "XboxInternals/Iso/XexExecutable.h"

IsoDialog::IsoDialog(QWidget* parent)
    : QDialog(parent)
{
    setWindowTitle(tr("Xbox 360 ISO Browser"));
    setAcceptDrops(true);
    resize(900, 600);

    // Top button row
    extractSelectedButton_ = new QPushButton(tr("Extract Selected"), this);
    extractAllButton_ = new QPushButton(tr("Extract All Files"), this);
    expandAllButton_ = new QPushButton(tr("Expand All"), this);
    collapseAllButton_ = new QPushButton(tr("Collapse All"), this);
    
    extractSelectedButton_->setEnabled(false);
    extractAllButton_->setEnabled(false);
    expandAllButton_->setEnabled(false);
    collapseAllButton_->setEnabled(false);

    auto* buttonLayout = new QHBoxLayout();
    buttonLayout->addWidget(extractSelectedButton_);
    buttonLayout->addWidget(extractAllButton_);
    buttonLayout->addStretch();
    buttonLayout->addWidget(expandAllButton_);
    buttonLayout->addWidget(collapseAllButton_);

    // Search box
    auto* searchLayout = new QHBoxLayout();
    auto* searchLabel = new QLabel(tr("Search:"), this);
    searchBox_ = new QLineEdit(this);
    searchBox_->setPlaceholderText(tr("Filter files by name..."));
    searchBox_->setClearButtonEnabled(true);
    searchLayout->addWidget(searchLabel);
    searchLayout->addWidget(searchBox_);

    // Tree widget for ISO contents (collapsible hierarchy)
    tree_ = new QTreeWidget(this);
    tree_->setHeaderLabels({tr("Name"), tr("Type"), tr("Size")});
    tree_->setSelectionMode(QAbstractItemView::ExtendedSelection);
    tree_->header()->setStretchLastSection(false);
    tree_->header()->setSectionResizeMode(0, QHeaderView::Stretch);
    tree_->setIndentation(20); // Proper indentation for folders

    auto* mainLayout = new QVBoxLayout(this);
    mainLayout->addLayout(buttonLayout);
    mainLayout->addLayout(searchLayout);
    mainLayout->addWidget(tree_);

    connect(extractSelectedButton_, &QPushButton::clicked, this, &IsoDialog::onExtractSelected);
    connect(extractAllButton_, &QPushButton::clicked, this, &IsoDialog::onExtractAll);
    connect(expandAllButton_, &QPushButton::clicked, this, &IsoDialog::onExpandAll);
    connect(collapseAllButton_, &QPushButton::clicked, this, &IsoDialog::onCollapseAll);
    connect(tree_, &QTreeWidget::itemDoubleClicked, this, &IsoDialog::onItemDoubleClicked);
    connect(searchBox_, &QLineEdit::textChanged, this, &IsoDialog::onSearchTextChanged);
}

IsoDialog::~IsoDialog() = default;

void IsoDialog::dragEnterEvent(QDragEnterEvent* event) {
    if (event->mimeData()->hasUrls()) event->acceptProposedAction();
}

void IsoDialog::dropEvent(QDropEvent* event) {
    const auto urls = event->mimeData()->urls();
    for (const QUrl& u : urls) {
        const QString path = u.toLocalFile();
        if (!path.isEmpty() && path.endsWith(".iso", Qt::CaseInsensitive)) {
            loadIsoContents(path);
            break;  // Only load first ISO
        }
    }
}

void IsoDialog::loadIsoContents(const QString& path) {
    tree_->clear();
    folderItems_.clear();
    currentIsoPath_ = path;

    if (!iso_) iso_.reset(new XboxInternals::Iso::IsoImage());
    
    if (!iso_->open(path.toStdString())) {
        QMessageBox::warning(this, tr("Error"), 
            tr("Failed to open ISO file:\n%1\n\nThis could be due to:\n"
               "- File is not a valid Xbox 360 ISO\n"
               "- Magic string not found at expected offsets\n"
               "- File access permissions\n"
               "- Corrupted file").arg(path));
        return;
    }

    populateTree();

    extractSelectedButton_->setEnabled(true);
    extractAllButton_->setEnabled(true);
    expandAllButton_->setEnabled(true);
    collapseAllButton_->setEnabled(true);

    const auto& info = iso_->info();
    setWindowTitle(tr("Xbox 360 ISO Browser - %1 (%2 bytes)").arg(path).arg(info.imageSize));
}

QTreeWidgetItem* IsoDialog::findOrCreateFolder(const QString& path) {
    if (path.isEmpty()) return nullptr;
    
    // Check cache first
    if (folderItems_.contains(path)) {
        return folderItems_[path];
    }
    
    // Split path and create hierarchy
    QStringList parts = path.split('/', Qt::SkipEmptyParts);
    QTreeWidgetItem* parent = nullptr;
    QString currentPath;
    
    for (const QString& part : parts) {
        currentPath += (currentPath.isEmpty() ? "" : "/") + part;
        
        if (folderItems_.contains(currentPath)) {
            parent = folderItems_[currentPath];
        } else {
            QTreeWidgetItem* item;
            if (parent) {
                item = new QTreeWidgetItem(parent);
            } else {
                item = new QTreeWidgetItem(tree_);
            }
            
            item->setText(0, part);
            item->setText(1, tr("Folder"));
            item->setText(2, "");
            item->setData(0, Qt::UserRole, currentPath);
            item->setData(0, Qt::UserRole + 1, true); // Mark as folder
            
            folderItems_[currentPath] = item;
            parent = item;
        }
    }
    
    return parent;
}

void IsoDialog::populateTree() {
    auto entries = iso_->listEntries();
    
    for (const auto& entry : entries) {
        // Skip directory entries - they'll be created as containers for files
        if (entry.type == XboxInternals::Iso::IsoEntryType::Directory) {
            continue;
        }
        
        QString fullPath = QString::fromStdString(entry.path);
        QStringList pathParts = fullPath.split('/', Qt::SkipEmptyParts);
        
        if (pathParts.isEmpty()) continue;
        
        QString fileName = pathParts.last();
        QString folderPath;
        
        if (pathParts.size() > 1) {
            pathParts.removeLast();
            folderPath = pathParts.join('/');
        }
        
        // Create folder hierarchy if needed
        QTreeWidgetItem* parent = findOrCreateFolder(folderPath);
        
        // Create file item
        QTreeWidgetItem* item;
        if (parent) {
            item = new QTreeWidgetItem(parent);
        } else {
            item = new QTreeWidgetItem(tree_);
        }
        
        item->setText(0, fileName);
        item->setText(1, tr("File"));
        item->setText(2, QString::number(entry.size));
        item->setData(0, Qt::UserRole, fullPath);
        item->setData(0, Qt::UserRole + 1, false); // Mark as file
    }
    
    tree_->sortItems(0, Qt::AscendingOrder);
}

void IsoDialog::onExtractSelected() {
    auto selected = tree_->selectedItems();
    if (selected.isEmpty()) {
        QMessageBox::information(this, tr("Info"), tr("No files or folders selected"));
        return;
    }

    const QString outDir = QFileDialog::getExistingDirectory(this, tr("Select Output Directory"));
    if (outDir.isEmpty()) return;

    int extracted = 0;
    auto entries = iso_->listEntries();
    
    for (auto* item : selected) {
        QString itemPath = item->data(0, Qt::UserRole).toString();
        bool isFolder = item->data(0, Qt::UserRole + 1).toBool();
        
        if (isFolder) {
            // Extract all files within this folder, preserving structure from parent
            QString folderPrefix = itemPath + "/";
            
            // Get the parent path to strip (everything before the selected folder)
            QString folderName = itemPath;
            int lastSlash = itemPath.lastIndexOf('/');
            QString parentPath = (lastSlash >= 0) ? itemPath.left(lastSlash + 1) : "";
            
            for (const auto& entry : entries) {
                QString entryPath = QString::fromStdString(entry.path);
                if (entry.type == XboxInternals::Iso::IsoEntryType::File &&
                    entryPath.startsWith(folderPrefix)) {
                    
                    // Calculate relative path by removing parent folders
                    QString relativePath = entryPath.mid(parentPath.length());
                    
                    // Create a modified entry with relative path
                    XboxInternals::Iso::IsoEntry modEntry = entry;
                    modEntry.path = relativePath.toStdString();
                    
                    if (iso_->extractFile(modEntry, outDir.toStdString())) {
                        extracted++;
                    }
                }
            }
        } else {
            // Extract single file with just its filename
            for (const auto& entry : entries) {
                if (QString::fromStdString(entry.path) == itemPath && 
                    entry.type == XboxInternals::Iso::IsoEntryType::File) {
                    
                    // Create a modified entry with just the filename
                    XboxInternals::Iso::IsoEntry modEntry = entry;
                    modEntry.path = entry.name;  // Use just the filename
                    
                    if (iso_->extractFile(modEntry, outDir.toStdString())) {
                        extracted++;
                    }
                    break;
                }
            }
        }
    }

    QMessageBox::information(this, tr("Success"), tr("Extracted %1 file(s)").arg(extracted));
}

void IsoDialog::onExtractAll() {
    const QString outDir = QFileDialog::getExistingDirectory(this, tr("Select Output Directory for Batch Export"));
    if (outDir.isEmpty()) return;

    // Get game name from default.xex if available
    XboxInternals::Iso::XexExecutable xex;
    xex.parseFromFilename(currentIsoPath_.toStdString());
    
    QString folderName = QString::fromStdString(xex.info().titleId);
    if (folderName.isEmpty()) {
        folderName = QFileInfo(currentIsoPath_).baseName();
    }

    QString fullOutDir = outDir + "/" + folderName;
    
    if (iso_->extractAll(fullOutDir.toStdString())) {
        QMessageBox::information(this, tr("Success"), tr("Batch export completed to:\n%1").arg(fullOutDir));
    } else {
        QMessageBox::warning(this, tr("Error"), tr("Batch export failed"));
    }
}

void IsoDialog::onItemDoubleClicked(QTreeWidgetItem* item, int column) {
    Q_UNUSED(column);
    
    // Skip if it's a folder
    bool isFolder = item->data(0, Qt::UserRole + 1).toBool();
    if (isFolder) {
        return;
    }
    
    QString itemPath = item->data(0, Qt::UserRole).toString();
    
    auto entries = iso_->listEntries();
    for (const auto& entry : entries) {
        if (QString::fromStdString(entry.path) == itemPath && 
            entry.type == XboxInternals::Iso::IsoEntryType::File) {
            openFileInViewer(entry);
            break;
        }
    }
}

void IsoDialog::openFileInViewer(const XboxInternals::Iso::IsoEntry& entry) {
    QString fileName = QString::fromStdString(entry.name).toLower();
    
    // Extract to temp location
    QString tempDir = QDir::temp().filePath("velocity_iso_preview");
    QDir().mkpath(tempDir);
    
    if (!iso_->extractFile(entry, tempDir.toStdString())) {
        QMessageBox::warning(this, tr("Error"), 
            tr("Failed to extract file for viewing:\n%1\n\nPath: %2")
            .arg(QString::fromStdString(entry.name))
            .arg(QString::fromStdString(entry.path)));
        return;
    }

    // Use full path since extractFile now preserves directory structure
    QString tempFile = tempDir + "/" + QString::fromStdString(entry.path);
    
    // Verify file exists
    if (!QFile::exists(tempFile)) {
        QMessageBox::warning(this, tr("Error"), 
            tr("Extracted file not found:\n%1").arg(tempFile));
        return;
    }
    
    // Handle image files with ImageDialog popup
    if (fileName.endsWith(".jpg") || fileName.endsWith(".jpeg") || 
        fileName.endsWith(".png") || fileName.endsWith(".bmp") || 
        fileName.endsWith(".gif") || fileName.endsWith(".tga") ||
        fileName.endsWith(".dds")) {
        
        // Try to load as standard image first
        QImage image;
        
        // For JPG files, explicitly try JPEG format
        if (fileName.endsWith(".jpg") || fileName.endsWith(".jpeg")) {
            image.load(tempFile, "JPEG");
        } else if (fileName.endsWith(".png")) {
            image.load(tempFile, "PNG");
        } else if (fileName.endsWith(".bmp")) {
            image.load(tempFile, "BMP");
        } else {
            // Let Qt auto-detect
            image.load(tempFile);
        }
        
        if (!image.isNull()) {
            ImageDialog *dlg = new ImageDialog(image, QString::fromStdString(entry.name), this);
            dlg->setAttribute(Qt::WA_DeleteOnClose);
            dlg->show();
        } else {
            // Read first few bytes to check if it's really a JPEG or Xbox texture format
            QFile checkFile(tempFile);
            if (checkFile.open(QIODevice::ReadOnly)) {
                QByteArray header = checkFile.read(4);
                checkFile.close();
                
                // Check for known image formats
                bool isStandardFormat = false;
                if (header.size() >= 3 && (unsigned char)header[0] == 0xFF && 
                    (unsigned char)header[1] == 0xD8 && (unsigned char)header[2] == 0xFF) {
                    isStandardFormat = true; // JPEG
                } else if (header.size() >= 4 && header[0] == (char)0x89 && 
                           header[1] == 'P' && header[2] == 'N' && header[3] == 'G') {
                    isStandardFormat = true; // PNG
                } else if (header.size() >= 4 && header[0] == 'D' && header[1] == 'D' && 
                           header[2] == 'S' && header[3] == ' ') {
                    isStandardFormat = true; // DDS
                }
                
                if (!isStandardFormat) {
                    // Xbox 360 proprietary texture format - offer to extract
                    QMessageBox msgBox(this);
                    msgBox.setWindowTitle(tr("Xbox 360 Texture Format"));
                    msgBox.setText(tr("This file appears to be an Xbox 360 proprietary texture format, not a standard image.\n\n"
                                     "File: %1\nSize: %2 bytes\n\n"
                                     "Would you like to extract it to view with external tools?")
                                   .arg(QString::fromStdString(entry.name))
                                   .arg(entry.size));
                    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
                    msgBox.setDefaultButton(QMessageBox::Yes);
                    
                    if (msgBox.exec() == QMessageBox::Yes) {
                        QString saveFile = QFileDialog::getSaveFileName(this, 
                            tr("Save Xbox Texture"), 
                            QString::fromStdString(entry.name),
                            tr("All Files (*)"));
                        
                        if (!saveFile.isEmpty()) {
                            if (QFile::copy(tempFile, saveFile)) {
                                QMessageBox::information(this, tr("Success"), 
                                    tr("File extracted to:\n%1").arg(saveFile));
                            } else {
                                QMessageBox::warning(this, tr("Error"), 
                                    tr("Failed to save file"));
                            }
                        }
                    }
                } else {
                    // Standard format but Qt couldn't load it
                    QMessageBox::warning(this, tr("Error"), 
                        tr("Failed to load image:\n%1\n\nFile appears to be a standard format but Qt failed to decode it.\n"
                           "File size: %2 bytes")
                        .arg(QString::fromStdString(entry.name))
                        .arg(QFileInfo(tempFile).size()));
                }
            } else {
                QMessageBox::warning(this, tr("Error"), 
                    tr("Failed to load image:\n%1\n\nCould not read file.")
                    .arg(QString::fromStdString(entry.name)));
            }
        }
        QFile::remove(tempFile);
        return;
    }
    
    // Handle XML files with XmlDialog
    if (fileName.endsWith(".xml") || fileName.endsWith(".xsd") || 
        fileName.endsWith(".xsl") || fileName.endsWith(".xslt")) {
        
        QFile file(tempFile);
        if (file.open(QIODevice::ReadOnly)) {
            QByteArray data = file.readAll();
            file.close();
            
            QString xmlContent;
            // Check for BOM and decode accordingly (same logic as packageviewer)
            if (data.startsWith("\xFF\xFE")) {
                auto toUtf16LE = QStringDecoder(QStringDecoder::Utf16LE);
                xmlContent = toUtf16LE(data.mid(2));
            } else if (data.startsWith("\xFE\xFF")) {
                auto toUtf16BE = QStringDecoder(QStringDecoder::Utf16BE);
                xmlContent = toUtf16BE(data.mid(2));
            } else if (data.startsWith("\xEF\xBB\xBF")) {
                auto toUtf8 = QStringDecoder(QStringDecoder::Utf8);
                xmlContent = toUtf8(data.mid(3));
            } else {
                auto toUtf8 = QStringDecoder(QStringDecoder::Utf8);
                xmlContent = toUtf8(data);
                
                if (xmlContent.contains(QChar::ReplacementCharacter)) {
                    auto toLatin1 = QStringDecoder(QStringDecoder::Latin1);
                    xmlContent = toLatin1(data);
                }
            }
            
            XmlDialog *dlg = new XmlDialog(xmlContent, QString::fromStdString(entry.name), this);
            dlg->setAttribute(Qt::WA_DeleteOnClose);
            dlg->show();
        } else {
            QMessageBox::warning(this, tr("Error"), tr("Failed to read XML file"));
        }
        QFile::remove(tempFile);
        return;
    }
    
    // Handle text files with TextDialog
    if (fileName.endsWith(".txt") || fileName.endsWith(".cfg") || 
        fileName.endsWith(".ini") || fileName.endsWith(".json") || 
        fileName.endsWith(".log") || fileName.endsWith(".lua") ||
        fileName.endsWith(".h") || fileName.endsWith(".cpp") ||
        fileName.endsWith(".c") || fileName.endsWith(".hlsl") ||
        fileName.endsWith(".fx") || fileName.endsWith(".shader")) {
        
        QFile file(tempFile);
        if (file.open(QIODevice::ReadOnly)) {
            QByteArray data = file.readAll();
            file.close();
            
            // Pass raw data to TextDialog - it will handle encoding detection and UI
            TextDialog *dlg = new TextDialog(data, QString::fromStdString(entry.name), this);
            dlg->setAttribute(Qt::WA_DeleteOnClose);
            dlg->show();
        } else {
            QMessageBox::warning(this, tr("Error"), tr("Failed to read text file"));
        }
        QFile::remove(tempFile);
        return;
    }
    
    // For other files, just show info
    QMessageBox::information(this, tr("File Info"), 
        tr("File: %1\nSize: %2 bytes\n\n(Binary file - no preview available)")
        .arg(QString::fromStdString(entry.name))
        .arg(entry.size));
    QFile::remove(tempFile);
}

void IsoDialog::onSearchTextChanged(const QString& text) {
    // Simple search: hide items that don't match
    QString searchLower = text.toLower();
    
    QTreeWidgetItemIterator it(tree_);
    while (*it) {
        QTreeWidgetItem* item = *it;
        QString itemName = item->text(0).toLower();
        
        if (searchLower.isEmpty() || itemName.contains(searchLower)) {
            item->setHidden(false);
        } else {
            // Only hide files, not folders (folders might contain matching files)
            bool isFolder = item->data(0, Qt::UserRole + 1).toBool();
            if (!isFolder) {
                item->setHidden(true);
            }
        }
        ++it;
    }
}

void IsoDialog::onExpandAll() {
    tree_->expandAll();
}

void IsoDialog::onCollapseAll() {
    tree_->collapseAll();
}

