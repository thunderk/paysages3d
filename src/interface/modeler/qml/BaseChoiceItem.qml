import QtQuick 2.0
import QtQuick.Controls 1.1

Rectangle {
    id: choice_item
    property string icon
    property bool checked: false
    property ExclusiveGroup exclusiveGroup: null
    property int value
    property int padding: 4

    color: "#333333"
    radius: padding * 2

    signal toggled(bool value)

    width: 40
    height: 40

    Image {
        id: icon_image
        source: parent.icon
        width: parent.width - 2 * parent.padding
        height: parent.height - 2 * parent.padding
        anchors.centerIn: parent
        antialiasing: true
    }

    MouseArea {
        anchors.fill: parent
        cursorShape: Qt.PointingHandCursor
        onClicked: checked = true
    }

    onExclusiveGroupChanged: {
        if (exclusiveGroup) {
            exclusiveGroup.bindCheckable(choice_item);
        }
    }

    onCheckedChanged: choice_item.toggled(checked)

    Behavior on color {
        PropertyAnimation {
            duration: 200
        }
    }

    states: [
        State {
            name: "Checked"
            when: checked

            PropertyChanges {
                target: choice_item
                color: "#dddddd"
            }
        }

    ]
}
