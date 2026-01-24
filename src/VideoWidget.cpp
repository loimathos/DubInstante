#include "VideoWidget.h"
#include <QPainter>

VideoWidget::VideoWidget(QWidget *parent)
    : QOpenGLWidget(parent), m_videoSink(new QVideoSink(this)) {
  setAutoFillBackground(false);
  
  connect(m_videoSink, &QVideoSink::videoFrameChanged, this,
          &VideoWidget::handleFrame);
}

QVideoSink *VideoWidget::videoSink() const { return m_videoSink; }

void VideoWidget::handleFrame(const QVideoFrame &frame) {
  if (!frame.isValid()) {
    return;
  }

  QVideoFrame clonedFrame(frame);
  if (!clonedFrame.map(QVideoFrame::ReadOnly)) {
    return;
  }

  // Optimize: Convert to image directly. 
  // For most formats, toImage is reasonably fast if not coupled with extra format conversions.
  m_currentImage = clonedFrame.toImage().convertToFormat(QImage::Format_RGB888);
  clonedFrame.unmap();
  
  update();
}

void VideoWidget::paintEvent(QPaintEvent *event) {
  Q_UNUSED(event);

  QPainter painter(this);
  if (m_currentImage.isNull()) {
    painter.fillRect(rect(), QColor(240, 240, 240));
    painter.setPen(QColor(150, 150, 150));
    painter.setFont(QFont("Segoe UI", 12));
    painter.drawText(rect(), Qt::AlignCenter, "Aucune vidéo chargée");
    return;
  }

  // Calculate aspect ratio scaling
  QSize imageSize = m_currentImage.size();
  QSize widgetSize = size();

  float imageRatio = static_cast<float>(imageSize.width()) / imageSize.height();
  float widgetRatio =
      static_cast<float>(widgetSize.width()) / widgetSize.height();

  QRect targetRect;
  if (imageRatio > widgetRatio) {
    int targetHeight = static_cast<int>(widgetSize.width() / imageRatio);
    targetRect = QRect(0, (widgetSize.height() - targetHeight) / 2,
                       widgetSize.width(), targetHeight);
  } else {
    int targetWidth = static_cast<int>(widgetSize.height() * imageRatio);
    targetRect = QRect((widgetSize.width() - targetWidth) / 2, 0, targetWidth,
                       widgetSize.height());
  }
  
  painter.fillRect(rect(), Qt::black);
  painter.drawImage(targetRect, m_currentImage);
}
