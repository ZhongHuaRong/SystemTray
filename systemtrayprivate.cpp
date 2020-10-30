#include "systemtrayprivate.h"
#include "systemtray.h"

#include <QGuiApplication>
#include <QScreen>

#pragma comment(lib,"user32.lib")
#pragma comment(lib,"Gdi32.lib")

bool TrayEventWindow::nativeEvent(const QByteArray &eventType, void *message, long *result)
{
	if (eventType == "windows_generic_MSG" || eventType == "windows_dispatcher_MSG")
	{
		MSG * pMsg = reinterpret_cast<MSG *>(message);
		
		if(pMsg->message == WM_TRAYNOTIFY){
			emit mouseEvent(pMsg->lParam,pMsg->pt.x,pMsg->pt.y);
			return true;
		}
	}
	return false;
}



SystemTrayPrivate::SystemTrayPrivate(SystemTray *ptr):
	systemTray(ptr) {
	moveTimer.setInterval(50);
	moveTimer.setSingleShot(true);
	flashTimer.setInterval(500);
	flashTimer.setSingleShot(false);
	
	connect(&moveTimer,&QTimer::timeout,
			this,&SystemTrayPrivate::mousePos);
	
	connect(&flashTimer,&QTimer::timeout,
			this,&SystemTrayPrivate::flashIcon);
	
	connect(&eventWindow,&TrayEventWindow::mouseEvent,
			this,&SystemTrayPrivate::dealMouseMove);
}

void SystemTrayPrivate::initTray(const QString &source, const QString &tip) noexcept
{
	this->source = source;
	this->tooltip = tip;
	
	nid.cbSize = sizeof nid;
	icon.load(source);
	nid.hIcon = QtWin::toHICON(icon);
	nid.hWnd = HWND(eventWindow.winId());
	nid.uCallbackMessage = TrayEventWindow::WM_TRAYNOTIFY;
	nid.uID = 1;
	nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
	wcscpy(nid.szTip, L"程序名称"); //信息提示条
	
	Shell_NotifyIcon(NIM_ADD, &nid);
}

void SystemTrayPrivate::setIcon(QUrl url) noexcept
{
	//释放上一次的图标
	if(nid.hIcon == hicon){
		hicon = NULL;
	} else if(hicon != NULL){
		nid.hIcon = hicon;
		hicon = NULL;
	}
	if(nid.hIcon != NULL)
		CloseHandle(nid.hIcon);
	
	icon.load(url.toString());
	nid.hIcon = QtWin::toHICON(icon);
	Shell_NotifyIcon(NIM_MODIFY,&nid);
}

void SystemTrayPrivate::removeTray() noexcept
{
	if(nid.hIcon == hicon){
		hicon = NULL;
	} else if(hicon != NULL){
		CloseHandle(hicon);
		hicon = NULL;
	}
	if(nid.hIcon != NULL)
		CloseHandle(nid.hIcon);
	Shell_NotifyIcon(NIM_DELETE, &nid);
}

void SystemTrayPrivate::setFlash(bool flag) noexcept
{
	if(flashFlag == flag)
		return;
	
	flashFlag = flag;
	if(flag){
		//做初始化操作
		if(nid.hIcon == hicon){
			hicon = NULL;
		} else if(hicon != NULL){
			CloseHandle(hicon);
			hicon = NULL;
		}
		flashTimer.start();
		
	} else {
		flashTimer.stop();
		
		if(nid.hIcon == NULL){
			nid.hIcon = hicon;
			
			Shell_NotifyIcon(NIM_MODIFY,&nid);
		}
	}
}

void SystemTrayPrivate::setTip(QString tip) noexcept
{
	if(tip == tooltip)
		return;
	if(tip.size() > 128)
		return;
	wcscpy(nid.szTip, tip.toStdWString().c_str());
	
	Shell_NotifyIcon(NIM_MODIFY, &nid);
}

QRect SystemTrayPrivate::geometry() noexcept
{
	NOTIFYICONIDENTIFIER notify;
	notify.cbSize = sizeof notify;
	notify.hWnd = HWND(eventWindow.winId());
	notify.uID = 1;
	notify.guidItem = GUID_NULL;
	
	RECT rect;
	Shell_NotifyIconGetRect(&notify, &rect);
	return QRect(rect.left, rect.top, 
				 rect.right - rect.left, rect.bottom - rect.top);
}

bool SystemTrayPrivate::containMouse() noexcept
{
	auto rect = geometry();
	POINT point;
	::GetCursorPos(&point);
	return rect.contains(point.x,point.y);
}

QPoint SystemTrayPrivate::getPopupWindowPosition(int width, 
												 int height) noexcept
{
	auto factor = getScaleFactor();
	
	QPoint targetPoint;
	//获取托盘位置
	auto iconRect = geometry();
	//转换成qt坐标
	iconRect.setRect(iconRect.x() / factor,
					 iconRect.y() / factor,
					 iconRect.width() / factor,
					 iconRect.height() / factor);
	
	QScreen *screen=QGuiApplication::primaryScreen ();
	//获取完整的屏幕大小和可用大小,用于判断任务栏大小和位置
	QRect completeRect = screen->geometry();
	QRect availabelRect = screen->availableGeometry();
	
	if(availabelRect.x() != 0){
		//靠左
		auto missionBoardRect = completeRect.size() - availabelRect.size();
		targetPoint.setY(iconRect.y() - (height - iconRect.height())/ 2);
		targetPoint.setX(missionBoardRect.width());
	} else if(availabelRect.y() != 0){
		//靠上
		targetPoint.setX(iconRect.x() - (width - iconRect.width() )/ 2);
		targetPoint.setY(iconRect.height());
	} else if(availabelRect.width() < completeRect.width()){
		//靠右
		targetPoint.setX(availabelRect.width() - width);
		targetPoint.setY(iconRect.y() - (height - iconRect.height())/ 2);
	} else {
		//靠下
		targetPoint.setX(iconRect.x() + (iconRect.width() - width )/ 2);
		targetPoint.setY(iconRect.y() - height);
	}
	
	return targetPoint;
}

void SystemTrayPrivate::mousePos() noexcept
{
	if(containMouse()){
		if(!hover){
			emit systemTray->activated(SystemTray::Hover,QPoint());
			hover = true;
		}
	}
	else {
		emit systemTray->activated(SystemTray::Leave,QPoint());
		contain = false;
		hover = false;
	}
}

void SystemTrayPrivate::dealMouseMove(int type,int x, int y) noexcept
{
	switch (type)
	{
	case WM_MOUSEMOVE:
	{
		//先判断是否包含鼠标,如果鼠标已经在托盘内则不处理,并开启计时器
		if(contain){
			moveTimer.start();
			return;
		}
		
		//发出鼠标进入事件信号,并开启计时器
		contain = true;
		emit systemTray->activated(SystemTray::Enter,platformPoint2QPoint(QPoint(x,y)));
		moveTimer.start();
		break;
	}
	case WM_LBUTTONDBLCLK:
		emit systemTray->activated(SystemTray::LeftDoubleClick,platformPoint2QPoint(QPoint(x,y)));
		break;
	case WM_LBUTTONUP:
		if(geometry().contains(x,y))
			emit systemTray->activated(SystemTray::LeftClick,platformPoint2QPoint(QPoint(x,y)));
		break;
	case WM_RBUTTONDBLCLK:
		emit systemTray->activated(SystemTray::RightDoubleClick,platformPoint2QPoint(QPoint(x,y)));
		break;
	case WM_RBUTTONUP:
		if(geometry().contains(x,y))
			emit systemTray->activated(SystemTray::RightClick,platformPoint2QPoint(QPoint(x,y)));
		break;
	case WM_MBUTTONUP:
		if(geometry().contains(x,y))
			emit systemTray->activated(SystemTray::MiddleClick,platformPoint2QPoint(QPoint(x,y)));
		break;
	case WM_MBUTTONDBLCLK:
		emit systemTray->activated(SystemTray::MiddleDoubleClick,platformPoint2QPoint(QPoint(x,y)));
		break;
	}
}

void SystemTrayPrivate::flashIcon() noexcept
{
	auto i = nid.hIcon;
	nid.hIcon = hicon;
	hicon = i;
	Shell_NotifyIcon(NIM_MODIFY,&nid);
}

QPoint SystemTrayPrivate::platformPoint2QPoint(QPoint point) noexcept
{
	auto factor = getScaleFactor();
	return QPoint(point.x() / factor,point.y() / factor);
}

QPoint SystemTrayPrivate::qpoint2PlatfromPoint(QPoint point) noexcept
{
	auto factor = getScaleFactor();
	return QPoint(point.x() * factor,point.y() * factor);
}

float SystemTrayPrivate::getScaleFactor() noexcept
{
//	int iWidth = GetSystemMetrics(SM_CXSCREEN); //获取屏幕水平分辨率
//	int iHeight = GetSystemMetrics(SM_CYSCREEN); //获取屏幕垂直分辨率
	
//	HDC hdc = ::GetDC(NULL);
//	int iScrWidth=GetDeviceCaps(hdc,HORZRES);//获取屏幕水平分辨率
//	int iScrHeight=GetDeviceCaps(hdc,VERTRES);//获取屏幕垂直分辨率

//	int iScrWidth2=GetDeviceCaps(hdc,HORZSIZE);//获取屏幕水平分辨率
//	int iScrHeight2=GetDeviceCaps(hdc,VERTSIZE);//获取屏幕垂直分辨率
	
//	int hdpi = GetDeviceCaps(hdc, LOGPIXELSX);//水平方向每逻辑英寸多少个像素点
//	int wdpi = GetDeviceCaps(hdc, LOGPIXELSY);//垂直方向每逻辑英寸多少个像素点
	
	if(scaleFactor < 0.0001f){
		QScreen *screen=QGuiApplication::primaryScreen ();
		float iWidth = GetSystemMetrics(SM_CXSCREEN);		//获取屏幕水平分辨率
		scaleFactor = iWidth / screen->size().width();		//qt获取到的是缩放后的宽度
	}
	
	return scaleFactor;
}
