#include "wstartupscreen.h"
#include "OTBackend/OTGlobal.h"

#include <QApplication>
#include <QTranslator>
#include <QDebug>
#include <QFile>
#include "OTBackend/OTLogger.h"

/// Test area for anything
void testArea()
{

}

/// Main function
int main(int argc, char *argv[])
{
    OTSettings set;
    OTMiscellaneous misc;
    OTFileOperations fop;

    QString newName;
    bool isCrash = false;
    if (set.read("main", "closeCheck") == "false")
    {
        isCrash = true;
        newName = QString("logfile_crash_%1.txt").arg(misc.getDate("dd-MM-yyyy") + "_" + misc.getTime("hh-mm-ss"));

        QFile::copy("logfile.txt", newName);
    }

    if ((set.read("main", "logfileMode") != -1))
    {
        Logger::attach();
        Logger::logging = true;
    }

    set.write("main", "closeCheck", false);

    qDebug().noquote() << "Settings" + set.getAllSettings() + "\n================================================\n";

    qInfo().noquote() << "Starting " + OTName + "...";

    QApplication a(argc, argv);

    set.loadTranslator();

    if (isCrash)
    {
        qWarning() << "Crash on last start detected! A logfile has been saved. Please contact the developer.";
        QMessageBox::StandardButton reply = QMessageBox::question(NULL, QObject::tr("Crash detected"), QObject::tr("%1 seems to have crashed on last launch. The logfile of the last start was saved separately. Please contact the developer with this.\nOpen the path of the logfile?").arg(OTName));

        if (reply == QMessageBox::Yes)
            fop.showInExplorer(newName);
    }
    else
        qDebug() << "No crash detected.";

    testArea();

    // If the application was updated, remove the temporary directory in the temp folder
    if ((QCoreApplication::arguments().count() >= 2) && (QCoreApplication::arguments().at(1) == "updateInstalled"))
    {
        qInfo() << "Update installed, remove old files...";
        QDir(QDir::tempPath() + "/OMSI-Tools_tempAppDir").removeRecursively();
    }

    wStartUpScreen *WSTARTUPSCREEN;
    wFbh *WFBH;

    if ((QCoreApplication::arguments().count() >= 2) && (QCoreApplication::arguments().at(1) == "openFbh"))
    {
        WFBH = new wFbh();
        WFBH->show();
    }
    else
    {
        WSTARTUPSCREEN = new wStartUpScreen();
        WSTARTUPSCREEN->show();
    }

    int exec = a.exec();

    qInfo().noquote() << OTName + " is closing...";

    set.write("main", "closeCheck", true);

    return exec;
}

