import QtQuick 2.0

Rectangle {
    opacity: enabled ? 1 : 0
    enabled: true

    MouseArea {
        // Used to prevent mouse events to fall down to the OpenGLView
        anchors.fill: parent
        acceptedButtons: Qt.LeftButton | Qt.RightButton | Qt.MiddleButton
        onWheel: {}
    }

    Behavior on opacity {
        PropertyAnimation {
            duration: 200
        }
    }
}
