#include "qthelpers.h"

QString QtHelpers::ByteArrayToString(BYTE *buffer, DWORD len, bool spacesBetween)
{
    if (spacesBetween)
    {
        QString builder = "";
        for (DWORD i = 0; i < len; i++)
            builder += QString("%1").arg((BYTE)buffer[i], 2, 16, QChar('0')).toUpper() + " ";
        return builder;
    }
    else
    {
        QString builder = "";
        for (DWORD i = 0; i < len; i++)
            builder += QString("%1").arg((BYTE)buffer[i], 2, 16, QChar('0')).toUpper();
        return builder;
    }
}

DWORD QtHelpers::ParseHexString(QString string)
{
    string.replace("0x", "");
    return string.toULong(0, 16);
}

QString QtHelpers::ToHexString(UINT64 num)
{
    return "0x" + QString::number(num, 16).toUpper();
}

void QtHelpers::ParseHexStringBuffer(QString bytes, BYTE *outBuffer, int len)
{
    bytes = bytes.trimmed().replace(" ", "");

    if (len != (bytes.length() / 2))
        throw QString("QT: Error converting hex string to byte array, length mismatch.\n");

    for (int i = 0; i < len; i++)
    {
        QString temp = bytes.mid(i * 2, 2);
        outBuffer[i] = temp.toInt(0, 16);
    }
}

QString QtHelpers::DesktopLocation()
{
    return QDesktopServices::storageLocation(QDesktopServices::DesktopLocation).replace("\\", "/");
}

bool QtHelpers::VerifyHexStringBuffer(QString bytes)
{
    return VerifyHexString(bytes.replace(" ", ""));
}

bool QtHelpers::VerifyHexString(QString str)
{
    str = str.replace("0x", "");
    for (int i = 0; i < str.length(); i++)
        if (!isxdigit(str.at(i).toAscii()))
            return false;
    return true;
}

#include <QDebug>

std::string QtHelpers::GetKVPath(ConsoleType type, QWidget *parent)
{
    std::string kvName = ExecutingDirectory().toStdString() + "KV_";
    if (type == Retail)
        kvName += "R.bin";
    else
        kvName += "D.bin";

    if (!QFile::exists(QString::fromStdString(kvName)))
    {
        QFileDialog dialog(parent, "KV Location", DesktopLocation() + "/KV.bin");
        dialog.setFileMode(QFileDialog::ExistingFile);
        dialog.setViewMode(QFileDialog::Detail);

        if (dialog.exec() == QFileDialog::Accepted)
        {
            QStringList files = dialog.selectedFiles();

            if(files.size() > 0)
                kvName = files.at(0).toStdString();
        }
        else
            kvName = "";
    }
    return kvName;
}

bool QtHelpers::VerifyDecimalString(QString str)
{
    for (int i = 0; i< str.length(); i++)
        if (!str.at(i).isDigit())
            return false;
    return true;
}

bool QtHelpers::ParseVersionString(QString version, Version *out)
{
    QStringList segs = version.split('.');

    if (segs.length() != 4)
        return false;

    bool ok;
    out->major = segs.at(0).toUShort(&ok);
    if (!ok)
        return false;

    out->minor = segs.at(1).toUShort(&ok);
    if (!ok)
        return false;

    out->build = segs.at(2).toUShort(&ok);
    if (!ok)
        return false;

    out->revision = segs.at(3).toUShort(&ok);
    if (!ok)
        return false;

    if (out->major > 15)
        return false;
    if (out->minor > 15)
        return false;
/*
    if (out->build > 0xFFFF)
        return false;
*/
    if (out->revision > 0xFF)
        return false;

    return true;
}

QString QtHelpers::ExecutingDirectory()
{
    return QCoreApplication::applicationDirPath() + "/";
}

void QtHelpers::GenAdjustWidgetAppearanceToOS(QWidget *rootWidget)
{
    if (rootWidget == NULL)
            return;

        QObject *child = NULL;
        QObjectList Containers;
        QObject *container  = NULL;
        QStringList DoNotAffect;

        // Make an exception list (Objects not to be affected)
        DoNotAffect.append("aboutTitleLabel");     // about Dialog
        DoNotAffect.append("aboutVersionLabel");   // about Dialog
        DoNotAffect.append("aboutCopyrightLabel"); // about Dialog
        DoNotAffect.append("aboutUrlLabel");       // about Dialog
        DoNotAffect.append("aboutLicenseLabel");   // about Dialog

        // Set sizes according to OS:
    #ifdef __APPLE__
        int ButtonHeight = 35;
        int cmbxHeight = 30;
        QFont cntrlFont("Myriad Pro", 14);
        QFont txtFont("Myriad Pro", 14);
    #elif _WIN32 // Win XP/7
        int ButtonHeight = 24;
        int cmbxHeight = 20;
        QFont cntrlFont("MS Shell Dlg 2", 8);
        QFont txtFont("MS Shell Dlg 2", 8);
    #else
        int ButtonHeight = 24;
        int cmbxHeight = 24;
        QFont cntrlFont("Ubuntu Condensed", 10);
        QFont txtFont("Ubuntu", 10);
    #endif

        // Append root to containers
        Containers.append(rootWidget);
        while (!Containers.isEmpty())
        {
            container = Containers.takeFirst();
            if (container != NULL)
            {
                for (int ChIdx=0; ChIdx < container->children().size(); ChIdx++)
                {
                    child = container->children()[ChIdx];
                    if (!child->isWidgetType() || DoNotAffect.contains(child->objectName()))
                        continue;
                    // Append containers to Stack for recursion
                    if (child->children().size() > 0)
                        Containers.append(child);
                    else
                    {
                        // Cast child object to button and label
                        // (if the object is not of the correct type, it will be NULL)
                        QPushButton *button = qobject_cast<QPushButton *>(child);
                        QLabel *label = qobject_cast<QLabel *>(child);
                        QComboBox *cmbx = qobject_cast<QComboBox *>(child);
                        QLineEdit *ln = qobject_cast<QLineEdit *>(child);
                        QTreeWidget *tree = qobject_cast<QTreeWidget *>(child);
                        QPlainTextEdit *plain = qobject_cast<QPlainTextEdit *>(child);
                        QCheckBox *check = qobject_cast<QCheckBox *>(child);
                        QProgressBar *progress = qobject_cast<QProgressBar *>(child);
                        if (button != NULL)
                        {
                            button->setMinimumHeight(ButtonHeight); // Win
                            button->setMaximumHeight(ButtonHeight); // Win
                            button->setFont(cntrlFont);
                        }
                        else if (cmbx != NULL)
                        {
                            cmbx->setFont(cntrlFont);
                            cmbx->setMaximumHeight(cmbxHeight);
                        }
                        else if (label != NULL)
                            label->setFont(txtFont);
                        else if (ln != NULL)
                            ln->setFont(txtFont);
                        else if (tree != NULL)
                        {
                            tree->header()->setFont(txtFont);
                        }
                        else if (plain != NULL)
                            plain->setFont(txtFont);
                        else if (check != NULL)
                            check->setFont(txtFont);
                        else if (progress != NULL)
                            progress->setMinimumHeight(20);
                    }
                }
            }
        }
}


void QtHelpers::SearchTreeWidget(QTreeWidget *widget, QLineEdit *searchWidget, QString searchString)
{
    QList<QTreeWidgetItem*> itemsMatched = widget->findItems(searchWidget->text(), Qt::MatchContains | Qt::MatchRecursive);

    for (int i = 0; i < widget->topLevelItemCount(); i++)
        QtHelpers::HideAllItems(widget->topLevelItem(i));

    if (itemsMatched.count() == 0 || searchString == "")
    {
        searchWidget->setStyleSheet("color: rgb(255, 1, 1);");
        for (int i = 0; i < widget->topLevelItemCount(); i++)
        {
            QtHelpers::ShowAllItems(widget->topLevelItem(i));
            QtHelpers::CollapseAllChildren(widget->topLevelItem(i));
        }
        return;
    }

    searchWidget->setStyleSheet("");
    // add all the matched ones to the list
    for (int i = 0; i < itemsMatched.count(); i++)
    {
        // show all the item's parents
        QTreeWidgetItem *parent = itemsMatched.at(i)->parent();
        while (parent != NULL)
        {
            widget->setItemHidden(parent, false);
            parent->setExpanded(true);
            parent = parent->parent();
        }

        // show the item itself
        widget->setItemHidden(itemsMatched.at(i), false);
    }
}

QTreeWidgetItem *QtHelpers::GetRootLevelTreeWidgetItem(QTreeWidgetItem *item)
{
    while (item->parent() != NULL)
        item = item->parent();
    return item;
}

void QtHelpers::HideAllItems(QTreeWidgetItem *parent)
{
    for (int i = 0; i < parent->childCount(); i++)
    {
        if (parent->child(i)->childCount() != 0)
            QtHelpers::HideAllItems(parent->child(i));
        parent->child(i)->setHidden(true);
    }
    parent->setHidden(true);
}

void QtHelpers::ShowAllItems(QTreeWidgetItem *parent)
{
    for (int i = 0; i < parent->childCount(); i++)
    {
        if (parent->child(i)->childCount() != 0)
            QtHelpers::HideAllItems(parent->child(i));
        parent->child(i)->setHidden(false);
    }
    parent->setHidden(false);
}

void QtHelpers::CollapseAllChildren(QTreeWidgetItem *item)
{
    item->setExpanded(false);

    // collapse all children
    for (int i = 0; i < item->childCount(); i++)
        QtHelpers::CollapseAllChildren(item->child(i));
}

void QtHelpers::GetFileIcon(DWORD magic, QString fileName, QIcon &icon, QTreeWidgetItem &item, FileSystem fileSystem)
{
    item.setData(1, Qt::UserRole, "");

    switch (magic)
    {
        case CON:
        case LIVE:
        case PIRS:
            icon = QIcon(":/Images/PackageFileIcon.png");
            if (fileSystem == FileSystemSTFS)
                item.setData(1, Qt::UserRole, "STFS");
            else if (fileSystem == FileSystemSVOD)
                item.setData(1, Qt::UserRole, "SVOD");
            break;
        case 0x58444246:    // Xdbf
            icon = QIcon(":/Images/GpdFileIcon.png");
            item.setData(1, Qt::UserRole, "Xdbf");
            break;
        case 0x53545242:    // STRB
            icon = QIcon(":/Images/StrbFileIcon.png");
            item.setData(1, Qt::UserRole, "STRB");
            break;
        case 0x58455832:    // XEX2
            icon = QIcon(":/Images/XEXFileIcon.png");
            item.setData(1, Qt::UserRole, "XEX");
            break;
        case 0x89504E47:    // PNG
            icon = QIcon(":/Images/ImageFileIcon.png");
            item.setData(1, Qt::UserRole, "Image");
            break;
        default:
            int index = fileName.lastIndexOf(".");
            QString extension = "";
            if (index != -1)
                extension = fileName.mid(index);

            if (fileName == "Account")
                icon = QIcon(":/Images/AccountFileIcon.png");
            else if (fileName == "PEC")
            {
                icon = QIcon(":/Images/PecFileIcon.png");
                item.setData(1, Qt::UserRole, "PEC");
            }
            else if (extension == ".jpg" || extension == ".jpeg")
            {
                icon = QIcon(":/Images/ImageFileIcon.png");
                item.setData(1, Qt::UserRole, "Image");
            }
            else
                icon = QIcon(":/Images/DefaultFileIcon.png");
    }
}

void QtHelpers::AddSubWindow(QMdiArea *mdiArea, QWidget *widget)
{
    widget->installEventFilter(new SubWindowEvents(widget));
    mdiArea->addSubWindow(widget);
}

QtHelpers::SubWindowEvents::SubWindowEvents(QObject *parent)
{
}

bool QtHelpers::SubWindowEvents::eventFilter(QObject *obj, QEvent *event)
{
    QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);

    if (event->type() == QEvent::KeyPress && keyEvent->key() == Qt::Key_Escape) 
    {
        return true;
    } 
    
    return QObject::eventFilter(obj, event);
}

QStringList QtHelpers::StdStringArrayToQStringList(std::vector<std::string> strings)
{
    QStringList toReturn;
    for (size_t i = 0; i < strings.size(); i++)
        toReturn.append(QString::fromStdString(strings.at(i)));
    return toReturn;
}

void QtHelpers::DrawFreeMemoryGraph(FatxDrive *drive, QLabel *graph, QColor backgroundColor, QLabel *freeMemLegendColor, QLabel *freeMemLegend,
                                    QLabel *usedMemLengendColor, QLabel *usedMemLegend, bool contentOnly, void(*progress)(void*, bool))
{
    UINT64 totalFreeSpace = 0;
    UINT64 totalSpace = 0;

    // load the partion information
    std::vector<Partition*> parts = drive->GetPartitions();
    for (DWORD i = 0; i < parts.size(); i++)
    {
        if (!contentOnly || parts.at(i)->name == "Content")
        {
            totalFreeSpace += drive->GetFreeMemory(parts.at(i), progress);
            totalSpace += (UINT64)parts.at(i)->clusterCount * parts.at(i)->clusterSize;
        }
    }

    // calculate the percentage
    float freeMemPercentage = (((float)totalFreeSpace * 100.0) / totalSpace);

    // draw the insano piechart
    QPixmap chart(750, 500);
    chart.fill(backgroundColor);
    QPainter painter(&chart);
    Nightcharts pieChart;
    pieChart.setType(Nightcharts::Dpie);
    pieChart.setCords(25, 1, 700, 425);
    pieChart.setFont(QFont());
    pieChart.addPiece("Used Space", QColor(0, 0, 254), 100.0 - freeMemPercentage);
    pieChart.addPiece("Free Space", QColor(255, 0, 254), freeMemPercentage);
    pieChart.draw(&painter);

    graph->setPixmap(chart);

    // setup the legend
    QPixmap freeMemClr(16, 16);
    freeMemClr.fill(QColor(255, 0, 254));
    freeMemLegendColor->setPixmap(freeMemClr);
    freeMemLegend->setText(QString::fromStdString(ByteSizeToString(totalFreeSpace)) + " of Free Space");

    QPixmap usedMemClr(16, 16);
    usedMemClr.fill(QColor(0, 0, 254));
    usedMemLengendColor->setPixmap(usedMemClr);
    usedMemLegend->setText(QString::fromStdString(ByteSizeToString(totalSpace - totalFreeSpace)) + " of Used Space");
}
