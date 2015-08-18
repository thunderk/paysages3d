import QtQuick 2.0
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.1

BasePanel {
    width: 70

    objectName: "atmosphere_daytime"
    default property real value: day_night.value == 2 ? 1.0 : slider.value * 0.54 + 0.23;
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
        anchors.fill: parent
        anchors.margins: 10
        spacing: 20

        BaseChoice {
            id: day_night
            width: parent.width

            Layout.alignment: Qt.AlignTop | Qt.AlignHCenter

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
            visible: day_night.value == 1
        }
    }
}
