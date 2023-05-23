#ifndef FIELDMODEL_H
#define FIELDMODEL_H

#include <QAbstractListModel>

class Field
{
 public :
    int id;
    double size;
    double x;
    double y;
    int player;
    bool isClicked = false;
    Field();
    Field(int id, double size, double x, double y, int player):id(id), size(size), x(x), y(y), player(player){};
    Field(const QString& firstname, const QString& lastname):mFirstname(firstname), mLastName(lastname){};
    QString mFirstname;

 private:
    QString mLastName;
};
class FieldModel : public QAbstractListModel
{
  Q_OBJECT
   enum ItemRoles {
        IdRole = Qt::UserRole + 1,
       SizeRole = Qt::UserRole + 2,
       XRole = Qt::UserRole + 3,
       YRole = Qt::UserRole + 4,
       PlayerRole = Qt::UserRole + 5,
       IsClickedRole = Qt::UserRole + 6,
    };
 public :

   QSharedPointer<Field> getField(int id)const;
    void setStartPosition();
    FieldModel(QObject * parent = 0);
    virtual int rowCount(const QModelIndex& parent = QModelIndex()) const;
    virtual QVariant data(const QModelIndex &index, int role) const;
    Q_INVOKABLE void draw(double width, double height);
    Q_INVOKABLE void action(int id);
    Q_INVOKABLE void pushClickedField(int id);
    QHash<int, QByteArray> roleNames() const;

 private:
   QList<QSharedPointer<Field>> mDatas;
   QHash<int, QByteArray> roles;
   QVariant getRoleData(int role, int row) const;
   std::vector<int> ClickedFields;
};

#endif // FIELDMODEL_H
