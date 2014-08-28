import QtQuick 2.0

Rectangle {
    property ToolbarButton tool
    id: panel

    width: 200
    height: parent.height - 100
    color: "red"
    enabled: visible

    anchors.right: parent.right
    anchors.verticalCenter: parent.verticalCenter

    states: [
        State {
            name: "Active"
            when: tool.selected

            PropertyChanges {
                target: panel
                visible: true
            }
        }

    ]
}
