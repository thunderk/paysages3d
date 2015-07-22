import QtQuick 2.2

Rectangle {
    width: 400
    height: 300

    function refresh() {
        preview_image.source = "";
        preview_image.source = "image://renderpreviewprovider/live";
    }

    onOpacityChanged: {
        refresh();
    }
    onVisibleChanged: {
        refresh();
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

    Timer {
        interval: 500
        running: true
        repeat: true

        onTriggered: {
            refresh();
        }
    }
}
