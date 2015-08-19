import QtQuick 2.0
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.1

Item {
    default property alias children : inner_layout.children
    property alias spacing : inner_layout.spacing
    property int value

    ExclusiveGroup {
        id: choice_group

        onCurrentChanged: value = current.value
    }

    Row {
        id: inner_layout
        spacing: 5
        anchors.fill: parent
    }

    onValueChanged: {
        for (var i = 0; i < children.length; i++)
        {
            if (children[i].value == value)
            {
                choice_group.current = children[i];
            }
        }
    }

    onChildrenChanged: {
        for (var i = 0; i < children.length; i++)
        {
            children[i].exclusiveGroup = choice_group;
        }
    }
}
