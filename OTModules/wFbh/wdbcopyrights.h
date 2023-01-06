#ifndef WDBCOPYRIGHTS_H
#define WDBCOPYRIGHTS_H

#include <QMainWindow>
#include "OTBackend/OTGlobal.h"
#include "OTBackend/OTDatabaseHandler.h"
#include "OTModules/wFbh/waddpath.h"
#include "OTModules/wFbh/waddterm.h"
#include <QtSql>

namespace Ui {
class wDBCopyrights;
}

class wDBCopyrights : public QMainWindow
{
    Q_OBJECT

public:
    explicit wDBCopyrights(QWidget *parent = nullptr);
    ~wDBCopyrights();

private slots:

    void on_btnPathSettingsAdd_clicked();

    void on_btnPathSettingsRemove_clicked();

    void on_btnCopyrightTermsAdd_clicked();

    void on_btnCopyrightTermsRemove_clicked();

    void on_tvwCopyrightTerms_activated(const QModelIndex &index);

    void on_tvwPathSettings_activated(const QModelIndex &index);

    void on_btnCreateBackup_clicked();

private:
    Ui::wDBCopyrights *ui;

    OTSettings set;
    OTMiscellaneous misc;

    QString dbPath = "C:/Users/pietr/OneDrive/Dev/OMSI-Tools/OMSI-Tools/data/db/fbhCopyrights.db";
    OTDatabaseHandler dbHandler;

    void updateView();
    void addTermFinished(int ID, QString argument);
    void addPathFinished(int ID, QString path, QString argumentIDs, int redirect);
};

#endif // WDBCOPYRIGHTS_H