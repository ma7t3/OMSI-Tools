#ifndef WHOFEDITOR_H
#define WHOFEDITOR_H

#include <QMainWindow>

#include "data/hoffileroot.h"

namespace Ui {
class wHofEditor;
}

class wHofEditor : public QMainWindow {
    Q_OBJECT

public:
    explicit wHofEditor(QWidget *parent = nullptr);
    ~wHofEditor();

private slots:
    void closeEvent(QCloseEvent *event);

    void actionFileNew();
    void actionFileOpen();
    void actionFileSave();
    void actionFileSaveAs();
    void actionFileBackToHome();
    void actionFileClose();

    void saveToFile(const QString &fileName);

signals:
    void backToHome();

private:
    Ui::wHofEditor *ui;

    HofFileRoot *_hofFile;
};

#endif // WHOFEDITOR_H
