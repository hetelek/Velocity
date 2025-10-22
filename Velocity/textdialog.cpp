#include "textdialog.h"
#include "ui_textdialog.h"

TextDialog::TextDialog(const QString &textContent, const QString &fileName, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TextDialog)
{
    ui->setupUi(this);
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    
    // Set window title
    if (!fileName.isEmpty())
        setWindowTitle("Text Viewer - " + fileName);
    else
        setWindowTitle("Text Viewer");
    
    // Set the text content in the text edit
    ui->textEdit->setPlainText(textContent);
    
    // Make it read-only
    ui->textEdit->setReadOnly(true);
    
    // Use a monospace font for better readability
    QFont font("Courier New", 10);
    ui->textEdit->setFont(font);
}

TextDialog::~TextDialog()
{
    delete ui;
}
