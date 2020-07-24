#include "ttkapplication.h"

#include <QScreen>
#include <QMenu>
#include <QPainter>
#include <QMouseEvent>
#include <QFileDialog>
#include <QApplication>
#include <QDesktopWidget>

TTKApplication::TTKApplication(QWidget *parent)
	: QWidget(parent)
{
    m_isDrawing = false;
}

void TTKApplication::showEvent(QShowEvent *event)
{
    Q_UNUSED(event);
#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
    m_originPix = QApplication::primaryScreen()->grabWindow(QApplication::desktop()->winId());
#else
    m_originPix = QPixmap::grabWindow(QApplication::desktop()->winId());
#endif
    m_renderPix = m_originPix;

    showFullScreen();
    setCursor(Qt::CrossCursor);
}

void TTKApplication::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter painter(this);
    painter.drawPixmap(0, 0, m_renderPix);
}

void TTKApplication::mouseMoveEvent(QMouseEvent *event)
{
	QPoint pt = event->pos();
    if(m_isDrawing)
	{
        m_renderPix = m_originPix;
        m_endPonit = event->pos();
        paint(m_renderPix);
	}
	else
	{
        if(m_rect.contains(pt.x(), pt.y()))
		{
			setCursor(Qt::SizeAllCursor);
		}
		else
		{
			setCursor(Qt::CrossCursor);
		}
	}
}

void TTKApplication::mousePressEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton)
	{
        m_isDrawing = true;
        m_startPoint = event->pos();
	}
}

void TTKApplication::mouseReleaseEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton)
	{
        m_isDrawing = false;
        m_endPonit = event->pos();
        m_rect = QRect(m_startPoint, m_endPonit);
        paint(m_renderPix);
	}
}

void TTKApplication::paint(QPixmap &pixmap)
{
	QPainter painter(&pixmap);
    painter.drawRect(QRect(m_startPoint, m_endPonit));
	update();
}

void TTKApplication::keyPressEvent(QKeyEvent *event)
{
    if(event->key() == Qt::Key_Escape)
	{
		close();
	}
    else if(event->key() == Qt::Key_Space)
	{
        if(!m_rect.isNull())
		{
			save();
		}
	}
}

void TTKApplication::save()
{
	QString filename;
    filename = QFileDialog::getSaveFileName(this, tr("Save"), QDir::currentPath(), tr("Image (*.jpg *.png *.bmp)"));
    if(filename.isNull())
	{
		return;
	}


    QPixmap snapMap = QPixmap::grabWidget(this, m_rect);
	snapMap.save(filename);
}

void TTKApplication::contextMenuEvent(QContextMenuEvent *event)
{
    QAction *actionClose = new QAction(tr("Quit"),this);
	QAction *actionSave;
	QMenu menu(this);
    if(m_rect.contains(event->pos()))
	{ 
        actionSave = new QAction(tr("Save"), this);
		menu.addAction(actionSave);
		connect(actionSave, SIGNAL(triggered()), this, SLOT(save()));
	}
	menu.addAction(actionClose);
	connect(actionClose, SIGNAL(triggered()), this, SLOT(close()));
	menu.exec(event->pos());
}
