#ifndef POPUPWINDOW_H
#define POPUPWINDOW_H

#include <QObject>
#include <QQuickWindow>

/**
 * @brief The PopupWindow class
 * qml的window没有窗口丢失焦点的函数
 * 所以用C++的事件处理函数实现一个
 */
class PopupWindow : public QQuickWindow
{
    Q_OBJECT
public:
	
	Q_PROPERTY(bool windowFocus READ getWindowFocus WRITE setWindowFocus NOTIFY windowFocusChanged)
public:
    explicit PopupWindow(QQuickWindow *parent = nullptr);

	bool getWindowFocus() const;
	void setWindowFocus(bool value);
	
	/**
	 * @brief open
	 * 在xy位置打开窗口
	 */
	Q_INVOKABLE void open(int x,int y) noexcept;
protected:
	virtual bool event(QEvent* e) override;
signals:
    void windowFocusChanged();
	
private:
	bool windowFocus{false};
};

#endif // POPUPWINDOW_H
