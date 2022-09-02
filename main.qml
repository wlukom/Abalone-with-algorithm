import QtQuick 2.15
import QtQuick.Window 2.15

Window {
    id: window
    width: 900
    height: 640
    //height: 640
    visible: true
    title: qsTr("Hello World")

    Rectangle{
        id: menu
        width: parent.width
        height: 150
        color: "white"
        anchors.top: parent.top
        border.width: 3
        Text{
        anchors.centerIn: parent
        text: "MENU"
        color:"black"
        }

    }

    Rectangle{
        id: boardWindow
        width: parent.width
        height: parent.height-menu.height
        color: "teal"
        anchors.bottom: parent.bottom


        Component.onCompleted: {
                FieldModel.draw(boardWindow.width, boardWindow.height);
            }

        Repeater {

                model: FieldModel
                Field { }


        }

    }

//    Rectangle {
//        //id: id_p
//        anchors.fill: parent
//        color: "blue"
//        ListView {
//            width: 180; height: 200

//            model:
//                UserModel
//            delegate: Text {
//                text: "id_role="+id_role
//            }
//        }

//    }



}

///trzeba zrobic zeby kulki byly odpowiednio mniejsze - miescily sie w danym miejscu
