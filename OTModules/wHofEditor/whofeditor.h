#ifndef WHOFEDITOR_H
#define WHOFEDITOR_H

#include <QMainWindow>

namespace Ui {
class wHofEditor;
}

class wHofEditor : public QMainWindow
{
    Q_OBJECT

public:
    explicit wHofEditor(QWidget *parent = nullptr);
    ~wHofEditor();

private slots:
    void closeEvent(QCloseEvent *event);

signals:
    void backToHome();

private:
    Ui::wHofEditor *ui;
};

#endif // WHOFEDITOR_H
