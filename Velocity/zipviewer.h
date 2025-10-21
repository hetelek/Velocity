#ifndef ZIPVIEWER_H
#define ZIPVIEWER_H

#include <QDialog>
#include <QString>
#include <QTableWidgetItem>

// Custom table item for numeric size sorting
class NumericSizeItem : public QTableWidgetItem
{
public:
    NumericSizeItem(const QString &text, qulonglong numericValue)
        : QTableWidgetItem(text), numericValue(numericValue) {}
    
    bool operator<(const QTableWidgetItem &other) const override
    {
        // Sort by numeric value stored in UserRole
        return data(Qt::UserRole).toULongLong() < other.data(Qt::UserRole).toULongLong();
    }

private:
    qulonglong numericValue;
};

namespace Ui {
class ZipViewer;
}

class ZipViewer : public QDialog
{
    Q_OBJECT

public:
    explicit ZipViewer(const QString &zipPath, const QString &fileName, QWidget *parent = nullptr);
    ~ZipViewer();

private slots:
    void onExtractSelected();
    void onExtractAll();

private:
    Ui::ZipViewer *ui;
    QString zipPath;
    void loadZipContents();
};

#endif // ZIPVIEWER_H
