#include "systemtray.h"
#include "systemtrayprivate.h"
#include <QRect>

SystemTray::SystemTray(QObject *parent) : 
	QObject(parent),
	d_ptr(new SystemTrayPrivate(this))
{
	d_ptr->initTray(QString(),QString());
}

SystemTray::~SystemTray()
{
	d_ptr->removeTray();
	delete d_ptr;
}

QString SystemTray::getToolTip() const noexcept
{
	return d_ptr->tooltip;
}

bool SystemTray::getFlashFlag() const noexcept
{
	return d_ptr->flashFlag;
}

QRect SystemTray::geometry() noexcept
{
	return d_ptr->geometry();
}

QPoint SystemTray::getPopupWindowPosition(int width, int height) noexcept
{
	return d_ptr->getPopupWindowPosition(width,height);
}

QUrl SystemTray::getSource() const noexcept
{
	return d_ptr->source;
}

void SystemTray::setSource(QUrl source) noexcept
{
	if (d_ptr->source == source)
		return;
	
	d_ptr->setIcon(source);
	emit sourceChanged(d_ptr->source);
}

void SystemTray::setToolTip(QString tooltip) noexcept
{
	if (d_ptr->tooltip == tooltip)
		return;
	
	d_ptr->setTip(tooltip);
	emit tooltipChanged(d_ptr->tooltip);
}

void SystemTray::setFlashFlag(bool flashFlag) noexcept
{
	if (d_ptr->flashFlag == flashFlag)
		return;
	
	d_ptr->setFlash(flashFlag);
	emit flashFlagChanged(d_ptr->flashFlag);
}
