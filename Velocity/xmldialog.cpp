#include "xmldialog.h"
#include "ui_xmldialog.h"

XmlDialog::XmlDialog(const QString &xmlContent, const QString &fileName, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::XmlDialog)
{
    ui->setupUi(this);
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    
    // Set window title
    if (!fileName.isEmpty())
        setWindowTitle("XML Viewer - " + fileName);
    else
        setWindowTitle("XML Viewer");
    
    // Set the XML content in the text edit
    ui->textEdit->setPlainText(xmlContent);
    
    // Make it read-only
    ui->textEdit->setReadOnly(true);
    
    // Use a monospace font for better readability
    QFont font("Courier New", 10);
    ui->textEdit->setFont(font);
}

XmlDialog::~XmlDialog()
{
    delete ui;
}
