#ifndef SYSTEMTRAYPRIVATE_H
#define SYSTEMTRAYPRIVATE_H

#include <QObject>
#include "windows.h"    
#include "shellapi.h"
#include <QIcon>
#include <QPixmap>
#include <QWidget>
#include <QTimer>
#include <QtWinExtras/qwinfunctions.h>
#include <QUrl>
#include <QDebug>

class SystemTray;
class SystemTrayPrivate;

class TrayEventWindow: public QWidget{
	Q_OBJECT
public:
	enum EventType{
		WM_TRAYNOTIFY = WM_USER + 1
	};
	
signals:
	void mouseEvent(int type,int x,int y);
protected:
	bool nativeEvent(const QByteArray &eventType, void *message, long *result) override;
};

class SystemTrayPrivate:public QObject{
	Q_OBJECT
public:
	NOTIFYICONDATA		nid;
	QPixmap				icon;
	HICON				hicon{NULL};
	
	TrayEventWindow		eventWindow;
	QString				tooltip;
	bool				flashFlag{false};
	bool				hover{false};
	QUrl				source;
	QTimer				moveTimer;
	QTimer				flashTimer;
	SystemTray			*systemTray{nullptr};
	bool				contain{false};
	
	SystemTrayPrivate(SystemTray *ptr);
	
	/**
	 * @brief initTray
	 * 初始化并创建托盘
	 * @param source		图标资源路劲
	 * @param tip			悬浮提示文字
	 */
	void initTray(const QString& source,
				  const QString& tip) noexcept;
	
	/**
	 * @brief setIcon
	 * 设置新的图标
	 */
	void setIcon(QUrl url) noexcept;
	
	/**
	 * @brief removeTray
	 * 移除托盘
	 */
	void removeTray() noexcept;
	
	/**
	 * @brief setFlash
	 * 设置图标闪烁
	 */
	void setFlash(bool flag) noexcept;
	
	/**
	 * @brief setTip
	 * 设置提示
	 */
	void setTip(QString tip) noexcept;
	
	/**
	 * @brief geometry
	 * 获取图标位置和大小
	 */
	QRect geometry() noexcept;
	
	/**
	 * @brief containMouse
	 * 判断鼠标在不在托盘内
	 */
	bool containMouse() noexcept;
	
	/**
	 * @brief getPopupWindowPosition
	 * 获取弹窗位置
	 * @param width			弹窗宽度
	 * @param height		弹窗高度
	 * @return 
	 */
	QPoint getPopupWindowPosition(int width,int height) noexcept;
public slots:
	/**
	 * @brief mousePos
	 * 判断目前鼠标位置
	 * 如果在托盘里面则不处理,
	 * 不在托盘里面则需要发出leave信号
	 */
	void mousePos() noexcept;
	
	/**
	 * @brief dealMouseMove
	 * 处理鼠标移动事件
	 */
	void dealMouseMove(int type,int x,int y) noexcept;
	
	/**
	 * @brief flashIcon
	 * 按照要求闪烁icon
	 */
	void flashIcon() noexcept;
protected:
	/**
	 * @brief platformPoint2QPoint
	 * 系统坐标转为qt坐标
	 */
	QPoint platformPoint2QPoint(QPoint point) noexcept;
	
	/**
	 * @brief qpoint2PlatfromPoint
	 * qt坐标转为系统坐标
	 */
	QPoint qpoint2PlatfromPoint(QPoint point) noexcept;
	
	/**
	 * @brief getScaleFactor
	 * 获取比例因子
	 */
	float getScaleFactor() noexcept;
private:
	float		scaleFactor{0.0};
};

#endif // SYSTEMTRAYPRIVATE_H
