import QtQuick 2.2
import QtQuick.Controls 1.2
import QtQuick.Dialogs 1.0
import QtGraphicalEffects 1.0

BaseRectangle {
    id: render_dialog
    objectName: "render_dialog"
    width: 400
    height: 300
    color: "#222429"
    property bool rendering: false

    function refresh() {
        preview_image.source = "";
        preview_image.source = "image://renderpreviewprovider/live";
    }

    onVisibleChanged: {
        if (visible)
        {
            refresh();
            render_progress.value = 0;
        }
    }

    Rectangle {
        width: parent.width
        height: 80
        anchors.top: parent.top
        anchors.left: parent.left
        color: "#667080"
    }

    FileDialog {
        id: render_save_dialog
        objectName: "render_save_dialog"
        title: "Choose a file to save the rendered image"
        folder: shortcuts.documents
        selectExisting: false

        signal saveRequired(string filepath);

        onAccepted: saveRequired(render_save_dialog.fileUrls[0].toString())
    }

    ToolbarButton {
        id: render_save
        objectName: "render_save"
        picture: "images/icon_file_save.png"
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.margins: 10
        enabled: !render_dialog.rendering
        checkable: false
        image_width: 48
        image_height: 48

        onClicked: render_save_dialog.open()
    }

    ToolbarButton {
        id: render_cancel
        objectName: "render_cancel"
        picture: "images/icon_cancel.png"
        anchors.top: parent.top
        anchors.right: parent.right
        anchors.margins: 10
        checkable: false
        image_width: 48
        image_height: 48
    }

    RectangularGlow {
        anchors.fill: preview_image
        glowRadius: 8
        spread: 0.2
        color: "#e0e0e8"
        cornerRadius: glowRadius
    }

    Image {
        id: preview_image
        objectName: "render_preview_image"
        anchors.centerIn: parent
        width: 100
        height: 100
        source: ""
        cache: false
    }

    ProgressBar {
        id: render_progress
        objectName: "render_progress"
        visible: render_dialog.rendering
        width: parent.width * 0.8
        anchors.top: preview_image.bottom
        anchors.horizontalCenter: preview_image.horizontalCenter
        anchors.topMargin: 20
    }

    Text {
        id: render_timing
        objectName: "render_timing"
        anchors.top: render_progress.bottom
        anchors.horizontalCenter: render_progress.horizontalCenter
        anchors.topMargin: 20
        color: "#dddddd"
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
