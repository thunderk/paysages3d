import QtQuick 2.0

BaseRectangle {
    property string helptext
    property string hovertext
    width: content.width
    height: content.height

    color: "#99000000"

    Text {
        id: content
        color: "white"
        text: parent.helptext || parent.hovertext
    }
}
