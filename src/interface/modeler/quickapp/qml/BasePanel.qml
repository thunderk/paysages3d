import QtQuick 2.0

BaseRectangle {
    property ToolbarButton tool
    id: panel

    enabled: false
    width: 200
    height: parent.height - 100
    color: "#a0909090"

    anchors.right: parent.right
    anchors.verticalCenter: parent.verticalCenter

    states: [
        State {
            name: "Active"
            when: tool.selected

            PropertyChanges {
                target: panel
                enabled: true
            }
        }

    ]
}
