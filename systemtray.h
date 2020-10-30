#ifndef SYSTEMTRAY_H
#define SYSTEMTRAY_H

#include <QObject>
#include <QUrl>
#include <QRect>

class SystemTrayPrivate;
class QString;
class SystemTray;

class SystemTray : public QObject
{
	Q_OBJECT
public:
	enum ActivationReason{
		Unknown,
		LeftClick,
		LeftDoubleClick,
		RightClick,
		RightDoubleClick,
		MiddleClick,
		MiddleDoubleClick,
		Enter,
		Leave,
		Hover,
	};
	
	Q_ENUMS(ActivationReason)
public:
	Q_PROPERTY(QUrl		source		READ getSource		WRITE setSource
			   NOTIFY sourceChanged)
	Q_PROPERTY(QString	tooltip		READ getToolTip		WRITE setToolTip 
			   NOTIFY tooltipChanged)
	Q_PROPERTY(bool		flashFlag	READ getFlashFlag	WRITE setFlashFlag
			   NOTIFY	flashFlagChanged)
public:
	explicit SystemTray(QObject *parent = nullptr);
	
	~SystemTray();
	
	
	QUrl getSource() const noexcept;
	QString getToolTip() const noexcept;
	bool getFlashFlag() const noexcept;
	
	/**
	 * @brief geometry
	 * 获取托盘区域
	 */
	Q_INVOKABLE QRect geometry() noexcept;
	
	/**
	 * @brief getMenuShowPoint
	 * 获取菜单显示的左上角坐标
	 * @param width	菜单宽度
	 * @param height 菜单高度
	 */
	Q_INVOKABLE QPoint getPopupWindowPosition(int width,
											  int height) noexcept;
public slots:
	void setSource(QUrl source) noexcept;
	void setToolTip(QString tooltip) noexcept;
	void setFlashFlag(bool flashFlag) noexcept;
	
signals:
	void sourceChanged(QUrl source);
	void tooltipChanged(QString tip);
	void flashFlagChanged(bool flashFlag);
	
	/**
	 * @brief activated
	 * 托盘鼠标事件触发时会发出该信号
	 * @param reason		触发的原因
	 * @param point			触发时的鼠标坐标(Hover和Leave该值为0)
	 */
	void activated(int reason,QPoint point);
private:
	SystemTrayPrivate * d_ptr;
};

#endif // SYSTEMTRAY_H
