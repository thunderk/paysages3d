import QtQuick 2.0
import QtQuick.Controls 1.2

// Panel with a selection of pictures to choose from
// Mainly used for presets or models
BasePanel {
    id: panel
    property alias items: container.children
    property int value: -1
    width: items[0].width + 10

    ExclusiveGroup {
        id: exclusive_item

        onCurrentChanged: {
            if (current) {
                panel.value = current.position;
            } else {
                panel.value = -1;
            }
        }
    }

    onValueChanged: {
        if (!exclusive_item.current || exclusive_item.current.position !== panel.value) {
            if (panel.value >= 0) {
                exclusive_item.current = items[panel.value];
            } else {
                exclusive_item.current = null;
            }
        }
    }

    onItemsChanged: {
        for (var i = 0; i < items.length; i++) {
            items[i].exclusiveGroup = exclusive_item;
            items[i].position = i;
        }
    }

    Column {
        id: container
        anchors.centerIn: panel
    }
}

