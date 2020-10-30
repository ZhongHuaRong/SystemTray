import QtQuick 2.9
import QtQuick.Window 2.2
import QtGraphicalEffects 1.0
import an.qt.ui 1.0

PopupWindow {
    id: root
    visible: false
    width: 640
    height: 480
	color: "#00000000"
	
	property alias background: mainRect
	//设置默认属性，子类在其实现各种ui显示
	default property alias content:mainRect.children
	
	
	//背景区域，只读，提供给外部改写状态，防止拖动失效
	Rectangle{
		id:mainRect
		anchors.fill: parent
		anchors.margins: 4
		color: "#ffffff"
	}
	
	//阴影
	DropShadow {
		anchors.fill: mainRect
		horizontalOffset: 1
		verticalOffset: 1
		radius: 8
		samples: 16
		source: mainRect
		color: "#999999"
	}
}
