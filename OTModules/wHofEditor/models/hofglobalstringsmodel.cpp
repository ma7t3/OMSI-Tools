#include "hofglobalstringsmodel.h"

#include "../data/hoffileroot.h"

HofGlobalStringsModel::HofGlobalStringsModel(QObject *parent) : QAbstractTableModel(parent), _hofFile{nullptr} {}

QVariant HofGlobalStringsModel::headerData(int section, Qt::Orientation orientation, int role) const {
    if(role != Qt::DisplayRole)
        return QVariant();

    if(orientation == Qt::Horizontal)
        return QVariant();

    return QString::number(section + 1);
}

int HofGlobalStringsModel::rowCount(const QModelIndex &parent) const {
    if (parent.isValid())
        return 0;

    return _hofFile->globalStringsCount();
}

int HofGlobalStringsModel::columnCount(const QModelIndex &parent) const {
    if (parent.isValid())
        return 0;

    return 1;
}

QVariant HofGlobalStringsModel::data(const QModelIndex &index, int role) const {
    if (!index.isValid())
        return QVariant();

    if(role != Qt::DisplayRole && role != Qt::EditRole)
        return QVariant();

    return _hofFile->globalString(index.row());
}

bool HofGlobalStringsModel::setData(const QModelIndex &index, const QVariant &value, int role) {
    if (data(index, role) != value) {
        // FIXME: Implement me!
        return true;
    }
    return false;
}

Qt::ItemFlags HofGlobalStringsModel::flags(const QModelIndex &index) const {
    if (!index.isValid())
        return Qt::NoItemFlags;

    return QAbstractItemModel::flags(index) | Qt::ItemIsEditable;
}

HofFileRoot *HofGlobalStringsModel::hofFile() const {
    return _hofFile;
}

void HofGlobalStringsModel::setHofFile(HofFileRoot *newHofFile) {
    beginResetModel();
    if(_hofFile)
        _hofFile->disconnect(this);

    _hofFile = newHofFile;
    if(_hofFile) {
        connect(_hofFile, &HofFileRoot::globalStringsReset, this, [this](){
            beginResetModel();
            endResetModel();
        });
    }

    endResetModel();
}
