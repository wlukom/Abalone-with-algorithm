import QtQuick 2.15


Rectangle{
    signal positionChanged(double x_value, double y_value, int box)
    signal clicked(double x_value, double y_value, int box)
    id: rectangle
    x: x_p
    y: y_p
    width: width_p
    height: height_p
    color: "transparent"
    //border.width: 2

    Rectangle{
        id: left_box
        width: parent.width * 0.2
        height: parent.height
        color: "transparent"
        MouseArea{
            anchors.fill: parent
            hoverEnabled: true
            onPositionChanged: rectangle.positionChanged(mouseX, mouseY, 1)
            onClicked: rectangle.clicked(mouseX, mouseY, 1)
            enabled: BoardModel.winner !== 0 ? false : true
        }
    }
    Rectangle{
        id: middle_box
        anchors.left: left_box.right
        width: parent.width * 0.6
        height: parent.height
        color: "transparent"
        Rectangle{

            id: circle
            anchors.centerIn: parent
            width: parent.width
            height: parent.width
            color: player === 1 ? "white" :  player === 2 ? "black" : "transparent"
            radius: width / 2
            border.width: 2
            Rectangle{
                anchors.centerIn: parent
                rotation: arrow
                Text{
                    anchors.centerIn: parent
                    bottomPadding: height * 0.15
                    text: arrow === -1 ? "" : ">"
                    color: player === 1 ? "#363237" :  "white"
                    font.weight: Font.DemiBold
                    fontSizeMode: Text.Fit
                    font.pointSize: circle.width ? circle.width*0.6 : 1
                    horizontalAlignment: Text.AlignVCenter
                }
            }
        }
        MouseArea{
            anchors.fill: parent
            hoverEnabled: true
            onPositionChanged: rectangle.positionChanged(mouseX, mouseY, 2)
            onClicked: rectangle.clicked(mouseX, mouseY, 2)
            enabled: BoardModel.winner !== 0 ? false : true
        }
    }
    Rectangle{
        id: right_box
        anchors.left: middle_box.right
        width: parent.width * 0.2
        height: parent.height
        color: "transparent"
        MouseArea{
            anchors.fill: parent
            hoverEnabled: true
            onPositionChanged: rectangle.positionChanged(mouseX, mouseY, 3)
            onClicked: rectangle.clicked(mouseX, mouseY, 3)
            enabled: BoardModel.winner !== 0 ? false : true
        }
    }
}



