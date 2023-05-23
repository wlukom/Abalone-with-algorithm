#include <list>
#include <algorithm>
#include <iterator>
#include <vector>



#include "movement.cpp"

int offTheBoard[] = {5,6,7,8,15,16,17,25,26,35,45,54,55,63,64,65,72,73,74,75};

class Board{
    vector<int> directions = {10, 1, -9, -10, -1, 9};
    public:
        QList<QSharedPointer<Field>> fields;

        Board(){
            // int fieldsPlayer1[] = {27,36,37,38,46,47,48,56,57,58,66,67,76,77};
            // int fieldsPlayer2[] = {3,4,13,14,22,23,24,32,33,34,42,43,44,53};
            int fieldsPlayer1[] = {27,36,37,38,46,47,48,56,57,58,66,67,76,77};
            int fieldsPlayer2[] = {3,4,13,14,22,23,24,32,33,34,42,43,44,53};
            // int fieldsPlayer1[] = {27,36,37,38,46,47,48,49,56,57,58,66,67,77};
            // int fieldsPlayer2[] = {3,4,13,14,22,23,24,31,32,33,42,43,44,53};

            int offTheBoardSize = sizeof(offTheBoard)/sizeof(offTheBoard[0]);
            int fieldsPlayer1Size = sizeof(fieldsPlayer1)/sizeof(fieldsPlayer1[0]);
            int fieldsPlayer2Size = sizeof(fieldsPlayer2)/sizeof(fieldsPlayer2[0]);

            int x = 0;
            for(int i = 0; i < 61; i++, x++){
                while(contains(x, offTheBoard, offTheBoardSize))
                        x++;

                if(contains(x, fieldsPlayer1, fieldsPlayer1Size))
                    this->fields.push_back(QSharedPointer<Field>(new Field(x,1)));
                else if(contains(x, fieldsPlayer2, fieldsPlayer2Size))
                    this->fields.push_back(QSharedPointer<Field>(new Field(x,2)));
                else
                    this->fields.push_back(QSharedPointer<Field>(new Field(x,0)));
            }
        }

        Board(QList<QSharedPointer<Field>> input_fields){
            for(QSharedPointer<Field> f : input_fields){
                this->fields.push_back(QSharedPointer<Field>(new Field(f->id, f->player)));
            }
        }

        void updateMovement(QSharedPointer<Movement> movement){
            // this is from first Path in the tree
            if(movement->getDirection() == 0){
                return;
            }
            int current_player = movement->getPlayer();
            int opponent = oppositePlayer(current_player);

            QSharedPointer<Field> firstField = findField(movement->getFieldId());
            QSharedPointer<Field> secField = findField(firstField->id + movement->getDirection());

            firstField->player = 0;

            while(!isNULL(secField) && secField->player == current_player){
                firstField = secField;
                secField = findField(firstField->id + movement->getDirection());
            }

            // next field NULL
            if(isNULL(secField))
                return;

            // next field empty
            if(secField->player == 0){
                secField->player = current_player;
                return;
            }

            firstField = secField;
            firstField->player = current_player;

            secField = findField(firstField->id + movement->getDirection());

            while(!isNULL(secField) && secField->player == opponent){
                firstField = secField;
                secField = findField(firstField->id + movement->getDirection());
            }

            // next field NULL
            if(isNULL(secField))
                return;

            // next field empty
            if(secField->player == 0)
                secField->player = opponent;

            return;
        }

        float rate(int player){

            float score = 5;

            // NUMBER OF MARBLES
            score += getCountMarbles(player) * 1.0;
            score -= getCountMarbles(oppositePlayer(player)) * 1.0;

            //cout << "\t MARBLES BALANCE=" << score << endl;



            // DISTANCE FROM THE CENTER
            int d = summary_distance_from_center(player);

            score -= d * 0.01;
            // cout << "\t DISTANCE=" << d << endl;
            // cout << "\tAFTER DISTANCE=" << score << endl;

            // // GROUPING
            int grouping = marbles_neighborhood_rate(player);
            //cout << "\t GROUPING=" << grouping * 0.005 << endl;
            score += grouping * 0.005;

            // Attacking by group

            return score;

        }

        int getCountMarbles(int player){
            int points = 0;
            for(QSharedPointer<Field> marble : fields){
                if(marble->player == player){
                    points++;
                }
            }
            return points;
        }

        int marbles_neighborhood_rate(int player){

            vector<int> marbles_ids = get_marbles_ids(player);
            int count_of_contacts = 0;

            for(int marble_id : marbles_ids){
                for(int neighbor_id : get_neighbors_ids(marble_id)){
                    if(list_contains_element(marbles_ids, neighbor_id))
                        count_of_contacts++;
                }
            }
            return count_of_contacts;
        }

        QSharedPointer<Field> getField(int id){
            for(QSharedPointer<Field> marble : fields){
                if(marble->id == id)
                    return marble;
            }
            return nullptr;
        }

        int get_marbles_balance(int player){
            int player_marbles = 0;
            int opponent_marbles = 0;
            for(QSharedPointer<Field> marble : fields){
                if(marble->player == player)
                    player_marbles++;
                else if(marble->player != 0)
                    opponent_marbles++;
            }
            //cout << "me=" << player_marbles << " and my bro=" << opponent_marbles << endl;
            return player_marbles - opponent_marbles;
        }


        int summary_distance_from_center(int player){
            int const center = 40;
            int const max_distance = 4;
            int summary_distance = 0;
            vector<int> marbles_ids = get_marbles_ids(player);

            for(int distance = 1; distance <= max_distance; distance++){
                vector<int> neighbors_ids = get_neighbors_ids(center, distance);
                for(int marble_id : marbles_ids){
                    if(list_contains_element(neighbors_ids, marble_id)){
                        //cout << "dla marble=" << marble_id << " distance=" << distance << endl;
                        summary_distance += distance;
                    }
                }
            }
            return summary_distance;
        }
        bool list_contains_element(vector<int> l, int element) const{
            if(find(begin(l), end(l), element) != end(l))
                return true;
            return false;
        }
        vector<int> get_neighbors_ids(int field_id, int distance = 1){
            vector<int> neighbors_ids;
            vector<int> basic_neighbors_ids;
            for(int dir : this->directions){
                int neighbor_id = field_id + (dir * distance);
                basic_neighbors_ids.push_back(neighbor_id);

            }

            for(int i = 0; i < this->directions.size(); i++){
                int begin_neighbor_id = basic_neighbors_ids.at(i);
                int end_neighbor_id = basic_neighbors_ids.at((i+1) % directions.size());

                int direction = directions.at((i+2)% directions.size());

                for(
                    int neighbor_id = begin_neighbor_id + direction;
                    end_neighbor_id != neighbor_id;
                    neighbor_id += direction)
                    {
                    neighbors_ids.push_back(neighbor_id);
                }
            }
            neighbors_ids.insert(neighbors_ids.end(), basic_neighbors_ids.begin(), basic_neighbors_ids.end());
            return neighbors_ids;
        }
        vector<int> get_marbles_ids(int player){
            vector<int> marbles_ids;
            for(QSharedPointer<Field> marble : fields){
                if(marble->player == player)
                    marbles_ids.push_back(marble->id);
            }
            return marbles_ids;
        }



        QList<QSharedPointer<Movement>> generate_movements(int currentPlayer){ //player 1,2

            QList<QSharedPointer<Movement>> movements;
            for(int direction: {-1, 1, -10, 10, -9, 9}){

                for(QSharedPointer<Movement> m : possibleMovesInLine(direction, currentPlayer)){
                    //m->print();
                    movements.push_back(m);
                }
            }
            return movements;
        }

        QList<QSharedPointer<Movement>> possibleMovesInLine(int direction, int player){
            QList<QSharedPointer<Movement>> m;

            for(QSharedPointer<Field> firstField : this->fields){


                if(firstField->player == player){
                    //cout << "TO JEST ID=" << firstField.id << endl;
                    QSharedPointer<Field> secField = findField(firstField->id + direction);
                    // 1 + NULL
                    if(isNULL(secField))
                        continue;
                    // 1 + 0
                    if(secField->player == 0){
                        m.push_back(QSharedPointer<Movement>(new Movement(firstField, direction)));
                        continue;
                    }
                    // 2
                    else if(secField->player == player){
                        //cout << " 2 ";
                        QSharedPointer<Field> thirdField = findField(secField->id + direction);
                        // 2 + NULL
                        if(isNULL(thirdField))
                            continue;
                        // 2 + 0
                        else if(thirdField->player == 0){
                            //cout << " 2+0 ";
                            m.push_back(QSharedPointer<Movement>(new Movement(firstField, direction)));
                            //cout << " 2+0 end";
                            continue;
                        }
                        QSharedPointer<Field> firstOut = findField(thirdField->id + direction);
                        // 2 + 1
                        if(thirdField->player != player){
                            // 2 + 1 + NULL
                            if(isNULL(firstOut))
                                m.push_back(QSharedPointer<Movement>(new Movement(firstField, direction)));
                            // 2 + 1 + 0
                            else if(firstOut->player == 0)
                                m.push_back(QSharedPointer<Movement>(new Movement(firstField, direction)));
                            // 2 + 1 + 1 || 2 + 2
                            else
                                continue;
                        }
                        // 3
                        else{
                            // 3 + NULL
                            if(isNULL(firstOut))
                                continue;
                            // 3 + 0
                            else if(firstOut->player == 0)
                                m.push_back(QSharedPointer<Movement>(new Movement(firstField, direction)));
                            // 4  -wrong
                            else if(firstOut->player == player)
                                continue;
                            // 3 + 1 opponent
                            else{
                                QSharedPointer<Field> secOut = findField(firstOut->id + direction);
                                // 3 + 1 + NULL
                                if(isNULL(secOut))
                                    m.push_back(QSharedPointer<Movement>(new Movement(firstField, direction)));
                                // 3 + 1 + 1  -wrong
                                else if(secOut->player == player)
                                    continue;
                                // 3 + 1 + 0
                                else if(secOut->player == 0)
                                    m.push_back(QSharedPointer<Movement>(new Movement(firstField, direction)));

                                // 3 + 2 opponent
                                else{
                                    QSharedPointer<Field> thirdOut = findField(secOut->id + direction);
                                    // 3 + 2 + NULL
                                    if(isNULL(thirdOut))
                                        m.push_back(QSharedPointer<Movement>(new Movement(firstField, direction)));
                                    // 3 + 2 + 0
                                    else if(thirdOut->player == 0)
                                        m.push_back(QSharedPointer<Movement>(new Movement(firstField, direction)));
                                    // 3 + 3 || 3 + 2 + 1
                                    else
                                        continue;
                                }
                            }
                        }
                    }
                }
            }

            return m;
        }


        bool contains(int x, int tab[], int tabSize){
            for(int i = 0; i < tabSize; i++){
                if(x == tab[i])
                    return true;
            }
            return false;
        }

        bool isOffTheBoard(int x){
            if(x < 0 || x > 80)
                return false;
            for(int i : offTheBoard){
                if(i == x)
                    return false;
            }
            return true;
        }


        QSharedPointer<Field> findField(int id){
            for(QSharedPointer<Field> field : this->fields){
                if(field->id == id)
                    return field;
            }
            return QSharedPointer<Field>(new Field(-1));
        }

        bool isNULL(QSharedPointer<Field> object){

            if(object->id == -1)
                return true;
            return false;
        }

        int oppositePlayer(int p){
            if(p == 1)
                return 2;
            if(p == 2)
                return 1;
            return 0;
        }

};
