#include "wstart.h"
#include "ui_wstart.h"

wStart::wStart(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::wStart)
{
    qInfo().noquote() << "Starting " + moduleName + "...";

    qDebug() << "Set up UI...";
    ui->setupUi(this);
    qDebug() << "UI set";

    // Load settings
    setStyleSheet(set.read("main", "theme").toString());

    setWindowTitle(OTName + " " + OTVersion);

#ifdef QT_DEBUG
    setWindowTitle("[Debug Build] " + windowTitle());
#endif

    // Prepare GUI:
    bool isNoRelease;

    if (OTBuild == OTBuildOptions::Release || OTBuild == OTBuildOptions::EA ||
        OTBuild == OTBuildOptions::Lite || OTBuild == OTBuildOptions::Prerelease)
        isNoRelease = false;
    else
        isNoRelease = true;

    ui->btnSimulateCrash->setVisible(isNoRelease);
    ui->gbxDevTools->setVisible(isNoRelease);
    ui->actionSendFeedback->setVisible(isNoRelease);

    if (!QFile("Fbh.unlock").exists())
        ui->gbxFbh->setVisible(false);

    adjustSize();

    ui->actionAbout->setText(tr("About %1").arg(OTName));

    ui->fraUpdate->setVisible(false);

    QVariant checkVersion = set.read("main", "autoUpdateCheck");
    QVariant lastAutoUpdateCheck = set.read("main", "lastAutoUpdateCheck").toString();
    bool checkForUpdate = false;

    QDate lastCheck;
    lastCheck.setDate(lastAutoUpdateCheck.toString().remove(4, 4).toInt(),
                      lastAutoUpdateCheck.toString().remove(0, 4).remove(2, 2).toInt(),
                      lastAutoUpdateCheck.toString().remove(0, 6).toInt());

    if (!checkVersion.isValid())
    {
        checkForUpdate = true;
        set.write("main", "autoUpdateCheck", 2);
    }
    else if (checkVersion == 1)
        checkForUpdate = true;
    else if ((!lastAutoUpdateCheck.isValid()) && (checkVersion != 0))
        checkForUpdate = true;
    else if ((checkVersion == 2) && (lastAutoUpdateCheck.toString() != misc.getDate("yyyyMMdd")))
        checkForUpdate = true;
    else if (checkVersion == 3)
    {
        if (QDate::currentDate().toString("yyyyMMdd") >= lastCheck.addDays(7).toString("yyyyMMdd"))
            checkForUpdate = true;
    }
    else if (checkVersion == 4)
    {
        if (QDate::currentDate().toString("yyyyMMdd") >= lastCheck.addMonths(1).toString("yyyyMMdd"))
            checkForUpdate = true;
    }

    if (checkForUpdate)
    {
        QStringList update = misc.getUpdateInformation();
        set.write("main", "lastAutoUpdateCheck", misc.getDate("yyyyMMdd"));

        if (update.at(0) == "noUpdates")
            ui->statusbar->showMessage(tr("No updates available."), 30000);
        else if (update.at(0) != "false")
        {
            ui->lblUpdate->setText(tr("New update available") + " •");
            ui->fraUpdate->setVisible(true);
            ui->lblUpdateVersion->setText(update.at(1));

            updateVersion = update.at(1);
        }
    }

    qInfo().noquote() << moduleName + " started";
}

wStart::~wStart()
{
    qInfo().noquote() << moduleName << "is closing...";
    delete ui;
}

/// Opens the settings
void wStart::on_actionSettings_triggered()
{
    WSETTINGS = new wSettings(this);
    WSETTINGS->setWindowModality(Qt::ApplicationModal);
    WSETTINGS->show();
}

/// Shows the about message
void wStart::on_actionAbout_triggered()
{
    WABOUT = new wAbout(this);
    WABOUT->setWindowModality(Qt::ApplicationModal);
    WABOUT->show();
}

/// Closes the apllication
void wStart::on_actionClose_triggered()
{
    QApplication::quit();
}

/// Reopen test, coming soon... https://trello.com/c/8nOzgfSg
void wStart::reopenTest(QObject*)
{
    show();
    qDebug() << "Show!";
}

/// Opens the font module
void wStart::on_btnFonts_clicked()
{
    hide();
    WFONTS = new wFonts();
    WFONTS->show();
}

/// Shows "Where should I release my modification" message
void wStart::on_btnWhereShouldIReleaseMyModification_clicked()
{
    QMessageBox::information(this, tr("Where should I release my modification?"), tr("Coming soon!"));
}

/// Opens verify map module
void wStart::on_btnVerifyMap_clicked()
{
    hide();
    WVERIFYMAP = new wVerifyMap();
    WVERIFYMAP->show();
}

/// Opens DevTools
void wStart::on_btnDevTools_clicked()
{
    close();
    WDEVTOOLS = new wDevTools();
    WDEVTOOLS->show();
}

/// Opens database panel for content search
void wStart::on_btnDBPanel_clicked()
{
    close();
    WDBPANEL = new wDBPanel();
    WDBPANEL->show();
}

/// Opens content search module
void wStart::on_btnContentSearch_clicked()
{
    close();
    WCONTENTSEARCH = new wContentSearch();
    WCONTENTSEARCH->show();
}

/// Starts crash simulation
void wStart::on_btnSimulateCrash_clicked()
{
    QMessageBox::StandardButton reply = QMessageBox::question(this, tr("Simulate crash"), tr("Should an appliaction crash be simulated?"));

    if (reply == QMessageBox::Yes)
        qFatal("Crash simulation started by user");
}

/// Opens bug report module
void wStart::on_actionSendFeedback_triggered()
{
    misc.sendFeedback();
}

/// Opens the changelog
void wStart::on_actionChangelog_triggered()
{
    WCHANGELOG = new wChangelog(this);
    WCHANGELOG->setWindowModality(Qt::ApplicationModal);
    WCHANGELOG->show();
}

/// Opens the manual, currenty only in german
void wStart::on_actionManual_triggered()
{
    QDesktopServices::openUrl(QUrl("file:///" + QApplication::applicationDirPath() + "/_docs/OMSI-Tools Lite - Handbuch DE.pdf"));
}

/// Opens Filebase-Helper
void wStart::on_btnOpenFbh_clicked()
{
    hide();
    WFBH = new wFbh();
    WFBH->show();
}

/// Creates shortcut for Filebase-Helper
void wStart::on_btnCreateShortcut_clicked()
{
    fop.createShortcut(qApp->applicationFilePath(), QDir().homePath() + QString("/Desktop/Filebase-Helper.lnk"), this);
    QMessageBox::information(this, "Verknüpfung erstellt", "Die gesetzte Verknüpfung muss jedoch noch modifiziert werden. Wie genau das funktioniert, kannst du im entsprechenden Thread im internen OWD-Forum sehen.");
}

/// Opens style test
void wStart::on_btnStyleTest_clicked()
{
    hide();
    WSTYLETEST = new wStyleTest();
    WSTYLETEST->show();
}


void wStart::on_btnView_clicked()
{
    WCHANGELOG = new wChangelog(this, true, updateVersion);
    WCHANGELOG->setWindowModality(Qt::ApplicationModal);
    WCHANGELOG->show();
}
