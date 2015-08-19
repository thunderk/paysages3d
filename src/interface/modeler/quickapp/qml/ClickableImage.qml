import QtQuick 2.0

Image {
    signal clicked()

    MouseArea {
        anchors.fill: parent
        cursorShape: Qt.PointingHandCursor

        onClicked: {
            parent.clicked();
        }
    }
}

