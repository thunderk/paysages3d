import QtQuick 2.0
import QtQuick.Controls 1.1

Rectangle {
    id: choice_item
    property string icon
    property bool checked: false
    property ExclusiveGroup exclusiveGroup: null
    property int value

    color: "#333333"

    signal toggled(bool value)

    width: 20
    height: 20

    Image {
        anchors.fill: parent
        source: parent.icon
        antialiasing:  true
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

    states: [
        State {
            name: "Checked"
            when: checked

            PropertyChanges {
                target: choice_item
                color: "#999999"
            }
        }

    ]
}
