#include "whofeditor.h"
#include "ui_whofeditor.h"

#include <QCloseEvent>

wHofEditor::wHofEditor(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::wHofEditor)
{
    ui->setupUi(this);
}

wHofEditor::~wHofEditor()
{
    delete ui;
}

void wHofEditor::closeEvent(QCloseEvent *event)
{
    event->accept();
}
