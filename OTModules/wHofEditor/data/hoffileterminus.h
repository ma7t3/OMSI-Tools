#ifndef HOFFILETERMINUS_H
#define HOFFILETERMINUS_H

#include <QObject>

class HofFileRoot;

class HofFileTerminus : public QObject {
    friend class HofFileRoot;

    Q_OBJECT

private:
    explicit HofFileTerminus(QObject *parent = nullptr);

public:
    QString name() const;
    void setName(const QString &newName);

    int ibisCode() const;
    void setIbisCode(int newIbisCode);

    bool noPassengers() const;
    void setNoPassengers(bool newNoPassengers);

    int stringCount() const;
    void setStringCount(int newStringCount);
    void increaseStringCount(int increaseCountBy = 1);

    QStringList strings() const;
    QString string(int index) const;

    void setStrings(const QStringList &newStrings);
    void setString(int index, const QString &newValue);

protected:
    QString data() const;
    void setData(const QString &newData);

signals:
    void nameChanged(const QString &newName);
    void ibisCodeChanged(int newIbisCode);
    void noPassengersChanged(bool newNoPassengers);
    void stringChanged(int index, const QString &newValue);
    void stringsReset(const QStringList &newStrings);
    void stringCountChanged(int newStringCount);

private:
    QString _name;
    int _ibisCode;
    bool _noPassengers;
    QStringList _strings;
};

#endif // HOFFILETERMINUS_H
