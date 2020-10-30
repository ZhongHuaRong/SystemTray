#include "popupwindow.h"
#include <QScreen>
#include <QApplication>

PopupWindow::PopupWindow(QQuickWindow *parent) : QQuickWindow(parent)
{
	//去掉系统自带的阴影
	this->setFlags(Qt::Popup | Qt::NoDropShadowWindowHint);
}

bool PopupWindow::getWindowFocus() const
{
	return windowFocus;
}

void PopupWindow::setWindowFocus(bool value)
{
	windowFocus = value;
	if(value == false)
		hide();
	emit windowFocusChanged();
}

void PopupWindow::open(int x, int y) noexcept
{
	QScreen *screen=QGuiApplication::primaryScreen ();
	QRect mm=screen->availableGeometry();
	bool isRight = x + this->width() > mm.width();
	bool isBottom = y + this->height() > mm.height();
	if(isRight){
		if(isBottom){
			//右下角
			this->setPosition(x - width(),y - height());
		} else {
			//右上
			this->setPosition(x - width(),y);
		}
	} else {
		if(isBottom){
			//左下角
			this->setPosition(x,y - height());
		} else {
			//正常显示
			this->setPosition(x,y);
		}
	}
	show();
	requestActivate();
}

bool PopupWindow::event(QEvent *e)
{
	switch(e->type()){
	case QEvent::FocusIn:
		setWindowFocus(true);
		break;
	case QEvent::FocusOut:
		setWindowFocus(false);
		break;
	default:
		break;
	}

	return QQuickWindow::event(e);
}
