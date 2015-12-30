import QtQuick 2.0

Grid {
    id: clock

    property real value
    property real modvalue: value - Math.floor(value)

    property int hour: Math.floor(86400.0 * modvalue / 3600.0)
    property int minute: Math.floor((86400.0 * modvalue - 3600.0 * hour) / 60.0)
    property int second: Math.floor(86400.0 * modvalue - 3600.0 * hour - 60.0 * minute)
    rows: 3
    columns: 5
    rowSpacing: 4

    ClickableImage {
        width: 20
        height: 10
        source: "images/arrow_up.png"
        onClicked: value = modvalue + 1.0 / 24.0
    }
    Item {width: 1; height: 1}
    ClickableImage {
        width: 20
        height: 10
        source: "images/arrow_up.png"
        onClicked: value = modvalue + 1.0 / 1440.0
    }
    Item {width: 1; height: 1}
    ClickableImage {
        width: 20
        height: 10
        source: "images/arrow_up.png"
        onClicked: value = modvalue + 1.0 / 86400.0
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
        onClicked: value = modvalue - 1.0 / 24.0
    }
    Item {width: 1; height: 1}
    ClickableImage {
        width: 20
        height: 10
        source: "images/arrow_down.png"
        onClicked: value = modvalue - 1.0 / 1440.0
    }
    Item {width: 1; height: 1}
    ClickableImage {
        width: 20
        height: 10
        source: "images/arrow_down.png"
        onClicked: value = modvalue - 1.0 / 86400.0
    }
}
