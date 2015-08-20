import QtQuick 2.0
import QtGraphicalEffects 1.0

Item {
    id: button
    property string picture
    property bool selected: false
    property bool hovered: false
    property bool toggle: false
    property string helptext
    property string hovertext
    signal clicked
    signal changed(bool value)

    width: image.width + 10
    height: image.height + 10
    opacity: enabled ? 1.0 : 0.1

    onSelectedChanged: changed(selected)

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

        opacity: button.selected ? 1.0 : (button.hovered ? 0.5 : 0.0)
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
            tooltip_widget.hovertext = hovertext;
            tooltip_widget.hovered = this;
        }
        onExited: {
            button.hovered = false;
            tooltip_widget.hovertext = "";
        }
        onClicked: {
            button.selected = !button.selected;
            if (!button.toggle)
            {
                if (button.selected)
                {
                    var toolbar = button.parent;
                    for (var i = 0; i < toolbar.children.length; ++i)
                    {
                        var child = toolbar.children[i]
                        if (child !== button && !child.toggle)
                        {
                            child.selected = false;
                        }
                    }
                    tooltip_widget.helptext = helptext;
                }
                else
                {
                    tooltip_widget.helptext = "";
                }
            }
            button.clicked();
        }
    }
}
