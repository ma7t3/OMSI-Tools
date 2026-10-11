#include "hoffileterminus.h"

HofFileTerminus::HofFileTerminus(QObject *parent) : QObject{parent}, _ibisCode{0}, _noPassengers{false} {}

QString HofFileTerminus::name() const {
    return _name;
}

void HofFileTerminus::setName(const QString &newName) {
    _name = newName;
    emit nameChanged(newName);
}

int HofFileTerminus::ibisCode() const {
    return _ibisCode;
}

void HofFileTerminus::setIbisCode(int newIbisCode) {
    _ibisCode = newIbisCode;
    emit ibisCodeChanged(newIbisCode);
}

bool HofFileTerminus::noPassengers() const {
    return _noPassengers;
}

void HofFileTerminus::setNoPassengers(bool newNoPassengers) {
    _noPassengers = newNoPassengers;
    emit noPassengersChanged(newNoPassengers);
}

int HofFileTerminus::stringCount() const {
    return _strings.count();
}

void HofFileTerminus::setStringCount(int newStringCount) {
    bool countChanged = false;

    while(stringCount() < newStringCount) {
        _strings.append("");
        countChanged = true;
    }

    while(stringCount() > newStringCount) {
        _strings.removeLast();
        countChanged = true;
    }

    if(countChanged)
        emit stringCountChanged(stringCount());
}

void HofFileTerminus::increaseStringCount(int increaseCountBy) {
    setStringCount(stringCount() + increaseCountBy);
}

QStringList HofFileTerminus::strings() const {
    return _strings;
}

QString HofFileTerminus::string(int index) const {
    return _strings[index];
}

void HofFileTerminus::setStrings(const QStringList &newStrings) {
    _strings = newStrings;
    emit stringsReset(newStrings);
}

void HofFileTerminus::setString(int index, const QString &newValue) {
    _strings[index] = newValue;
    emit stringChanged(index, newValue);
}

QByteArray HofFileTerminus::data() const {
    QString result;
    if(_noPassengers)
        result += "{ALLEX}";

    result += "\t" + name() + "\t" + _strings.join("\t");
    return result.toLatin1();
}

void HofFileTerminus::setData(const QByteArray &newData) {
    const QStringList list = QString::fromLatin1(newData).split("\t");
    auto it = list.begin();
    if(it != list.end())
        setNoPassengers(*it == "{ALLEX}");

    ++it;
    if(it != list.end())
        setName(*it);

    QStringList strings;
    for(; it != list.end(); ++it)
        strings << *it;

    setStrings(strings);
}
