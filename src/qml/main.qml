import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material
import QtQuick.Layouts

import Backend

ApplicationWindow {
	id: root
	visible: true
	width: 805
	height: 500
	Material.theme: Material.System
	Material.primary: Material.Blue
	Material.accent: Material.Orange
	TabBar {
		id: bar
		width: parent.width
		TabButton {
			text: "Devices"
		}
		TabButton {
			text: "Applications"
		}
		TabButton {
			text: "Graph"
		}
	}
	ListView {
		anchors.top: bar.bottom
		anchors.bottom: parent.bottom
		anchors.left: parent.left
		anchors.right: parent.right
		model: DeviceModel
		delegate: RowLayout {
			width: parent.width
			Label {
				text: nick + "\n" + name
			}
			Slider {
				id: slider
				from: 0
				to: 200
				stepSize: 5
				Layout.fillWidth: true
			}
			Label {
				text: slider.value + " %"
			}
		}
	}
}
