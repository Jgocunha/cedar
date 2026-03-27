// CEDAR INCLUDES
#include "cedar/auxiliaries/gui/BaseWidget.h"

// SYSTEM INCLUDES
#include <QGuiApplication>
#include <QScreen>
#include <iostream>

// constructors
cedar::aux::gui::BaseWidget::BaseWidget(const std::string& widgetName, QWidget* pParent)
:
QWidget(pParent),
mWidgetName(widgetName)
{
}

namespace
{
  bool isWaylandPlatform()
  {
    return QGuiApplication::platformName().contains("wayland", Qt::CaseInsensitive);
  }
}

// destructor
cedar::aux::gui::BaseWidget::~BaseWidget()
{
}

void cedar::aux::gui::BaseWidget::showEvent(QShowEvent*)
{
  emit visibilityChanged(true);
}

void cedar::aux::gui::BaseWidget::hideEvent(QHideEvent*)
{
  emit visibilityChanged(false);
}

void cedar::aux::gui::BaseWidget::readCustomSettings(QSettings&)
{
}

void cedar::aux::gui::BaseWidget::writeCustomSettings(QSettings&)
{
}

void cedar::aux::gui::BaseWidget::readWindowSettings()
{
  QString name = QString::fromStdString(this->mWidgetName);
  setWindowTitle(name);

  QSettings settings("INI", QString::fromStdString(this->mWidgetName));

  QSize size = settings.value("size", QSize(400, 400)).toSize();
  QPoint pos = settings.value("pos", QPoint(200, 200)).toPoint();

  QScreen* screen = this->screen();
  if (screen == nullptr)
  {
    screen = QGuiApplication::primaryScreen();
  }

  if (screen != nullptr)
  {
    QRect available = screen->availableGeometry();

    if (size.width() > available.width())
    {
      size.setWidth(available.width());
    }
    if (size.height() > available.height())
    {
      size.setHeight(available.height());
    }

    if (!isWaylandPlatform())
    {
      if (!available.contains(pos))
      {
        pos = available.topLeft() + QPoint(10, 10);
      }
    }
  }

  this->resize(size);

  // On Wayland, restoring explicit top-level position is unreliable.
  if (!isWaylandPlatform())
  {
    this->move(pos);
  }

  this->readCustomSettings(settings);
}

void cedar::aux::gui::BaseWidget::writeWindowSettings()
{
  QSettings settings("INI", QString::fromStdString(this->mWidgetName));

  if (!isWaylandPlatform())
  {
    settings.setValue("pos", this->pos());
  }

  settings.setValue("size", this->size());

  this->writeCustomSettings(settings);
}

void cedar::aux::gui::BaseWidget::keyPressEvent(QKeyEvent* pEvent)
{
  switch (pEvent->key())
  {
    case Qt::Key_G:
      if (pEvent->modifiers() == Qt::ControlModifier)
      {
        std::cout << "Writing window settings." << std::endl;
        writeWindowSettings();
      }
      break;
    default:
      this->QWidget::keyPressEvent(pEvent);
  }
}