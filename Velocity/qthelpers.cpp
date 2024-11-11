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

QString QtHelpers::DefaultLocation()
{
#if QT_VERSION >= 0x050000
    QString defaultLocation = QStandardPaths::standardLocations(QStandardPaths::HomeLocation).at(0);
#else
    QString defaultLocation = QDesktopServices::storageLocation(QDesktopServices::HomeLocation);
#endif

    return defaultLocation.replace("\\", "/");
}

bool QtHelpers::VerifyHexStringBuffer(QString bytes)
{
    return VerifyHexString(bytes.replace(" ", ""));
}

bool QtHelpers::VerifyHexString(QString str)
{
    str = str.replace("0x", "");
    for (int i = 0; i < str.length(); i++)
        if (!isxdigit(str.at(i).toLatin1()))
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
        QFileDialog dialog(parent, "KV Location", DefaultLocation() + "/KV.bin");
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
    if (rootWidget == nullptr)
        return;

    QObject *child = nullptr;
    QObjectList Containers;
    QObject *container  = nullptr;
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
#elif _WIN32
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
        if (container != nullptr)
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
                    // (if the object is not of the correct type, it will be nullptr)
                    QPushButton *button = qobject_cast<QPushButton *>(child);
                    QLabel *label = qobject_cast<QLabel *>(child);
                    QComboBox *cmbx = qobject_cast<QComboBox *>(child);
                    QLineEdit *ln = qobject_cast<QLineEdit *>(child);
                    QTreeWidget *tree = qobject_cast<QTreeWidget *>(child);
                    QPlainTextEdit *plain = qobject_cast<QPlainTextEdit *>(child);
                    QCheckBox *check = qobject_cast<QCheckBox *>(child);
                    QProgressBar *progress = qobject_cast<QProgressBar *>(child);
                    if (button != nullptr)
                    {
                        button->setMinimumHeight(ButtonHeight); // Win
                        button->setMaximumHeight(ButtonHeight); // Win
                        button->setFont(cntrlFont);
                    }
                    else if (cmbx != nullptr)
                    {
                        cmbx->setFont(cntrlFont);
                        cmbx->setMaximumHeight(cmbxHeight);
                    }
                    else if (label != nullptr)
                        label->setFont(txtFont);
                    else if (ln != nullptr)
                        ln->setFont(txtFont);
                    else if (tree != nullptr)
                    {
                        tree->header()->setFont(txtFont);
                    }
                    else if (plain != nullptr)
                        plain->setFont(txtFont);
                    else if (check != nullptr)
                        check->setFont(txtFont);
                    else if (progress != nullptr)
                        progress->setMinimumHeight(20);
                }
            }
        }
    }
}


void QtHelpers::SearchTreeWidget(QTreeWidget *widget, QLineEdit *searchWidget, QString searchString)
{
    QList<QTreeWidgetItem*> itemsMatched = widget->findItems(searchWidget->text(),
            Qt::MatchContains | Qt::MatchRecursive);

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
        while (parent != nullptr)
        {
            parent->setHidden(false);
            parent->setExpanded(true);
            parent = parent->parent();
        }

        // show the item itself
        itemsMatched.at(i)->setHidden(false);
    }
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

void QtHelpers::GetFileIcon(DWORD magic, QString fileName, QIcon &icon, QTreeWidgetItem &item)
{
    item.setData(1, Qt::UserRole, "");

    switch (magic)
    {
        case CON:
        case LIVE:
        case PIRS:
            icon = QIcon(":/Images/PackageFileIcon.png");
            item.setData(1, Qt::UserRole, "STFS");
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
        case 0x89504E47:    // PNG
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

QtHelpers::SubWindowEvents::SubWindowEvents([[maybe_unused]] QObject *parent)
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
