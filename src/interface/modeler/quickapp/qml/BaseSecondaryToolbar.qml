import QtQuick 2.0

BaseToolbar {
    enabled: false
    height: parent.height - primary_toolbar.height
    anchors.left: primary_toolbar.left
    anchors.top: primary_toolbar.bottom
}
