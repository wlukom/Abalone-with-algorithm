import QtQuick 2.0

Rectangle {
    id: id_p
    x: x_p
    y: y_p
    width: size_p
    height: size_p
    radius: size_p/2
    border.width: 1
    border.color: isClicked ? "yellow" : "black"
    color: player === 1 ? "#FF9999" : player === 2 ? "silver" : "teal"

    Text{
        anchors.centerIn: parent
        text: id_role
    }
    MouseArea{
        hoverEnabled: true
        onPositionChanged:{
            console.log("x: ", mouseX, "y: ", mouseY);
        }
        anchors.fill: parent
        onClicked: {
            console.log("Click"+id_role)
            FieldModel.pushClickedField(id_role)
            FieldModel.action(id_role)

        }
    }

}
