#include "whofeditor.h"
#include "ui_whofeditor.h"

#include <QCloseEvent>

#include <QFileDialog>
#include <QMessageBox>

wHofEditor::wHofEditor(QWidget *parent) : QMainWindow(parent), ui(new Ui::wHofEditor) {
    ui->setupUi(this);

    connect(ui->actionNew,        &QAction::triggered, this, &wHofEditor::actionFileNew);
    connect(ui->actionOpen,       &QAction::triggered, this, &wHofEditor::actionFileOpen);
    connect(ui->actionSave,       &QAction::triggered, this, &wHofEditor::actionFileSave);
    connect(ui->actionSaveAs,     &QAction::triggered, this, &wHofEditor::actionFileSaveAs);
    connect(ui->actionBackToHome, &QAction::triggered, this, &wHofEditor::actionFileBackToHome);
    connect(ui->actionClose,      &QAction::triggered, this, &wHofEditor::actionFileClose);
}

wHofEditor::~wHofEditor() {
    delete ui;
}

void wHofEditor::closeEvent(QCloseEvent *event) {
    event->accept();
}

void wHofEditor::actionFileNew() {

}

void wHofEditor::actionFileOpen() {
    const QString fileName = QFileDialog::getOpenFileName(this, "", "", tr("OMSI Hof File (*.hof)")); // TODO: Fix presets
    if(fileName.isEmpty())
        return;

    QFile f(fileName);
    if(!f.open(QFile::ReadOnly)) {
        QMessageBox::critical(this, tr("Failed reading file"), tr("Couldn't read file:\n%1\nError: %2").arg(fileName, f.errorString()));
        return;
    }

    const QByteArray content = f.readAll();
    // TODO: Pass content
    qInfo() << content;
}

void wHofEditor::actionFileSave() {
    if(true) { // file name is empty
        actionFileSaveAs();
    } else {
        // saveToFile(fileName);
    }
}

void wHofEditor::actionFileSaveAs() {
    const QString fileName = QFileDialog::getSaveFileName(this, "", "", tr("OMSI Hof File (*.hof)")); // TODO: Fix presets
    if(fileName.isEmpty())
        return;

    saveToFile(fileName);
}

void wHofEditor::actionFileBackToHome() {
    if (close()) emit backToHome();
}

void wHofEditor::actionFileClose() {
    close();
}

void wHofEditor::saveToFile(const QString &fileName) {
    qCritical() << "NOT IMPLEMENTED";
    return;

    QFile f(fileName);
    if(!f.open(QFile::WriteOnly)) {
        QMessageBox::critical(this, tr("Failed writing to file"), tr("Couldn't write to file:\n%1\nError: %2").arg(fileName, f.errorString()));
        return;
    }
}
