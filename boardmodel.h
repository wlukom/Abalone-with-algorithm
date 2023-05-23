#ifndef BOARDMODEL_H
#define BOARDMODEL_H


#include <QAbstractListModel>

using namespace std;

class Field
{
 public :
    int id;
    double width;
    double height;
    double x;
    double y;
    int player;
    int arrow;
    Field();
    Field(int id, int player = 0): id(id), player(player) {  };
    Field(int id, double width, double height, double x, double y, int player = 0, int arrow = -1):id(id), width(width), height(height), x(x), y(y), player(player), arrow(arrow){};
};
struct fieldDirections
{
    int arrow;
    int inLine;
    int diffValue;
    fieldDirections(int a, int b, int c): arrow(a), inLine(b), diffValue(c){};
};
class BoardModel : public QAbstractListModel
{
  Q_OBJECT
   enum ItemRoles {
        IdRole = Qt::UserRole + 1,
        WidthRole = Qt::UserRole + 2,
        HeightRole = Qt::UserRole + 3,
        XRole = Qt::UserRole + 4,
        YRole = Qt::UserRole + 5,
        PlayerRole = Qt::UserRole + 6,
        ArrowRole = Qt::UserRole + 7,
    };
 public :
    Q_PROPERTY(int player MEMBER m_player NOTIFY playerChanged)
    Q_PROPERTY(bool isStartGame MEMBER isStartGame NOTIFY checkIsStartGame)
    Q_PROPERTY(int marblesPlayer1 MEMBER marblesPlayer1 NOTIFY checkMarblesPlayer1)
    Q_PROPERTY(int marblesPlayer2 MEMBER marblesPlayer2 NOTIFY checkMarblesPlayer2)
    Q_PROPERTY(int winner MEMBER winner NOTIFY getWinner)
    Q_INVOKABLE void draw(double width, double height);
    Q_INVOKABLE void updateSizeBoard(double width, double height);
    Q_INVOKABLE void predictMovement(int fieldID, double x, double y, int box);
    Q_INVOKABLE void movement(double x, double y, int box);
    Q_INVOKABLE void computerMovement();
    virtual int rowCount(const QModelIndex& parent = QModelIndex()) const;
    virtual QVariant data(const QModelIndex &index, int role) const;
    QHash<int, QByteArray> roleNames() const;
    BoardModel(QObject * parent = 0);
 private:
   int columns = 9;
   double rectangleWidth = 1;
   double rectangleHeight = 1;
   int marblesPlayer1 = 6;
   int marblesPlayer2 = 6;
   int m_player;
   int winner = 0;
   bool isStartGame = false;
   vector<int> arrowedFields;
   vector<int> otherPlayerPushedFields;
   vector<int> targetedFields;

   int getRectanglePart(double x, double y, int box)const;
   int getCirclePart(double x, double y)const;
   int getMiddleBoxPart(double x, double y)const;
   int getRightBoxPart(double x, double y)const;
   int getLeftBoxPart(double x, double y)const;
   int getArrowDirection(int fieldID, int part);
   int directionInLine(int fieldID, int part);
   int direction2Marbles(int fieldID, int part);
   int directionFromCorner(int fieldID, int part);
   bool canWeMoveFromCorner(int fieldID, int direction);
   bool existsID(int id);
   void checkPlayersMarbles();
   void changePlayer();
   void resetArrows();
   void setStartPosition();



   QMap<int, fieldDirections> partDirections;
   QMap<int, pair<int,int>> directionsFromCorners;
   void setPartDirections();
   void setDirectionFromCorners();

   QList<QSharedPointer<Field>> datas;
   QSharedPointer<Field> getField(int id)const;
   QSharedPointer<Field> getField(int fieldID, QList<QSharedPointer<Field>> myList)const;
   vector<int> getFieldsInLine(int fieldID, int part);
   QHash<int, QByteArray> roles;
   QVariant getRoleData(int role, int row) const;

signals:
   void playerChanged();
   void checkMarblesPlayer1();
   void checkMarblesPlayer2();
   void checkIsStartGame();
   void getWinner(int winner);
};

#endif








