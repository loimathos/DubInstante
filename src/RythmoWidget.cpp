#include "RythmoWidget.h"
#include <QTimer>
#include <QFontDatabase>
#include <QFontMetrics>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QPainter>
#include <algorithm>

RythmoWidget::RythmoWidget(QWidget *parent)
    : QWidget(parent), m_speed(100), m_currentPosition(0), m_text(""),
      m_fontSize(16), m_verticalPadding(4), m_textColor(QColor(34, 34, 34)),
      m_barColor(QColor(0, 0, 0, 0)), m_playingBarColor(QColor(0, 0, 0, 0)),
      m_isPlaying(false), m_lastMouseX(0), m_seekTimer(new QTimer(this)) {
  m_seekTimer->setSingleShot(true);
  connect(m_seekTimer, &QTimer::timeout, this, &RythmoWidget::triggerSeek);
  setAutoFillBackground(false);
  setAttribute(Qt::WA_TranslucentBackground);
}

// ============================================================================
// Helpers (Centralized logic)
// ============================================================================

void RythmoWidget::requestDebouncedSeek(qint64 positionMs) {
  m_currentPosition = positionMs;
  update(); // Instant visual feedback
  m_seekTimer->start(200); // 200ms debounce
}

void RythmoWidget::triggerSeek() {
  emit scrubRequested(m_currentPosition);
}

QFont RythmoWidget::getFont() const {
  if (m_cachedCharWidth == -1) {
    m_cachedFont = QFontDatabase::systemFont(QFontDatabase::FixedFont);
    m_cachedFont.setPointSize(m_fontSize);
    m_cachedFont.setBold(true);
  }
  return m_cachedFont;
}

int RythmoWidget::charWidth() const {
  if (m_cachedCharWidth == -1) {
    QFontMetrics fm(getFont());
    m_cachedCharWidth = fm.horizontalAdvance('A');
  }
  return m_cachedCharWidth;
}

int RythmoWidget::cursorIndex() const {
  int cw = charWidth();
  if (cw <= 0)
    return 0;
  double distPixels = (double(m_currentPosition) / 1000.0) * m_speed;
  return static_cast<int>(distPixels / cw);
}

qint64 RythmoWidget::charDurationMs() const {
  int cw = charWidth();
  if (cw <= 0 || m_speed <= 0)
    return 40; // Fallback ~1 frame
  return static_cast<qint64>((double(cw) / m_speed) * 1000.0);
}

// ============================================================================
// Properties
// ============================================================================

void RythmoWidget::setSpeed(int speed) {
  if (m_speed != speed && speed > 0) {
    m_speed = speed;
    emit speedChanged(m_speed);
    update();
  }
}

int RythmoWidget::speed() const { return m_speed; }

void RythmoWidget::setText(const QString &text) {
  if (m_text != text) {
    m_text = text;
    update();
  }
}

QString RythmoWidget::text() const { return m_text; }

void RythmoWidget::setTextColor(const QColor &color) {
  if (m_textColor != color) {
    m_textColor = color;
    update();
  }
}

// Visual Style
void RythmoWidget::setVisualStyle(VisualStyle style) {
  if (m_visualStyle != style) {
    m_visualStyle = style;
    updateGeometry(); // Trigger re-layout
    update();
  }
}

RythmoWidget::VisualStyle RythmoWidget::visualStyle() const {
  return m_visualStyle;
}

// ============================================================================
// Sync (Video position → Widget)
// ============================================================================

void RythmoWidget::sync(qint64 positionMs) {
  if (m_currentPosition != positionMs) {
    m_currentPosition = positionMs;
    update();
  }
}

void RythmoWidget::setPlaying(bool playing) {
  if (m_isPlaying != playing) {
    m_isPlaying = playing;
    update();
  }
}

// ============================================================================
// Resize
// ============================================================================

QSize RythmoWidget::sizeHint() const {
  // Base band height (slim look)
  int h = 35;

  // Add Header space for Handle/Timestamp if needed
  if (m_visualStyle == Standalone || m_visualStyle == UnifiedTop) {
    h += 25; // 25px Header (Increased from 15 to fix text clipping)
  }

  return QSize(QWidget::sizeHint().width(), h);
}

// ============================================================================
// Paint
// ============================================================================

void RythmoWidget::paintEvent(QPaintEvent *event) {
  Q_UNUSED(event);
 
  QPainter painter(this);
  painter.setRenderHint(QPainter::Antialiasing);
 
  // 1. Calculate Layout Dimensions
  int headerHeight = 0;
  if (m_visualStyle == Standalone || m_visualStyle == UnifiedTop) {
    headerHeight = 25; // Matching sizeHint (Increased from 15)
  }
 
  int bandHeight = height() - headerHeight;
  int bandY = headerHeight;
  QRect bandRect(0, bandY, width(), bandHeight);
 
  // 2. Map Layout Parameters for use in Layers
  int cw = charWidth();
  int targetX = width() / 5;
  double pixelOffset = (double(m_currentPosition) / 1000.0) * m_speed;
  double textStartX = targetX - pixelOffset;

  // 3. Draw Band Background
  QColor bgColor = m_isPlaying ? m_playingBarColor : m_barColor;
  painter.fillRect(bandRect, bgColor);
 
  // 4. Draw Scrolling Text (Layer 1) - VIRTUALIZED
  if (cw > 0) {
    QFont font = getFont();
    painter.setFont(font);
    painter.setPen(m_textColor);
 
    // Vertically center text in the BAND area
    int textY = bandY + (bandHeight + m_fontSize) / 2 - 2;
 
    // PERFORMANCE: Calculate visible range
    int firstVisibleIdx = std::max(0, static_cast<int>(-textStartX / cw));
    int lastVisibleIdx = std::min(static_cast<int>(m_text.length()), 
                                 static_cast<int>((width() - textStartX) / cw) + 1);
 
    if (firstVisibleIdx < lastVisibleIdx) {
        QString visibleText = m_text.mid(firstVisibleIdx, lastVisibleIdx - firstVisibleIdx);
        painter.drawText(QPointF(textStartX + (firstVisibleIdx * cw), textY), visibleText);
    }
  }
 
  // 5. Draw Band Border (Layer 2)
  QPen borderPen(QColor(0, 120, 215), 2);
  painter.setPen(borderPen);
 
  if (m_visualStyle == Standalone) {
    painter.drawRect(bandRect);
  } else if (m_visualStyle == UnifiedTop) {
    painter.drawRect(bandRect);
  } else if (m_visualStyle == UnifiedBottom) {
    painter.drawRect(bandRect);
  }
 
  // 6. Draw Target Line (Guide)
  QPen targetPen(QColor(0, 120, 215), 2);
  targetPen.setStyle(Qt::DashLine);
  painter.setPen(targetPen);
  painter.drawLine(targetX, bandY, targetX, bandY + bandHeight);
 
  // 7. Draw Edit Cursor (Unified Blue Line)
  if (cw > 0) {
    int idx = cursorIndex();
    double cursorScreenX = textStartX + (idx * cw);
 
    bool drawHandle =
        (m_visualStyle == Standalone || m_visualStyle == UnifiedTop);
    bool drawLabel =
        (m_visualStyle == Standalone || m_visualStyle == UnifiedTop);
 
    // Line Geometry
    int lineTop = bandY;
    int lineBottom = bandY + bandHeight;
 
    // Adjust line for Unified look
    if (m_visualStyle == UnifiedTop)
      lineBottom += 2;
    if (m_visualStyle == UnifiedBottom)
      lineTop -= 2;
 
    // Vertical Line
    QPen cursorPen(QColor(0, 120, 215), 3);
    painter.setPen(cursorPen);
    painter.drawLine(cursorScreenX, lineTop, cursorScreenX, lineBottom);
 
    // Triangle Handle (In Header Area)
    if (drawHandle) {
      QPolygon tri;
      tri << QPoint(cursorScreenX, bandY)           // Point at thread (25)
          << QPoint(cursorScreenX - 5, bandY - 10)  // Left Top (15)
          << QPoint(cursorScreenX + 5, bandY - 10); // Right Top (15)
      painter.setBrush(QColor(0, 120, 215));
      painter.drawPolygon(tri);
    }
 
    if (drawLabel) {
      int mm = (m_currentPosition / 60000) % 60;
      int ss = (m_currentPosition / 1000) % 60;
      int ms = m_currentPosition % 1000;
      QString timeStr = QString("%1:%2.%3")
                            .arg(mm, 2, 10, QChar('0'))
                            .arg(ss, 2, 10, QChar('0'))
                            .arg(ms, 3, 10, QChar('0'));
 
      painter.setPen(QColor(34, 34, 34));
      QFont smallFont("Segoe UI", 8, QFont::Bold);
      painter.setFont(smallFont);
      int tw = painter.fontMetrics().horizontalAdvance(timeStr);
      painter.drawText(cursorScreenX - tw / 2, bandY - 12, timeStr);
    }
  }
}

// ============================================================================
// Mouse Events
// ============================================================================

void RythmoWidget::mousePressEvent(QMouseEvent *event) {
  if (event->button() != Qt::LeftButton)
    return;

  m_lastMouseX = event->pos().x();

  int targetX = width() / 5;
  int clickX = event->pos().x();

  // Pixel-perfect seek
  double timeDeltaMs = (double(clickX - targetX) * 1000.0) / m_speed;
  qint64 newTime = std::max(qint64(0), m_currentPosition + static_cast<qint64>(timeDeltaMs));

  // Visual Instant Feedback
  requestDebouncedSeek(newTime);
  setFocus();
}

void RythmoWidget::mouseMoveEvent(QMouseEvent *event) {
  if (!(event->buttons() & Qt::LeftButton))
    return;

  int currentX = event->pos().x();
  int deltaX = currentX - m_lastMouseX;
  m_lastMouseX = currentX;

  // Dragging logic
  double timeDeltaMs = (double(deltaX) * 1000.0) / m_speed;
  qint64 newTime = std::max(qint64(0), m_currentPosition - static_cast<qint64>(timeDeltaMs));

  sync(newTime);
  requestDebouncedSeek(newTime);
}

void RythmoWidget::mouseDoubleClickEvent(QMouseEvent *event) {
  mousePressEvent(event);
}

// ============================================================================
// Keyboard Events
// ============================================================================

void RythmoWidget::keyPressEvent(QKeyEvent *event) {
  qint64 step = charDurationMs();

  // Navigation
  if (event->key() == Qt::Key_Left) {
    qint64 newTime = std::max(qint64(0), m_currentPosition - step);
    requestDebouncedSeek(newTime);
    return;
  }
  if (event->key() == Qt::Key_Right) {
    qint64 newTime = m_currentPosition + step;
    requestDebouncedSeek(newTime);
    return;
  }

  // Escape: Insert space (push text) and play
  if (event->key() == Qt::Key_Escape) {
    int idx = cursorIndex();
    while (m_text.length() < idx) {
      m_text.append(' ');
    }
    m_text.insert(idx, ' ');
    qint64 newTime = m_currentPosition + step;
    requestDebouncedSeek(newTime);
    emit playRequested();
    return;
  }

  // Text Editing
  int idx = cursorIndex();

  if (event->key() == Qt::Key_Backspace) {
    if (idx > 0 && idx <= m_text.length()) {
      m_text.remove(idx - 1, 1);
      qint64 newTime = std::max(qint64(0), m_currentPosition - step);
      requestDebouncedSeek(newTime);
    }
    return;
  }

  if (event->key() == Qt::Key_Delete) {
    if (idx >= 0 && idx < m_text.length()) {
      m_text.remove(idx, 1);
      update();
    }
    return;
  }

  // Printable Characters
  if (!event->text().isEmpty() && event->text().at(0).isPrint()) {
    // Pad with spaces if needed
    while (m_text.length() < idx) {
      m_text.append(' ');
    }
    m_text.insert(idx, event->text());
    qint64 newTime = m_currentPosition + step;
    requestDebouncedSeek(newTime);
  }
}
