import QtQuick 2.15
import QtQuick.Window 2.15


Window {
    width: 640
    height: 640
    visible: true
    title: qsTr("Abalone")
    color: "teal"
    onWidthChanged: BoardModel.updateSizeBoard(boardWindow.width, boardWindow.height)
    onHeightChanged: BoardModel.updateSizeBoard(boardWindow.width, boardWindow.height)

    Rectangle{
        id: menu
        anchors.top: parent.top
        width: parent.width
        height:  Math.min(parent.width * 0.2, 100)
        color: "#CDCDC0"
        border.width: 3
        border.color: "teal"
        Rectangle{
            id: rect1
            width: parent.width * 0.2
            height: parent.height
            color: "transparent"
            Text{
                id: name1
                anchors.centerIn: parent
                text: "PLAYER 1"
                color: "white"
                style: Text.Outline
                styleColor: "black"
                font.weight: Font.Bold
            }
            Text{
                anchors.bottom: name1.top
                height: parent.height * 0.3
                leftPadding: parent.width * 0.3
                text: BoardModel.player === 1 ? "TURN" : ""
                color: "#CC3333"
                style: Text.Sunken
                styleColor: "black"
                font.weight: Font.ExtraBold
                font.pointSize: height ? height * 0.4 : 1
            }
            Text{
                anchors.top: name1.bottom
                height: parent.height * 0.3
                leftPadding: parent.width * 0.41
                topPadding: 0
                text: BoardModel.marblesPlayer1
                color: "teal"
                style: Text.Sunken
                styleColor: "white"
                font.weight: Font.Black
                font.pointSize: height ? height * 0.7 : 1
            }
        }
        Rectangle{
            id: rect2
            anchors.left: rect1.right
            width: parent.width * 0.6
            height: parent.height
            color: "transparent"
            Text{
                id: title
                anchors.centerIn: parent
                text: "ABALONE"
                color:"teal"
                style: Text.Outline
                styleColor: "white"
                font.weight: Font.Bold
                font.pointSize: parent.height ? parent.height*0.5 : 1
                font.letterSpacing : 3
            }
            Rectangle{
                id: buttonRestart
                anchors.top: title.bottom
                width: parent.width
                height: parent.height * 0.1
                color: "transparent"
                Text{
                    id: restart
                    anchors.centerIn: parent
                    bottomPadding: parent.height*2.5
                    text: BoardModel.isStartGame === true ? "RESTART" : ""
                    font.italic: true
                    font.weight: Font.Medium
                    MouseArea{
                        anchors.fill: parent
                        onClicked: BoardModel.draw(boardWindow.width, boardWindow.height)
                    }
                }
            }
        }
        Rectangle{
            id: rect3
            anchors.left: rect2.right
            width: parent.width * 0.2
            height: parent.height
            color: "transparent"
            Text{
                id: name2
                anchors.centerIn: parent
                text: "PLAYER 2"
                style: Text.Outline
                styleColor: "white"
                font.weight: Font.Bold
            }
            Timer {
                id: timer
                function setTimeout(cb, delayTime) {
                    timer.interval = delayTime;
                    timer.repeat = false;
                    timer.triggered.connect(cb);
                    timer.triggered.connect(function release () {
                        timer.triggered.disconnect(cb); // This is important
                        timer.triggered.disconnect(release); // This is important as well
                    });
                    timer.start();
                }
            }
            Connections {
                target: BoardModel
                function onPlayerChanged()
             {
                     if(BoardModel.player === 2){
                         timer.setTimeout(function(){ BoardModel.computerMovement(); }, 1);
                     }
             }
            }
            Text{
                anchors.bottom: name2.top
                height: parent.height * 0.3
                leftPadding: parent.width * 0.3
                text: BoardModel.player === 2 ? "TURN" : ""
                color: "#CC3333"
                style: Text.Sunken
                styleColor: "black"
                font.weight: Font.ExtraBold
                font.pointSize: height ? height * 0.4 : 1

            }
            Text{
                anchors.top: name2.bottom
                height: parent.height * 0.3
                leftPadding: parent.width * 0.41
                topPadding: 0
                text: BoardModel.marblesPlayer2
                font.weight: Font.Black
                color: "teal"
                style: Text.Sunken
                styleColor: "black"
                font.pointSize: height ? height * 0.7 : 1
            }
        }
    }
    Rectangle{
        id: boardWindow
        anchors.bottom: parent.bottom
        width: parent.width
        height: parent.height-menu.height
        color: "teal"
        radius:  width
        border.width: 2
        border.color: "white"
        Component.onCompleted: BoardModel.draw(boardWindow.width, boardWindow.height)
        Repeater {
                model: BoardModel
                Field {
                    onClicked: (x_value, y_value, box)=>{
                        BoardModel.movement(x_value, y_value, box)
                    }
                    onPositionChanged: (x_value, y_value, box)=>{
                        BoardModel.predictMovement(id_role, x_value, y_value, box)
                    }
                }
        }
    }
    Rectangle{
        id: win
        anchors.bottom: parent.bottom
        width: parent.width
        height: parent.height-menu.height
        color: "gray"
        opacity: 0.9
        visible: BoardModel.winner === 0 ? false : true
        Rectangle{
            anchors.centerIn: parent
            height: win.height*0.5
            Rectangle{
                anchors.top: parent.top
                height: parent.height*0.5
                Text{
                    id: whoWin
                    anchors.centerIn: parent
                    text: BoardModel.winner === 1 ? "Player1 win!" : "Player2 win!"
                    color: "white"
                    style: Text.Sunken
                    font.weight: Font.DemiBold
                    font.pointSize: parent.height ? parent.height*0.5 : 20
                 }
            }
            Text{
                id: playAgain
                anchors.centerIn: parent
                topPadding: whoWin.height
                text:  "PLAY AGAIN"
                style: Text.Outline
                styleColor: "#CC3333"
                font.weight: Font.Bold
                font.pointSize: whoWin.height ? whoWin.height*0.3 : 10
                MouseArea{
                    anchors.fill: parent
                    onClicked: BoardModel.draw(boardWindow.width, boardWindow.height)
                }
            }
        }
    }
}

