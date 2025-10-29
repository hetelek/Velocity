#pragma once
#include <QDialog>
#include <QMap>
#include <memory>

QT_BEGIN_NAMESPACE
class QPushButton;
class QTreeWidget;
class QTreeWidgetItem;
class QLineEdit;
class QDragEnterEvent;
class QDropEvent;
QT_END_NAMESPACE

namespace XboxInternals::Iso { 
    class IsoImage; 
    struct IsoEntry;
}

class IsoDialog : public QDialog {
    Q_OBJECT
public:
    explicit IsoDialog(QWidget* parent = nullptr);
    ~IsoDialog() override;

    // Load an ISO or GOD file
    void loadIsoContents(const QString& path);

protected:
    void dragEnterEvent(QDragEnterEvent* event) override;
    void dropEvent(QDropEvent* event) override;

private slots:
    void onExtractSelected();
    void onExtractAll();
    void onItemDoubleClicked(QTreeWidgetItem* item, int column);
    void onSearchTextChanged(const QString& text);
    void onExpandAll();
    void onCollapseAll();

private:
    void populateTree();
    void openFileInViewer(const XboxInternals::Iso::IsoEntry& entry);
    QTreeWidgetItem* findOrCreateFolder(const QString& path);
    
    QPushButton* extractSelectedButton_ = nullptr;
    QPushButton* extractAllButton_ = nullptr;
    QPushButton* expandAllButton_ = nullptr;
    QPushButton* collapseAllButton_ = nullptr;
    QLineEdit* searchBox_ = nullptr;
    QTreeWidget* tree_ = nullptr;
    
    std::unique_ptr<XboxInternals::Iso::IsoImage> iso_;
    QString currentIsoPath_;
    QMap<QString, QTreeWidgetItem*> folderItems_;
};

