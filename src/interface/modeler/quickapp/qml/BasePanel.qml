import QtQuick 2.0

Rectangle {
    property ToolbarButton tool
    id: panel

    opacity: 0
    width: 200
    height: parent.height - 100
    color: "#a0909090"
    enabled: visible

    anchors.right: parent.right
    anchors.verticalCenter: parent.verticalCenter

    Behavior on opacity {
        PropertyAnimation {
            duration: 200
        }
    }

    states: [
        State {
            name: "Active"
            when: tool.selected

            PropertyChanges {
                target: panel
                visible: true
                opacity: 1
            }
        }

    ]
}
