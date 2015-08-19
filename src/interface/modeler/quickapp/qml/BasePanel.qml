import QtQuick 2.0

BaseRectangle {
    property ToolbarButton tool
    id: panel

    width: 200
    height: primary_toolbar.current ? primary_toolbar.current.height : 10
    color: "#40909090"

    anchors.left: primary_toolbar.current ? primary_toolbar.current.right : parent.left
    anchors.top: primary_toolbar.current ? primary_toolbar.current.top : parent.top

    states: [
        State {
            name: "hidden"
            when: !tool.selected

            PropertyChanges {
                target: panel
                enabled: false
            }
        }

    ]
}
