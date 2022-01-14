#include "wfirstsetup.h"
#include "ui_wfirstsetup.h"

wFirstSetup::wFirstSetup(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::wFirstSetup)
{
    qInfo().noquote() << "Starting " + moduleName + "...";

    qDebug() << "Set up UI...";
    ui->setupUi(this);
    //adjustSize();
    qDebug() << "UI set";

    // Load settings
    setStyleSheet(set.read("main", "theme").toString());

    setWindowTitle(OTName + " - " + tr("first setup"));

    wSettings wSet;
    ui->btnEn->setText("\t" + wSet.en);
    ui->btnDe->setText("\t" + wSet.de);
    ui->btnFr->setText("\t" + wSet.fr);
    ui->btnIt->setText("\t" + wSet.it);

    qInfo().noquote() << moduleName + " started successfully.";
}

wFirstSetup::~wFirstSetup()
{
    delete ui;
}

/// \brief Sets language to english
void wFirstSetup::on_btnEn_clicked()
{
    set.write("main", "language", 0);
    misc.restart();
}

/// \brief Sets language to german
void wFirstSetup::on_btnDe_clicked()
{
    set.write("main", "language", 1);
    misc.restart();
}

/// \brief Sets language to french
void wFirstSetup::on_btnFr_clicked()
{
    set.write("main", "language", 2);
    misc.restart();
}

/// \brief Sets language to italian
void wFirstSetup::on_btnIt_clicked()
{
    set.write("main", "language", 3);
    misc.restart();
}