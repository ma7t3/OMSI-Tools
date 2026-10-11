#include "hoffileroot.h"

#include "hoffileterminus.h"

HofFileRoot::HofFileRoot(QObject *parent) : QObject{parent}, _serviceTripTerminus{nullptr} {
    connect(&_undoStack, &QUndoStack::cleanChanged, this, &HofFileRoot::isSavedChanged);
}

QString HofFileRoot::fileName() const {
    return _fileName;
}

void HofFileRoot::setFileName(const QString &fileName) {
    _fileName = fileName;
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
    _fileName.clear();
    emit fileNameChanged(_fileName);

    _displayName.clear();
    emit displayNameChanged(_displayName);

    _serviceTripTerminus = nullptr;
    emit serviceTripTerminusChanged(_serviceTripTerminus);

    _globalStrings.clear();
    emit globalStringsReset(_globalStrings);

    qDeleteAllLater(_termini);
    _termini.clear();
    emit terminiReset(_termini);

    qDeleteAllLater(_busstops);
    _busstops.clear();
    emit busstopsReset(_busstops);

    qDeleteAllLater(_routes);
    _routes.clear();
    emit routesReset(_routes);

    _undoStack.clear();
}

bool HofFileRoot::isSaved() const {
    return _undoStack.isClean();
}

void HofFileRoot::setSaved(bool b) {
    _undoStack.setClean();
}

HofFileTerminus *HofFileRoot::createTerminus(const QByteArray &data) {
    HofFileTerminus *terminus = new HofFileTerminus(this);
    terminus->setData(data);
    // TODO: CONNECT SIGNALS
    return terminus;
}
