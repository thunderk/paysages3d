import QtQuick 2.2
import QtQuick.Controls 1.2

Rectangle {
    width: 400
    height: 300

    function refresh() {
        preview_image.source = "";
        preview_image.source = "image://renderpreviewprovider/live";
    }

    onOpacityChanged: {
        refresh();
        render_progress.value = 0;
    }
    onVisibleChanged: {
        refresh();
        render_progress.value = 0;
    }

    Image {
        id: preview_image
        objectName: "preview_image"
        anchors.centerIn: parent
        width: 100
        height: 100
        source: ""
        cache: false
    }

    ProgressBar {
        id: render_progress
        objectName: "render_progress"
        width: parent.width * 0.8
        anchors.top: preview_image.bottom
        anchors.horizontalCenter: preview_image.horizontalCenter
        anchors.topMargin: 20
    }

    Timer {
        interval: 500
        running: true
        repeat: true

        onTriggered: {
            refresh();
        }
    }
}
