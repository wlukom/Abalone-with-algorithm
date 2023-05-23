
#include <iostream>
#include <list>

#include "path.cpp"

using namespace std;

class Algorithm{

    int started_player;
    int depth;
    Board initial_board;
    QSharedPointer<Path> root_node;

    public:

        Algorithm(QList<QSharedPointer<Field>> fields, int player, int depth = 3){
            this->started_player = player;
            this->depth = depth;
            this->initial_board = Board(fields);

            root_node = QSharedPointer<Path>(new Path(depth, changePlayer(started_player)));

        }
        array<int, 2> getTheBestMovement(){
            cout << "XD" << endl;

            array<int, 2> the_best_movement;

            cout << "YYYYY" << endl;

            alfabeta(root_node, initial_board, -10, 20);

            cout << "LOL" << endl;

            //QSharedPointer<Movement> m = choosingTheBestMovement(root_node);

//            the_best_movement[0] = m->getFieldId();
//            the_best_movement[1] = m->getDirection();

//            m->print();
//            initial_board.updateMovement(m);

            return the_best_movement;

        }

        float alfabeta(QSharedPointer<Path> path, Board board, float alfa, float beta){
            if(path->getDepth() == 0){
                //board.print();
                return board.rate(started_player);
            }


            int child_player = changePlayer(path->getPlayer());
            float forwarded_value = 20;

            if(child_player == started_player)
                forwarded_value = -10;

            for(QSharedPointer<Movement> m : board.generate_movements(child_player)){

                QSharedPointer<Path> child = QSharedPointer<Path>(new Path(m, path->getDepth() - 1));
                path->addChild(child);

                Board b = Board(board.fields);
                b.updateMovement(m);

                float rate = alfabeta(child, b, alfa, beta);
                m->setRate(rate);

                if(child_player == started_player){
                    forwarded_value = max(forwarded_value, rate);
                    alfa = max(alfa, forwarded_value);
                }
                else{
                    forwarded_value = min(forwarded_value, rate);
                    beta = min(beta, forwarded_value);
                }

                if(alfa >= beta)
                    break;
            }
            return forwarded_value;

        }
        QSharedPointer<Movement> choosingTheBestMovement(QSharedPointer<Path> path){
            QList<QSharedPointer<Movement>> good_movements;
            float the_highest_rate = 0;

            for(QSharedPointer<Path> p : path->getChildren()){
                if(p->getMovement()->getRate() > the_highest_rate){
                    the_highest_rate = p->getMovement()->getRate();
                    good_movements.clear();
                    good_movements.push_back(p->getMovement());
                }
                else if(p->getMovement()->getRate() == the_highest_rate){
                    good_movements.push_back(p->getMovement());
                }
            }
            srand ( time(NULL) );
            int rand_number = rand() % good_movements.size();
//            cout << "ilosc dobrych ruchow=" << good_movements.size() << endl;
//            for(QSharedPointer<Movement> m : good_movements){
//                m->print();
//            }
//            cout << "Koneic" << endl;

            return *(next(good_movements.begin(), rand_number));
        }
        int changePlayer(int player){
            if(player == 2)
                return 1;
            return 2;
        }

};
