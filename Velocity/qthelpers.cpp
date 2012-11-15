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

void QtHelpers::ParseHexStringBuffer(QString bytes, BYTE *outBuffer, DWORD len)
{
    bytes = bytes.trimmed().replace(" ", "");

    if (len != (bytes.length() / 2))
        throw QString("QT: Error converting hex string to byte array, length mismatch.\n");

    for (DWORD i = 0; i < len; i++)
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

QString QtHelpers::GetKVPath(ConsoleType type, QWidget *parent)
{
    QString kvName = ExecutingDirectory() + "KV_";
    if (type == Retail)
        kvName += "R.bin";
    else
        kvName += "D.bin";

    if (!QFile::exists(kvName))
    {
        kvName = QFileDialog::getOpenFileName(parent, "KV Location", DesktopLocation() + "/KV.bin");
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
                    }
                }
            }
        }
}
