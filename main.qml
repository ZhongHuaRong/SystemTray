import QtQuick 2.9
import QtQuick.Window 2.2
import QtQuick.Controls 2.2
import an.qt.cxx 1.0

Window {
	visible: true
	width: 640
	height: 480
	title: qsTr("Hello World")
	
	Button{
		id:flashBtn
		text: tray.flashFlag?"停止闪烁":"开始闪烁"
		onClicked:  tray.flashFlag = !tray.flashFlag
	}
	
	SystemTray{
		id:tray
		source: "D://Documents/QTProjects/SystemTray/1"
		
		onActivated: {
			switch(reason){
			case SystemTray.Enter:
				var i = tray.getPopupWindowPosition(hoverPopup.width,hoverPopup.height)
				hoverPopup.open(i.x,i.y)
				break
			case SystemTray.Leave:
				if(!area.containsMouse)
					hoverPopup.hide()
				break
			case SystemTray.LeftClick:
				menu.open(point.x,point.y)
				break
			case SystemTray.LeftDoubleClick:
				console.debug("左双击")
				break
			case SystemTray.RightClick:
				menu.open(point.x ,point.y)
				break
			case SystemTray.RightDoubleClick:
				console.debug("右双击")
				break
			case SystemTray.MiddleClick:
				console.debug("中单机")
				break
			case SystemTray.MiddleDoubleClick:
				console.debug("中双击")
				break
			case SystemTray.Hover:
				console.debug("悬浮")
				break
			}
		}
	}
	
	CPopup{
		id:menu 
		height:30
		width:100
		
		Column{
			width:92
			
			Button{
				width:92
				height:22
				text: "退出"
				onClicked: {
					Qt.quit()
				}
			}
		}
	}
	
	CPopup{
		id:hoverPopup
		height:100
		width: 200
		
		background.color: "#ffffff"
		
		MouseArea{
			id:area 
			anchors.fill: parent
			hoverEnabled: true
			
			onExited: hoverPopup.hide()
		}
	}
	
	TextInput {
		id: textInput
		x: 0
		y: 51
		width: 165
		height: 20
		text: qsTr("Text Input")
		font.pixelSize: 12
	}
	
	Button {
		id: button
		x: 0
		y: 88
		text: qsTr("修改提示语")
		onClicked: tray.tooltip = textInput.text
	}
}
