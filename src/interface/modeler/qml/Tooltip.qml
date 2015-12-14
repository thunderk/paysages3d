import QtQuick 2.0

BaseRectangle {
    property var hovered
    property string helptext
    property string hovertext
    width: content.width + 10
    height: content.height + 10
    enabled: content.deftext ? true : false
    anchors.margins: 5

    color: "#99000000"

    Text {
        id: content
        color: "white"
        font.bold: true
        font.pixelSize: 12
        property string deftext: parent.helptext || parent.hovertext
        property string oldtext
        text: deftext || oldtext
        anchors.centerIn: parent

        onDeftextChanged: {
            if (deftext)
            {
                oldtext = deftext;
            }
        }
    }
}
