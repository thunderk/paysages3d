import QtQuick 2.0

Rectangle {
    opacity: enabled ? 1 : 0
    enabled: true

    Behavior on opacity {
        PropertyAnimation {
            duration: 200
        }
    }
}

