#include "wfeedback.h"
#include "ui_wfeedback.h"

wFeedback::wFeedback(QWidget *parent, QUrl wikiEntry) :
    QMainWindow(parent),
    ui(new Ui::wFeedback)
{
    qInfo().noquote() << "Starting " + objectName() + "...";

    qDebug() << "Set up UI...";
    ui->setupUi(this);
    adjustSize();
    qDebug() << "UI set";

    setWindowFlags(Qt::Window | Qt::WindowCloseButtonHint);

    // Load prefs
    setWindowTitle(tr("Feedback") + " - " + OTInformation::name);

    if (wikiEntry.isEmpty())
        ui->btnWikiEntry->setEnabled(false);

    wikiurl = wikiEntry;

    qInfo().noquote() << objectName() + " started";
}

wFeedback::~wFeedback()
{
    delete ui;
}

void wFeedback::on_btnOpenSupportThread_clicked()
{
    QDesktopServices::openUrl(OTLinks::support);
    close();
}

void wFeedback::on_btnOpenSurvey_clicked()
{
    QDesktopServices::openUrl(OTLinks::feedbackForm);
    close();
}

void wFeedback::on_btnWikiEntry_clicked()
{
    QDesktopServices::openUrl(wikiurl);
    close();
}


void wFeedback::on_btnCopyMail_clicked()
{
    misc.copy(ui->ledMail->text());
}

