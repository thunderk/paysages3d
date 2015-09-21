import QtQuick 2.0

BaseRectangle {
    property string tool: ""

    anchors.bottom: main_ui.bottom
    height: main_ui.height - primary_toolbar.height
}

