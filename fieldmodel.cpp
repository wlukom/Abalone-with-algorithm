#include "fieldmodel.h"

#include <string>
#include <iostream>
#include<bits/stdc++.h>

FieldModel::FieldModel(QObject * parent): QAbstractListModel(parent)
{

 roles[IdRole] = "id_role";
 roles[SizeRole] = "size_p";
 roles[XRole] = "x_p";
 roles[YRole] = "y_p";
 roles[PlayerRole] = "player";
 roles[IsClickedRole] = "isClicked";
}

void FieldModel::draw(double width, double height){

    int columns = 9;
    int sizeObjects = std::min(width, height);
    //int sizeObjects = Math.min(boardWindow.width, boardWindow.height);
    int sizeObject = sizeObjects/(2*columns-1);
    int size_p = sizeObject*1.7;

    beginResetModel();
    mDatas.clear();
    for(int row = 0; row < 5; row++){
        for(int column = 0; column < columns-row; column++){
            double x_pom = sizeObject*row + sizeObject-size_p/2 + 2*column*sizeObject + (width-sizeObjects- sizeObject-size_p/2)/2;
            double y_pom = height/2 - sizeObject*sqrt(3)*row -size_p/2;
            double y_sec = height/2 + sizeObject*sqrt(3)*row -size_p/2;
            int id_sec = column*10 + row;

            mDatas.append(QSharedPointer<Field>(new Field(id_sec, size_p, x_pom, y_sec,0)));

            int id_pom = id_sec+row*10 - 2*row;
            if(row != 0)
                mDatas.append(QSharedPointer<Field>(new Field(id_pom, size_p, x_pom, y_pom,0)));

        }
    }

    setStartPosition();
    endResetModel();
}
QSharedPointer<Field> FieldModel::getField(int id)const{
    for(QSharedPointer<Field> field:mDatas){
        if(field->id == id)
            return field;
    }
    return nullptr;
}
void FieldModel::action(int id){
    //std::cout << "jestem w action" << std::endl;
    QSharedPointer<Field> field = getField(id);
    if(!field)
        return;

    QSharedPointer<Field> dest = getField(id-1);
    beginResetModel();
    dest->player = field->player;
    field->player = 0;
    endResetModel();
}
void FieldModel::pushClickedField(int id){

    if(ClickedFields.empty())
        ClickedFields.push_back(id);
    else{

        std::cout << " 1 " << std::endl;
        for(int i = 0; i < ClickedFields.size(); i++){
            std::cout << " field: " << ClickedFields[i] << "dla id " << id << std::endl;
            if (ClickedFields[i] == id-1 || ClickedFields[i] == id+1)
                ClickedFields.push_back(id);
            else if (ClickedFields[i] == id-10 || ClickedFields[i] == id+10)
                ClickedFields.push_back(id);
            else if (ClickedFields[i] == id-11 || ClickedFields[i] == id+11)
                ClickedFields.push_back(id);
            else if (ClickedFields[i] == id)
                ClickedFields.erase(ClickedFields.begin()+i);
            else{
                std::cout << " exit " << std::endl;
                return; };
        }
//        for (auto field = ClickedFields.begin(); field != ClickedFields.end(); ++field){

//        }
    }
    std::cout << " 3 " << std::endl;
    beginResetModel();
    QSharedPointer<Field> field = getField(id);
    if(field->isClicked)
        field->isClicked = false;
    else
        field->isClicked = true;
    std::cout << "set field->isClicked dla id: " << id << std::endl;
    endResetModel();
}
void FieldModel::setStartPosition(){
    QList<int> firstPlayer = {36,46,56,66,76,27,37,47,57,67,77,38,48,58};
    QList<int> secondPlayer = {4,14,24,34,44,3,13,23,33,43,53,22,32,42};

    for(QSharedPointer<Field> field:mDatas){
        if(firstPlayer.contains(field->id))
           field->player = 1;
        else if(secondPlayer.contains(field->id))
           field->player = 2;
        else
            field->player = 0;
    }
}
int FieldModel::rowCount(const QModelIndex& parent) const
   {
      return mDatas.size();
   }
QVariant FieldModel::data(const QModelIndex &index, int role) const
{
    if(!index.isValid()) {
        return QVariant();
    }
    if (!roles.contains(role)) {
        return QVariant();
    }

    return getRoleData(role,index.row());
}
QVariant FieldModel::getRoleData(int role, int row) const {
    QSharedPointer<Field> field = mDatas[row];
    switch(role) {
    case IdRole:
        return field->id;
    case SizeRole:
        return field->size;
    case XRole:
        return field->x;
    case YRole:
        return field->y;
    case PlayerRole:
        return field->player;
    case IsClickedRole:
        return field->isClicked;

    default:
        qWarning()<<"UserModel::getRoleData there is no role:"<<role;
    }
}
QHash<int, QByteArray> FieldModel::roleNames() const {
    return roles;
}
