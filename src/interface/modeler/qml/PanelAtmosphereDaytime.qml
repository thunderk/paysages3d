import QtQuick 2.0

BasePanel {
    width: 200

    Column {
        anchors.centerIn: parent
        width: parent.width - 10
        spacing: 30

        Text {
            text: "Sun location in the sky"
            anchors.horizontalCenter: parent.horizontalCenter
        }

        WidgetSphericalCoords {
            id: atmosphere_sun_direction
            objectName: "atmosphere_sun_direction"
            anchors.horizontalCenter: parent.horizontalCenter
            icon: "images/icon_atmosphere_day.png"
            width: parent.width
        }

        Text {
            text: "Time of day"
            anchors.horizontalCenter: parent.horizontalCenter
        }

        WidgetDayTime {
            id: atmosphere_daytime
            objectName: "atmosphere_daytime"
            anchors.horizontalCenter: parent.horizontalCenter
            onValueChanged: atmosphere_sun_direction.theta = (atmosphere_daytime.value + 0.75) * Math.PI * 2.0
        }

        Text {
            text: "Moon location in the sky"
            anchors.horizontalCenter: parent.horizontalCenter
        }

        WidgetSphericalCoords {
            id: atmosphere_moon_direction
            objectName: "atmosphere_moon_direction"
            icon: "images/icon_atmosphere_night.png"
            width: parent.width
            anchors.horizontalCenter: parent.horizontalCenter
        }
    }
}
