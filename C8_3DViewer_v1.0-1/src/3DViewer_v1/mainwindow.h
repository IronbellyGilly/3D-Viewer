#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <qgifimage.h>

#include <QColorDialog>
#include <QFileDialog>
#include <QMainWindow>
#include <QSettings>

#include "glview.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
  Q_OBJECT

 public:
  MainWindow(QWidget *parent = nullptr);
  ~MainWindow();
  void saveSettings();
  void loadSettings();

 private:
  Ui::MainWindow *ui;
  glview *openGLWidget;
  QSettings *settings;
  QColor color;

  QTimer *timer;
  QImage frames[50];
  QString screenFolder;
  QString screenTime;
  QString gif_name;
  QGifImage *gif;
  QString file_name;
  QTimer *timerGIF;  // идентификатор таймера
  QFile *ptr_save_file;
  int frame;
  int count;

  bool choiceScreenFolder();
  void joyMove();
  void joyRotate();
  void joyZoom();

 public slots:
  void slotTextFromGlView(const QString &message);

 private slots:
  void setBGColor(QString color_hex_string);
  void setLineColor(QString color_hex_string);
  void setPointColor(QString color_hex_string);

  void on_open_clicked();
  // void on_saveAs_clicked();
  void on_saveAsJpeg_clicked();
  void on_saveAsBmp_clicked();

  void on_boxOrtho_stateChanged(int arg1);
  void on_solidRadioButton_toggled(bool checked);
  void on_strippleRadioButton_toggled(bool checked);
  void on_spinBoxLineWidth_valueChanged(int arg1);
  void on_buttonBGColor_clicked();
  void on_buttonLineColor_clicked();
  void on_buttonPointColor_clicked();
  void on_spinBoxPointWidth_valueChanged(int arg1);
  void on_pointRadioButton_toggled(bool checked);
  void on_checkBoxRound_stateChanged(int arg1);
  void on_moveRadioButton_toggled(bool checked);
  void on_rotateRadioButton_toggled(bool checked);
  void on_zoomRadioButton_toggled(bool checked);

  void on_upPushButton_clicked();
  void on_downPushButton_clicked();
  void on_leftPushButton_clicked();
  void on_rightPushButton_clicked();

  void on_cwPushButton_clicked();
  void on_acwPushButton_clicked();

  void on_saveAsGif_clicked();
  void start_timer_gif();
  void make_gif();
};
#endif  // MAINWINDOW_H
