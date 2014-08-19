import QtQuick 2.0

Item {
    property string code
    property bool selected: false
    property bool hovered: false

    width: image.width + 10
    height: image.height + 10

    Rectangle {
        id: glow
        anchors.fill: parent
        color: "white"
        radius: 8

        opacity: parent.selected ? 1.0 : (parent.hovered ? 0.5 : 0.0)
        Behavior on opacity {
            PropertyAnimation {
                duration: 200
            }
        }
    }

    Image {
        id: image
        source: "qrc:///toolbar/primary/tab_" + parent.code + ".png"
        anchors.centerIn: parent
    }

    MouseArea {
        anchors.fill: parent
        hoverEnabled: true

        onEntered: parent.hovered = true
        onExited: parent.hovered = false
        onClicked: parent.selected = true
    }
}
