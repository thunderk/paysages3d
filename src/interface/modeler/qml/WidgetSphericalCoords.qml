import QtQuick 2.0

// Widget to edit spherical coordinates
Row {
    id: widget
    property real theta: 0
    property real phi: 0
    property string icon

    height: width / 2

    Item {
        height: parent.height
        width: height
        Image {
            source: "images/bg_phi_coord.png"
            anchors.fill: parent
        }
        Image {
            source: widget.icon
            width: parent.width / 10
            height: parent.width / 10
            anchors.centerIn: parent
            anchors.horizontalCenterOffset: width * 4 * Math.cos(phi) * Math.cos(theta)
            anchors.verticalCenterOffset: width * 4 * Math.sin(phi) * Math.cos(theta)
        }
        MouseArea {
            anchors.fill: parent
            hoverEnabled: true
            property real oldval: 0

            onPositionChanged: {
                var newval = Math.atan2(mouseY - height / 2, mouseX - width / 2);
                if (pressedButtons & Qt.LeftButton) {
                    phi += newval - oldval;
                }
                oldval = newval;
            }
        }
    }

    Item {
        height: parent.height
        width: height
        Image {
            source: "images/bg_theta_coord.png"
            anchors.fill: parent
        }
        Image {
            source: widget.icon
            width: parent.width / 10
            height: parent.width / 10
            anchors.centerIn: parent
            anchors.horizontalCenterOffset: width * 4.5 * Math.cos(theta)
            anchors.verticalCenterOffset: -width * 4.5 * Math.sin(theta)
        }
        MouseArea {
            anchors.fill: parent
            hoverEnabled: true
            property real oldval: 0

            onPositionChanged: {
                var newval = Math.atan2(height / 2 - mouseY, mouseX - width / 2);
                if (pressedButtons & Qt.LeftButton) {
                    theta += newval - oldval;
                }
                oldval = newval;
            }
        }
    }
}
