#ifndef HOFFILEROOT_H
#define HOFFILEROOT_H

#include <QObject>
#include <QUndoStack>

template <typename ForwardIterator>
Q_OUTOFLINE_TEMPLATE void qDeleteAllLater(ForwardIterator begin, ForwardIterator end)
{
    while (begin != end) {
        *begin->deleteLater();
        ++begin;
    }
}

template <typename Container>
inline void qDeleteAllLater(const Container &c)
{
    qDeleteAllLater(c.begin(), c.end());
}

class HofFileTerminus;
class HofFileBusstop;
class HofFileRoute;

class HofFileRoot : public QObject {
    Q_OBJECT
public:
    explicit HofFileRoot(QObject *parent = nullptr);

    QString fileName() const;
    void setFileName(const QString &fileName);

    void setData(const QByteArray &data);
    QByteArray data() const;

    void clear();

    bool isSaved() const;
    void setSaved(bool b);

    HofFileTerminus *createTerminus(const QByteArray &data);

signals:
    void isSavedChanged(bool b);

    void fileNameChanged(const QString &fileName);
    void displayNameChanged(const QString &displayName);
    void serviceTripTerminusChanged(HofFileTerminus *terminus);
    void globalStringsReset(const QStringList &globalStrings);
    void terminiReset(const QList<HofFileTerminus *> termini);
    void busstopsReset(const QList<HofFileBusstop *> busstops);
    void routesReset(const QList<HofFileRoute *> routes);

private:
    QString _fileName;
    QUndoStack _undoStack;

    QString _displayName;
    HofFileTerminus *_serviceTripTerminus;
    QStringList _globalStrings;
    QList<HofFileTerminus *> _termini;
    QList<HofFileBusstop *> _busstops;
    QList<HofFileRoute *> _routes;
};

#endif // HOFFILEROOT_H
