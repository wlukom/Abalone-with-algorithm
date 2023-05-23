#include "boardmodel.h"
#include "algorithm.cpp"

#include <cstdlib>
#include <string>
#include <iostream>
#include <vector>
#include <cmath>
#include <bits/stdc++.h>
#include <map>

using namespace std;

BoardModel::BoardModel(QObject * parent): QAbstractListModel(parent)
{
 roles[IdRole] = "id_role";
 roles[WidthRole] = "width_p";
 roles[HeightRole] = "height_p";
 roles[XRole] = "x_p";
 roles[YRole] = "y_p";
 roles[PlayerRole] = "player";
 roles[ArrowRole] = "arrow";
}
void BoardModel::draw(double width, double height){
    if(height < width){
        rectangleHeight = height/(columns+1);
        rectangleWidth = 2*rectangleHeight/sqrt(3);
    }
    else{ //width <= height
        rectangleWidth = width/(columns+1);
        rectangleHeight = 0.5*rectangleWidth*sqrt(3);
    }
    beginResetModel();
    datas.clear();
    for(int row = 0; row < 5; row++){
        for(int column = 0; column < columns-row; column++){
            double x_pom = rectangleWidth*column + (width - rectangleWidth*(columns-row))/2;
            double y_pom = height/2 - rectangleHeight/2 - rectangleHeight*row;
            double y_sec = height/2 - rectangleHeight/2 + rectangleHeight*row;
            int id_sec = column*10 + row;

            datas.append(QSharedPointer<Field>(new Field(id_sec, rectangleWidth, rectangleHeight, x_pom, y_sec)));

            int id_pom = id_sec+row*10 - 2*row;
            if(row != 0)
                datas.append(QSharedPointer<Field>(new Field(id_pom, rectangleWidth, rectangleHeight, x_pom, y_pom)));
        }
    }
    setStartPosition();
    endResetModel();
}
void BoardModel::updateSizeBoard(double width, double height){

    QList<QSharedPointer<Field>> pomDatas = datas;
    if(height < width){
        rectangleHeight = height/(columns+1);
        rectangleWidth = 2*rectangleHeight/sqrt(3);
    }
    else{ //width <= height
        rectangleWidth = width/(columns+1);
        rectangleHeight = 0.5*rectangleWidth*sqrt(3);
    }
        beginResetModel();
        datas.clear();
        for(int row = 0; row < 5; row++){
            for(int column = 0; column < columns-row; column++){
                double x_pom = rectangleWidth*column + (width - rectangleWidth*(columns-row))/2;
                double y_pom = height/2 - rectangleHeight/2 - rectangleHeight*row;
                double y_sec = height/2 - rectangleHeight/2 + rectangleHeight*row;
                int id_sec = column*10 + row;

                QSharedPointer<Field> f = getField(id_sec, pomDatas);
                datas.append(QSharedPointer<Field>(new Field(id_sec, rectangleWidth, rectangleHeight, x_pom, y_sec, f->player, f->arrow)));

                int id_pom = id_sec+row*10 - 2*row;
                if(row != 0){
                    QSharedPointer<Field> f = getField(id_pom, pomDatas);
                    datas.append(QSharedPointer<Field>(new Field(id_pom, rectangleWidth, rectangleHeight, x_pom, y_pom, f->player, f->arrow)));
                }
            }
        }
    endResetModel();
}
void BoardModel::movement(double x, double y, int box){

    if(arrowedFields.size() == 0)
        return;

    int direction = getRectanglePart(x, y, box);

    beginResetModel();
    // clear prev fields
    for(int i = 0; i < arrowedFields.size(); i++){
        QSharedPointer<Field> f = getField(arrowedFields[i]);
        f->player = 0;
    }
    //move player's marbles
    for(int i = 0; i < targetedFields.size(); i++){
        if(existsID(targetedFields[i]))
             getField(targetedFields[i])->player = m_player;
    }
    // other's player marbles
    int otherPlayer = 1;
    if(m_player == 1)
        otherPlayer = 2;
    for(int i = 0; i < otherPlayerPushedFields.size(); i++){
       QSharedPointer<Field> f = getField(otherPlayerPushedFields[i]);
       if(existsID(otherPlayerPushedFields[i] + partDirections.find(direction)->diffValue))
           getField(otherPlayerPushedFields[i] + partDirections.find(direction)->diffValue)->player = otherPlayer;
    }
    endResetModel();

    resetArrows();
    checkPlayersMarbles();
    changePlayer();

    if(!isStartGame){
        isStartGame = true;
        emit checkIsStartGame();
    }

    computerMovement();
}
void BoardModel::computerMovement(){

//    cout << "XDD" << endl;
    Board b = Board(datas);
    Algorithm a = Algorithm(b.fields, 2);
    a.getTheBestMovement();

}

void BoardModel::predictMovement(int fieldID, double x, double y, int box){
    resetArrows();
    QSharedPointer<Field> hoveredField = getField(fieldID);
    if(hoveredField->player != m_player)
        return;

    int part = 0;
    part = getRectanglePart(x,y, box);
    if(part == 0)
        return;

    beginResetModel();

    int arrow = 0;
    arrow = getArrowDirection(fieldID, part);
    if(arrow == 0){
        resetArrows();
        return;
    }
    for(vector<int>::iterator f=arrowedFields.begin(); f != arrowedFields.end(); f++){
        QSharedPointer<Field> field = getField(*f);
        field->arrow = partDirections.find(arrow)->arrow;
    }
    endResetModel();
}

int BoardModel::getArrowDirection(int fieldID, int part){
    if(part == 0)
        return 0;
    else if(part <= 6)
        return directionInLine(fieldID, part);
    else if(part <= 12)
        return directionFromCorner(fieldID, part);
    else if(part <= 30)
        return direction2Marbles(fieldID, part);
    else return 0;
}
int BoardModel::direction2Marbles(int fieldID, int part){
    //direction two marbles
    int secID = fieldID + partDirections.find(part)->inLine;
    int diff = partDirections.find(part)->diffValue;

    arrowedFields.push_back(fieldID);
    arrowedFields.push_back(secID);

    if(existsID(secID)){
        QSharedPointer<Field> secField = getField(secID);
        if(secField->player != m_player)
            return 0;
        for(int i = 0; i < 2; i++){
            if(existsID(arrowedFields[i] + diff)){
                QSharedPointer<Field> nextField = getField(arrowedFields[i] + diff);
                if(nextField->player != 0)
                    return 0;
            }
        }
    }
    else return 0;
    arrowedFields.push_back(fieldID);
    arrowedFields.push_back(secID);
    for(int i = 0; i < 2; i++)
        targetedFields.push_back(arrowedFields[i] + partDirections.find(part)->diffValue);
    return part;
}

int BoardModel::directionFromCorner(int fieldID, int part){
    // direction three marbles
    bool canWeMove = 0;
    canWeMove = canWeMoveFromCorner(fieldID, directionsFromCorners.find(part).value().first);
    if(canWeMove)
        return directionsFromCorners.find(part).value().first;
    canWeMove = canWeMoveFromCorner(fieldID, directionsFromCorners.find(part).value().second);
    if(canWeMove)
        return directionsFromCorners.find(part).value().second;
    return 0;
}
bool BoardModel::canWeMoveFromCorner(int fieldID, int direction){
    arrowedFields.clear();
    targetedFields.clear();

    int targetDifference = partDirections.find(direction)->diffValue;
    int inLineDifference = partDirections.find(direction)->inLine;

    int fields[] = {fieldID + inLineDifference, fieldID, fieldID - inLineDifference};

    // are three marbles inline?
    for (int i = 0; i < 3; i++){
        // are exists
        if(existsID(fields[i])){
            QSharedPointer<Field> f = getField(fields[i]);
            if(f->player == m_player)
                arrowedFields.push_back(fields[i]);
            else return 0;
        }
        else return 0;
        // if exists field where we want to move
        if(existsID(fields[i] + targetDifference)){
            QSharedPointer<Field> field = getField(fields[i] + targetDifference);
            if(field->player == 0)
                targetedFields.push_back(field->id);
            else return 0;
        }
    }
    return true;
}

int BoardModel::directionInLine(int fieldID, int part){
    //direction marbles inline
    vector<int> line = getFieldsInLine(fieldID, part);
    vector<int>::iterator it;
    QSharedPointer<Field> currentField = getField(fieldID);

    arrowedFields.push_back(fieldID);

    for(vector<int>::iterator f=++line.begin(); f != line.end(); f++){
        currentField = getField(*f);
        it = f;
        QSharedPointer<Field> field = getField(*f);
        if(field->player == m_player)
            arrowedFields.push_back(field->id);
        else break;
    }
    // more than 3 fields
    if(arrowedFields.size() > 3)
        return 0;
    // is the end of the board
    if(currentField->player == m_player){
        if(arrowedFields.size() > 1){
            targetedFields.push_back(arrowedFields[1]);
            if(arrowedFields.size() == 3)
                targetedFields.push_back(arrowedFields[2]);
        }
        return part;
    }
    // if is ground field
    if(currentField->player == 0){
        targetedFields.push_back(currentField->id);
        if(arrowedFields.size() > 1){
            targetedFields.push_back(arrowedFields[1]);
            if(arrowedFields.size() == 3)
                targetedFields.push_back(arrowedFields[2]);
        }
        return part;
    }
    // if is opponent's marble
    for(vector<int>::iterator f=it; f != line.end(); f++){
        currentField = getField(*f);
        it = f;
        QSharedPointer<Field> field = currentField;
        if(field->player != m_player && field->player != 0)
            otherPlayerPushedFields.push_back(*f);
        else break;
    }
    //attacker's marble behind the opponent
    if(currentField->player == m_player)
        return 0;
    // ground field or end of the board after the opponent
    else if(otherPlayerPushedFields.size() < 3 && otherPlayerPushedFields.size() < arrowedFields.size()){
        for (auto field : arrowedFields)
            targetedFields.push_back(field + partDirections.find(part)->inLine);
        return part;
    }
    else return 0;
}

vector<int> BoardModel::getFieldsInLine(int fieldID, int part){
    vector<int> fields;
    int dif = partDirections.find(part)->inLine;
    while(existsID(fieldID)){
        fields.push_back(fieldID);
        fieldID += dif;
    }
    return fields;
}
int BoardModel::getRectanglePart(double x, double y, int box)const{
    switch(box){
    case 1:
        return getLeftBoxPart(x, y);
    case 2:
        return getMiddleBoxPart(x, y);
    case 3:
        return getRightBoxPart(x, y);
    }
    return 0;
}
int BoardModel::getLeftBoxPart(double x, double y)const{
    double h = rectangleHeight;
    double a = 0.2*rectangleWidth;
    int part = 0;

    if(y < 0.25*h*(a-x)/a)
        part = 30;
    else if(y < 0.25*h*(a+x)/a)
        part = 29;
    else if(y < 0.5*h)
        part = 28;
    else if(y < 0.25*h*(3*a-x)/a)
        part = 27;
    else if(y < 0.25*h*(3*a+x)/a)
        part = 26;
    else
        part = 25;
    return part;
}
int BoardModel::getRightBoxPart(double x, double y)const{
    double h = rectangleHeight;
    double a = 0.2*rectangleWidth;
    int part = 0;

    if(y < 0.25*h*x/a)
        part = 19;
    else if(y < (0.5*h*a - 0.25*h*x)/a)
        part = 20;
    else if(y < 0.5*h)
        part = 21;
    else if(y < 0.25*h*(2*a+x)/a)
        part = 22;
    else if(y < 0.25*h*(4*a-x)/a)
        part = 23;
    else
        part = 24;
    return part;
}

int BoardModel::getMiddleBoxPart(double x, double y)const{
    int part = 0;
    double r = rectangleWidth*0.6/2;
    double h = rectangleHeight;
    double m = (h - 2*r)/2;
    double l = sqrt(pow(x-r,2)+pow(y-(r+m),2));

    //  inside circle
    if(l < r){
        if(x > r){ // right side
            if(abs(0.5*h-y) < 0.08*h && l < 0.7*r)
                    part = 2;
            else if(abs(0.5*h-y) < 0.08*h && l > 0.7*r)
                    part = 8;
            else if(y < 0.5*h)
                part = 1;
            else
                 part = 3;
        }
        else{ // left side
            if(abs(0.5*h-y) < 0.08*h && l < 0.7*r)
                    part = 5;
            else if(abs(0.5*h-y) < 0.08*h && l > 0.7*r)
                    part = 11;
            else if(y < 0.5*h)
                part = 6;
            else
                 part = 4;
        }
    }
    else{ // outside circle
        if(x > r){ // right side
            if(y < 0.5*h)
                part = 7;
            else
                part = 9;
        }
        else{ // left side
            if(y > 0.5*h)
                part = 10;
            else
                part = 12;
        }
    }
    return part;
}
QSharedPointer<Field> BoardModel::getField(int id)const{
    for(QSharedPointer<Field> field:datas){
        if(field->id == id)
            return field;
    }
    return nullptr;
}
QSharedPointer<Field> BoardModel::getField(int fieldID, QList<QSharedPointer<Field>> myList)const{
    for(QSharedPointer<Field> field:myList){
        if(field->id == fieldID)
            return field;
    }
    return nullptr;
}
void BoardModel::checkPlayersMarbles(){
    int player1 = 0;
    int player2 = 0;
    for(QSharedPointer<Field> field:datas){
        if(field->player == 1)
            player1++;
        else if(field->player == 2)
            player2++;
    }
    marblesPlayer1 = player1-8;
    emit checkMarblesPlayer1();
    marblesPlayer2 = player2-8;
    emit checkMarblesPlayer2();

    // is win
    if(marblesPlayer1 < 1)
        winner = 2;
    if(marblesPlayer2 < 1)
        winner = 1;
    emit getWinner(winner);
}

void BoardModel::resetArrows(){
    beginResetModel();
    for(int i = 0; i < arrowedFields.size(); i++){
        if(existsID(arrowedFields[i])){
            QSharedPointer<Field> f = getField(arrowedFields[i]);
            f->arrow = -1;
        }
    }
    endResetModel();
    targetedFields.clear();
    arrowedFields.clear();
    otherPlayerPushedFields.clear();
}
bool BoardModel::existsID(int id){
    for(QSharedPointer<Field> field:datas){
        if(field->id == id)
            return true;
    }
    return false;
}
void BoardModel::setStartPosition(){
    setPartDirections();
    setDirectionFromCorners();

    QList<int> firstPlayer = {36,46,56,66,76,27,37,47,57,67,77,38,48,58};
    QList<int> secondPlayer = {4,14,24,34,44,3,13,23,33,43,53,22,32,42};


    beginResetModel();
    for(QSharedPointer<Field> field:datas){
        if(firstPlayer.contains(field->id))
           field->player = 1;
        else if(secondPlayer.contains(field->id))
           field->player = 2;
        else
            field->player = 0;
    }
    endResetModel();
    checkPlayersMarbles();
    m_player = 1;
    emit playerChanged();
    isStartGame = false;
    emit checkIsStartGame();
    winner = 0;
    emit getWinner(winner);

}
void BoardModel::setPartDirections(){
    partDirections.insert(1, fieldDirections(125, -9, -9));
    partDirections.insert(2, fieldDirections(180, -10, -10));
    partDirections.insert(3, fieldDirections(-125, -1, -1));
    partDirections.insert(4, fieldDirections(-55, 9, 9));
    partDirections.insert(5, fieldDirections(0, 10, 10));
    partDirections.insert(6, fieldDirections(55, 1, 1));
    partDirections.insert(7, fieldDirections(125, 1, -9));
    partDirections.insert(8, fieldDirections(125, 10, -9));
    partDirections.insert(9, fieldDirections(180, 9, -10));
    partDirections.insert(10, fieldDirections(180, 1, -10));
    partDirections.insert(11, fieldDirections(-125, 10, -1));
    partDirections.insert(12, fieldDirections(-125, 9, -1));
    partDirections.insert(13, fieldDirections(-55, 1, 9));
    partDirections.insert(14, fieldDirections(-55, 10, 9));
    partDirections.insert(15, fieldDirections(0, 1, 10));
    partDirections.insert(16, fieldDirections(0, 9, 10));
    partDirections.insert(17, fieldDirections(55, 10, 1));
    partDirections.insert(18, fieldDirections(55, 9, 1));
    partDirections.insert(19, fieldDirections(180, 9, -10));
    partDirections.insert(20, fieldDirections(55, 10, 1));
    partDirections.insert(21, fieldDirections(-125, 9, -1));
    partDirections.insert(22, fieldDirections(125, 1, -9));
    partDirections.insert(23, fieldDirections(-55, 10, 9));
    partDirections.insert(24, fieldDirections(180, 1, -10));
    partDirections.insert(25, fieldDirections(0, -9, 10));
    partDirections.insert(26, fieldDirections(-125, -10, -1));
    partDirections.insert(27, fieldDirections(55, -9, 1));
    partDirections.insert(28, fieldDirections(-55, -1, 9));
    partDirections.insert(29, fieldDirections(125, -10, -9));
    partDirections.insert(30, fieldDirections(0, -1, 10));
}
void BoardModel::setDirectionFromCorners(){
   directionsFromCorners.insert(7,make_pair(7,8));
   directionsFromCorners.insert(8,make_pair(9,10));
   directionsFromCorners.insert(9,make_pair(11,12));
   directionsFromCorners.insert(10,make_pair(13,14));
   directionsFromCorners.insert(11,make_pair(15,16));
   directionsFromCorners.insert(12,make_pair(17,18));
}

void BoardModel::changePlayer(){
    if(m_player == 1)
        m_player = 2;
    else
        m_player = 1;

    emit playerChanged();
}
int BoardModel::rowCount(const QModelIndex& parent) const{

    return datas.size();
}
QVariant BoardModel::data(const QModelIndex &index, int role) const{
    if(!index.isValid())
        return QVariant();
    if (!roles.contains(role))
        return QVariant();

    return getRoleData(role,index.row());
}
QVariant BoardModel::getRoleData(int role, int row) const {
    QSharedPointer<Field> field = datas[row];
    switch(role) {
    case IdRole:
        return field->id;
    case WidthRole:
        return field->width;
    case HeightRole:
        return field->height;
    case XRole:
        return field->x;
    case YRole:
        return field->y;
    case PlayerRole:
        return field->player;
    case ArrowRole:
        return field->arrow;
    default:
        qWarning()<<"getRoleData there is no role:"<<role;
    }
}
QHash<int, QByteArray> BoardModel::roleNames() const {
    return roles;
}


