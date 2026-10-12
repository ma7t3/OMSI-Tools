#include "hoffileroot.h"

#include "hoffileterminus.h"

#include <QDebug>

QString HofFileParseError::errorString() const {
    return error == NoError             ? ""
         : error == UnexpectedEndOfFile ? QObject::tr("Unexpected End Of File")
         : "";
}

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

bool HofFileRoot::setData(const QByteArray &data) {
    clear();
    const QStringList lines = QString::fromLatin1(data).split("\r\n");
    for(auto it = lines.begin(); it != lines.end(); ++it) {
        const QStringList fields = (*it).split("\t");

        if(fields[0] == "[name]") {
            if(++it == lines.end()) {
                setError(HofFileParseError::UnexpectedEndOfFile, lines, it);
                return false;
            }

            setDisplayName((*it).split("\t")[0]);
            continue;
        }

        if(fields[0] == "[global_strings]") {
            if(++it == lines.end()) {
                setError(HofFileParseError::UnexpectedEndOfFile, lines, it);
                return false;
            }

            bool ok;
            const qsizetype stringCount = (*it).toInt(&ok);
            if(!ok) {
                setError(HofFileParseError::IllegalValue, lines, it);
                return false;
            }

            QStringList strings;
            for(int i = 0; i < stringCount; ++i) {
                if(++it == lines.end()) {
                    setError(HofFileParseError::UnexpectedEndOfFile, lines, it);
                    return false;
                }
                strings << (*it).split("\t")[0];
            }
            setGlobalStrings(strings);
            qInfo() << globalStrings();
        }

        // stringcount_terminus
        // stringcount_busstop

        if(fields[0] == "[addterminus_list]") {
            while(++it != lines.end() && (*it).split("\t")[0] != "[end]") {
                HofFileTerminus *t = createTerminus(*it);
                qInfo() << t->name();
            }
            if(it == lines.end()) {
                setError(HofFileParseError::UnexpectedEndOfFile, lines, it);
                return false;
            }
        }
    }

    return true;
}

QByteArray HofFileRoot::data() const {
    // TODO
    return QByteArray();
}

HofFileParseError HofFileRoot::error() const{
    return _error;
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
    _error.error = HofFileParseError::NoError;
    _error.lineNumber = 0;
    _error.columnNumber = 0;
}

bool HofFileRoot::isSaved() const {
    return _undoStack.isClean();
}

void HofFileRoot::setSaved() {
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

void HofFileRoot::setError(HofFileParseError::ErrorType type, const QStringList &list, const QStringList::const_iterator &lineIterator, int fieldIndex) {
    _error.error = type;
    _error.lineNumber = indexOfIterator(list, lineIterator) + 1;
    _error.columnNumber = 1;
    const QStringList fields = (*lineIterator).split("\t");
    for(int i = 0; i < fieldIndex && i < fields.count(); ++i) {
        _error.columnNumber += (fields[i].size() + 1);
    }
}
