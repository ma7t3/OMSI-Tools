#ifndef WDGTAB_H
#define WDGTAB_H

#include "OTBackend/OTGlobal.h"
#include "OTModules/wContentSearch/wcontentsearch.h"

#include <QListWidget>
#include <QKeyEvent>

namespace Ui {
class wdgTab;
}

class OTVerificationOverviewData
{
public:
    OTVerificationOverviewData(int _missing, int _existing)
    {
        missing = _missing;
        existing = _existing;
        total = missing + existing;
    }

    int missing;
    int existing;
    int total;
};

class wdgTab : public QWidget
{
    Q_OBJECT

public:
    explicit wdgTab(QWidget *parent = nullptr);
    ~wdgTab();

    void clear();
    void add(QStringList items, bool isMissing);
    void apply();
    void setName(QString name);

    OTVerificationOverviewData getData();

private slots:
    void on_btnSearchForMissingElements_clicked();

    void on_btnCopySelectedElements_clicked();

    void on_twgItems_currentChanged(int index);

    void on_lwgAll_currentRowChanged(int currentRow);

    void on_ledPath_textChanged(const QString &arg1);

    void on_lwgMissing_currentRowChanged(int currentRow);

    void on_btnCopyPath_clicked();

private:
    Ui::wdgTab *ui;
    OTMiscellaneous misc;
    wContentSearch* WCONTENTSEARCH;

    QStringList existing, missing;

    bool isApplied = false;

    void copy(QList<QListWidgetItem*> items);
    void copy(QStringList items);
    void search(QList<QListWidgetItem*> items);
    void search(QStringList items);

    void setPath();
};


class EventFilterCopyElements : public QObject {
    Q_OBJECT

public:
    EventFilterCopyElements(QListWidget *listWidget) : lwg(listWidget) { }

protected:
    bool eventFilter(QObject *obj, QEvent *event) override
    {
        if (event->type() == QEvent::KeyPress)
        {
            QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
            if (keyEvent->modifiers() == Qt::ControlModifier && keyEvent->key() == Qt::Key_C)
            {
                QList<QListWidgetItem*> items = lwg->selectedItems();
                QString copystring;

                foreach (QListWidgetItem *current, items) copystring += current->text() + "\n";
                misc.copy(copystring);

                return true;
            }
        }
        return QObject::eventFilter(obj, event);
    }

private:
    QListWidget *lwg;
    OTMiscellaneous misc;
};

#endif // WDGTAB_H
