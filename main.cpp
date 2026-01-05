#include "MainWindow.h"
#include <QApplication>

#include "MainWindow.h"
#include <QApplication>

#include <QStyleFactory>

int main(int argc, char *argv[]) {
  QApplication a(argc, argv);

  // 1. Set Style to Fusion (Good cross-platform base for custom coloring)
  a.setStyle(QStyleFactory::create("Fusion"));

  // 2. Define Dark Palette (DaVinci Resolve inspired)
  QPalette darkPalette;
  darkPalette.setColor(QPalette::Window, QColor(45, 45, 45));
  darkPalette.setColor(QPalette::WindowText, QColor(220, 220, 220));
  darkPalette.setColor(QPalette::Base, QColor(30, 30, 30));
  darkPalette.setColor(QPalette::AlternateBase, QColor(45, 45, 45));
  darkPalette.setColor(QPalette::ToolTipBase, QColor(220, 220, 220));
  darkPalette.setColor(QPalette::ToolTipText, QColor(45, 45, 45));
  darkPalette.setColor(QPalette::Text, QColor(220, 220, 220));
  darkPalette.setColor(QPalette::Button, QColor(45, 45, 45));
  darkPalette.setColor(QPalette::ButtonText, QColor(220, 220, 220));
  darkPalette.setColor(QPalette::BrightText, Qt::red);
  darkPalette.setColor(QPalette::Link, QColor(42, 130, 218));
  darkPalette.setColor(QPalette::Highlight,
                       QColor(255, 140, 0)); // Orange Accent
  darkPalette.setColor(QPalette::HighlightedText, Qt::black);

  a.setPalette(darkPalette);

  // 3. Apply Stylesheet for specific controls (Sliders, Buttons, Inputs)
  a.setStyleSheet(
      "QToolTip { color: #ffffff; background-color: #2a82da; border: 1px solid "
      "white; }"
      "QMainWindow { background-color: #2d2d2d; }"
      "QWidget { font-family: 'Segoe UI', 'Roboto', sans-serif; font-size: "
      "10pt; }"

      // GroupBoxes & Panels
      "QGroupBox { border: 1px solid #555; margin-top: 1ex; border-radius: "
      "4px; }"
      "QGroupBox::title { subcontrol-origin: margin; subcontrol-position: top "
      "center; padding: 0 3px; color: #aaa; }"

      // Buttons
      "QPushButton { background-color: #3e3e3e; border: 1px solid #555; "
      "border-radius: 4px; padding: 5px 15px; color: #ddd; }"
      "QPushButton:hover { background-color: #4e4e4e; border-color: #777; }"
      "QPushButton:pressed { background-color: #e57e00; color: black; "
      "border-color: #e57e00; }"
      "QPushButton:disabled { background-color: #2d2d2d; color: #777; "
      "border-color: #444; }"

      // LineEdit & SpinBox
      "QLineEdit, QSpinBox { background-color: #1e1e1e; border: 1px solid "
      "#444; border-radius: 3px; padding: 3px; color: #eee; "
      "selection-background-color: #e57e00; }"
      "QLineEdit:focus, QSpinBox:focus { border: 1px solid #e57e00; }"

      // Sliders (Pro Look)
      "QSlider::groove:horizontal { border: 1px solid #333; height: 6px; "
      "background: #1e1e1e; margin: 2px 0; border-radius: 3px; }"
      "QSlider::handle:horizontal { background: #b0b0b0; border: 1px solid "
      "#5c5c5c; width: 14px; height: 14px; margin: -5px 0; border-radius: 7px; "
      "}"
      "QSlider::handle:horizontal:hover { background: #e57e00; border-color: "
      "#e57e00; }"
      "QSlider::sub-page:horizontal { background: #555; border-radius: 3px; }"

      // ProgressBar
      "QProgressBar { border: 1px solid #444; border-radius: 3px; text-align: "
      "center; background: #1e1e1e; }"
      "QProgressBar::chunk { background-color: #e57e00; width: 10px; }"

      // Labels
      "QLabel { color: #ccc; }");

  MainWindow w;
  w.show();

  return a.exec();
}
