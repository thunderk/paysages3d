import QtQuick 2.0
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.1

BasePanel {
    id: daytime
    width: 100

    objectName: "atmosphere_daytime"
    default property real value: day_night.value == 2 ? 0.0 : slider.value * 0.54 + 0.23;
    signal changed(real value)

    onValueChanged: {
        changed(value);
        day_night.value = (value >= 0.23 && value <= 0.77) ? 1 : 2;
        if (day_night.value == 1)
        {
            slider.value = (value - 0.23) / 0.54;
        }
    }

    ColumnLayout
    {
        height: parent.height
        anchors.horizontalCenter: parent.horizontalCenter
        spacing: 20

        Item {height: 1}

        BaseChoice {
            id: day_night

            Layout.alignment: Qt.AlignHCenter
            spacing: 10
            width: 90
            height: 40

            BaseChoiceItem {
                icon: "images/icon_atmosphere_day.png"
                value: 1
                checked: true
            }
            BaseChoiceItem {
                icon: "images/icon_atmosphere_night.png"
                value: 2
            }
        }

        BaseSlider {
            id: slider
            orientation: Qt.Vertical
            Layout.maximumWidth: 15
            Layout.fillHeight: true
            Layout.alignment: Qt.AlignHCenter
        }

        Grid {
            id: clock
            property int hour: daytime.value * 86400 / 3600
            property int minute: (daytime.value * 86400 - 3600 * hour) / 60
            property int second: daytime.value * 86400 - 3600 * hour - 60 * minute
            rows: 3
            columns: 5
            rowSpacing: 4

            ClickableImage {
                width: 20
                height: 10
                source: "qrc:/images/arrow_up.png"
                onClicked: slider.value += (1.0 / 24.0) / 0.54
            }
            Item {width: 1; height: 1}
            ClickableImage {
                width: 20
                height: 10
                source: "qrc:/images/arrow_up.png"
                onClicked: slider.value += (1.0 / 1440.0) / 0.54
            }
            Item {width: 1; height: 1}
            ClickableImage {
                width: 20
                height: 10
                source: "qrc:/images/arrow_up.png"
                onClicked: slider.value += (1.0 / 86400.0) / 0.54
            }

            Text {
                text: (clock.hour > 9 ? "" : "0") + clock.hour.toString()
                font.pixelSize: 14
            }
            Text {
                text: " : "
                font.pixelSize: 14
            }
            Text {
                text: (clock.minute > 9 ? "" : "0") + clock.minute.toString()
                font.pixelSize: 14
            }
            Text {
                text: " : "
                font.pixelSize: 14
            }
            Text {
                text: (clock.second > 9 ? "" : "0") + clock.second.toString()
                font.pixelSize: 14
            }

            ClickableImage {
                width: 20
                height: 10
                source: "qrc:/images/arrow_down.png"
                onClicked: slider.value -= (1.0 / 24.0) / 0.54
            }
            Item {width: 1; height: 1}
            ClickableImage {
                width: 20
                height: 10
                source: "qrc:/images/arrow_down.png"
                onClicked: slider.value -= (1.0 / 1440.0) / 0.54
            }
            Item {width: 1; height: 1}
            ClickableImage {
                width: 20
                height: 10
                source: "qrc:/images/arrow_down.png"
                onClicked: slider.value -= (1.0 / 86400.0) / 0.54
            }
        }

        Item {height: 1}
    }

    states: [
        State {
            name: "night"
            when: day_night.value == 2
            PropertyChanges {
                target: slider
                enabled: false
            }
            PropertyChanges {
                target: clock
                enabled: false
            }
        }

    ]
}
