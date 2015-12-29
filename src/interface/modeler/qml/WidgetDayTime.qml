import QtQuick 2.0

Grid {
    id: clock

    property real value

    property int hour: value * 86400 / 3600
    property int minute: (value * 86400 - 3600 * hour) / 60
    property int second: value * 86400 - 3600 * hour - 60 * minute
    rows: 3
    columns: 5
    rowSpacing: 4

    ClickableImage {
        width: 20
        height: 10
        source: "images/arrow_up.png"
        onClicked: value += 1.0 / 24.0
    }
    Item {width: 1; height: 1}
    ClickableImage {
        width: 20
        height: 10
        source: "images/arrow_up.png"
        onClicked: value += 1.0 / 1440.0
    }
    Item {width: 1; height: 1}
    ClickableImage {
        width: 20
        height: 10
        source: "images/arrow_up.png"
        onClicked: value += 1.0 / 86400.0
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
        source: "images/arrow_down.png"
        onClicked: value -= 1.0 / 24.0
    }
    Item {width: 1; height: 1}
    ClickableImage {
        width: 20
        height: 10
        source: "images/arrow_down.png"
        onClicked: value -= 1.0 / 1440.0
    }
    Item {width: 1; height: 1}
    ClickableImage {
        width: 20
        height: 10
        source: "images/arrow_down.png"
        onClicked: value -= 1.0 / 86400.0
    }
}
