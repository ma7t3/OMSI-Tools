#include "hoffileroot.h"

#include "hoffileterminus.h"

HofFileRoot::HofFileRoot(QObject *parent) : QObject{parent}, _serviceTripTerminus{nullptr} {
    connect(&_undoStack, &QUndoStack::cleanChanged, this, &HofFileRoot::isSavedChanged);
}

QString HofFileRoot::fileName() const {
    return _fileName;
}

void HofFileRoot::setFileName(const QString &fileName) {
    if(_fileName == fileName)
        return;

    _fileName = fileName;
    emit fileNameChanged(_fileName);
}

void HofFileRoot::setData(const QByteArray &data) {
    clear();
    // TODO
}

QByteArray HofFileRoot::data() const {
    // TODO
    return QByteArray();
}

void HofFileRoot::clear() {
    setFileName(QString());
    setDisplayName(QString());

    setServiceTripTerminus(nullptr);
    setGlobalStrings({});

    deleteAllLater(_termini);
    setTermini({});

    //deleteAllLater(_busstops);
    setBusstops({});

    //deleteAllLater(_routes);
    setRoutes({});

    _undoStack.clear();
}

bool HofFileRoot::isSaved() const {
    return _undoStack.isClean();
}

void HofFileRoot::setSaved(bool b) {
    _undoStack.setClean();
}

HofFileTerminus *HofFileRoot::createTerminus(const QString &data) {
    HofFileTerminus *terminus = new HofFileTerminus(this);
    terminus->setData(data);
    // TODO: CONNECT SIGNALS
    return terminus;
}

QString HofFileRoot::displayName() const {
    return _displayName;
}

void HofFileRoot::setDisplayName(const QString &newDisplayName) {
    if (_displayName == newDisplayName)
        return;
    _displayName = newDisplayName;
    emit displayNameChanged(newDisplayName);
}

HofFileTerminus *HofFileRoot::serviceTripTerminus() const {
    return _termini.contains(_serviceTripTerminus) ? _serviceTripTerminus : nullptr;
}

void HofFileRoot::setServiceTripTerminus(HofFileTerminus *newServiceTripTerminus) {
    if (_serviceTripTerminus == newServiceTripTerminus)
        return;
    _serviceTripTerminus = newServiceTripTerminus;
    emit serviceTripTerminusChanged(newServiceTripTerminus);
}

QStringList HofFileRoot::globalStrings() const {
    return _globalStrings;
}

void HofFileRoot::setGlobalStrings(const QStringList &newGlobalStrings) {
    if(_globalStrings == newGlobalStrings)
        return;

    _globalStrings = newGlobalStrings;
    emit globalStringsReset(_globalStrings);
}

QList<HofFileTerminus *> HofFileRoot::termini() const {
    return _termini;
}

void HofFileRoot::setTermini(const QList<HofFileTerminus *> &newTermini) {
    if(_termini == newTermini)
        return;

    _termini = newTermini;
    emit terminiReset(_termini);
}

QList<HofFileBusstop *> HofFileRoot::busstops() const {
    return _busstops;
}

void HofFileRoot::setBusstops(const QList<HofFileBusstop *> &newBusstops) {
    if(_busstops == newBusstops)
        return;

    _busstops = newBusstops;
    emit busstopsReset(_busstops);
}

QList<HofFileRoute *> HofFileRoot::routes() const {
    return _routes;
}

void HofFileRoot::setRoutes(const QList<HofFileRoute *> &newRoutes) {
    if(_routes == newRoutes)
        return;

    _routes = newRoutes;
    emit routesReset(_routes);
}
