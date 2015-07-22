import QtQuick 2.2
import QtQuick.Controls 1.2

Slider {
    signal changed(real value)
    onValueChanged: changed(value)
}
