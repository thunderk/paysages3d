import QtQuick 2.0
import QtGraphicalEffects 1.0
import QtQuick.Controls 1.2

Item {
    id: button
    property var toolbar: null
    property string picture
    property bool checked: false
    property bool hovered: false
    property bool toggle: false
    property bool checkable: true
    property ExclusiveGroup exclusiveGroup: null
    property string helptext
    property string hovertext
    property string shortcut
    property alias image_width: image.width
    property alias image_height: image.height
    signal clicked
    signal toggled(bool value)

    width: image.width + 10
    height: image.height + 10
    opacity: enabled ? 1.0 : 0.1

    onExclusiveGroupChanged: {
        if (exclusiveGroup)
            exclusiveGroup.bindCheckable(button)
    }

    onCheckedChanged: {
        toggled(checked);
        if (toolbar && !toggle && checked) {
            toolbar.current = button;
        }
    }

    Behavior on opacity {
        PropertyAnimation {
            duration: 200
        }
    }

    Rectangle {
        id: glow
        anchors.fill: parent
        color: parent.toggle ? "#bbccbb" : "#cccccc"
        radius: 8

        opacity: button.checked ? 1.0 : (button.hovered ? 0.5 : 0.0)
        Behavior on opacity {
            PropertyAnimation {
                duration: 200
            }
        }
        RectangularGlow {
            anchors.fill: glow
            glowRadius: 8
            spread: 0.2
            color: button.toggle ? "#99aa99" : "#ffffff"
            cornerRadius: glow.radius + glowRadius
        }
    }

    Image {
        id: image
        source: button.picture
        anchors.centerIn: parent
        width: 32
        height: 32
        antialiasing: true
    }
    DropShadow {
        anchors.fill: image
        horizontalOffset: 2
        verticalOffset: 2
        radius: 4.0
        samples: 16
        color: "#80000000"
        source: image
    }

    MouseArea {
        anchors.fill: parent
        hoverEnabled: true
        cursorShape: Qt.PointingHandCursor

        onEntered: {
            button.hovered = true;
            tooltip_widget.hovertext = button.hovertext + (button.shortcut ? (" (" + button.shortcut + ")") : "");
            tooltip_widget.hovered = this;
        }
        onExited: {
            button.hovered = false;
            tooltip_widget.hovertext = "";
        }
        onClicked: {
            if (button.checked && button.exclusiveGroup) {
                return;
            }

            if (button.checkable)
            {
                button.checked = !button.checked;
                if (!button.toggle)
                {
                    if (button.checked)
                    {
                        tooltip_widget.helptext = helptext;
                    }
                    else
                    {
                        tooltip_widget.helptext = "";
                    }
                }
            }

            button.clicked();
        }
    }
}
