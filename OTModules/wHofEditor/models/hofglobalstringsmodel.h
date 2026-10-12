#ifndef HOFGLOBALSTRINGSMODEL_H
#define HOFGLOBALSTRINGSMODEL_H

#include <QAbstractTableModel>

class HofFileRoot;

class HofGlobalStringsModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    explicit HofGlobalStringsModel(QObject *parent = nullptr);

    // Header:
    QVariant headerData(int section,
                        Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const override;

    // Basic functionality:
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    // Editable:
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;

    Qt::ItemFlags flags(const QModelIndex &index) const override;

    HofFileRoot *hofFile() const;
    void setHofFile(HofFileRoot *newHofFile);

private:
    HofFileRoot *_hofFile;
};

#endif // HOFGLOBALSTRINGSMODEL_H
