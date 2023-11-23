#include "mainwindow.h"

#include <QColorDialog>
#include <QTimer>
#include <vector>

#include "glview.h"
#include "ui_mainwindow.h"

extern "C" {
#include "../base/affine.h"
#include "../base/object_parser.h"
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
  ui->setupUi(this);

  settings = new QSettings("settings.ini", QSettings::IniFormat, this);
  setFixedSize(QSize(1080, 620));
  openGLWidget = new glview(this);
  openGLWidget->move(240, 10);
  openGLWidget->resize(600, 580);
  openGLWidget->show();

  ui->spinBoxLineWidth->setValue(1.0f);

  connect(openGLWidget, &glview::signalTextToForm, this,
          &MainWindow::slotTextFromGlView);

  loadSettings();

  count = 0;
  connect(openGLWidget, &glview::signalTextToForm, this,
          &MainWindow::slotTextFromGlView);

  joyMove();
  ui->moveRadioButton->setChecked(1);
}

MainWindow::~MainWindow() {
  saveSettings();
  cleanStructs(openGLWidget->model);
  delete ui;
}

void MainWindow::slotTextFromGlView(const QString &message) {
  ui->statusBar->showMessage(message);
}

void MainWindow::setBGColor(QString color_hex_string) {
  QColor color(color_hex_string);
  openGLWidget->bg_red = color.redF();
  openGLWidget->bg_green = color.greenF();
  openGLWidget->bg_blue = color.blueF();
  openGLWidget->bg_alpha = color.alphaF();
}

void MainWindow::setLineColor(QString color_hex_string) {
  QColor color(color_hex_string);
  openGLWidget->line_red = color.redF();
  openGLWidget->line_green = color.greenF();
  openGLWidget->line_blue = color.blueF();
  openGLWidget->line_alpha = color.alphaF();
}

void MainWindow::setPointColor(QString color_hex_string) {
  QColor color(color_hex_string);
  openGLWidget->point_red = color.redF();
  openGLWidget->point_green = color.greenF();
  openGLWidget->point_blue = color.blueF();
  openGLWidget->point_alpha = color.alphaF();
}

bool MainWindow::choiceScreenFolder() {
  bool error = 0;
  screenFolder = QFileDialog::getExistingDirectory(
      this, tr("Open Directory"), "~/Users/ziggsdil/Desktop",
      QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
  if (screenFolder.isEmpty() && screenFolder.isNull())
    error = 1;
  else
    ui->statusBar->showMessage(screenFolder, 3000);

  return error;
}

void MainWindow::loadSettings() {
  openGLWidget->ortho = settings->value("ortho", "1").toInt();
  ui->boxOrtho->setChecked(openGLWidget->ortho);

  openGLWidget->draw_style = settings->value("draw_style", "0").toInt();
  switch (openGLWidget->draw_style) {
    case 0:
      ui->solidRadioButton->setChecked(1);
      break;
    case 1:
      ui->strippleRadioButton->setChecked(1);
      break;
    case 2:
      ui->pointRadioButton->setChecked(1);
      break;
    default:
      break;
  }

  openGLWidget->background_color =
      settings->value("background_color", "#000000").toString();
  ui->frameBFColor->setStyleSheet(
      "background-color: " + openGLWidget->background_color + ";");
  setBGColor(openGLWidget->background_color);

  openGLWidget->line_width = settings->value("line_width", "0").toInt();
  ui->spinBoxLineWidth->setValue(openGLWidget->line_width);
  openGLWidget->line_color =
      settings->value("line_color", "#ffffff").toString();
  ui->frameLineColor->setStyleSheet(
      "background-color: " + openGLWidget->line_color + ";");
  setLineColor(openGLWidget->line_color);

  openGLWidget->point_color =
      settings->value("point_color", "#ffffff").toString();
  ui->framePointColor->setStyleSheet(
      "background-color: " + openGLWidget->point_color + ";");
  setPointColor(openGLWidget->point_color);

  openGLWidget->point_width = settings->value("point_width", "0").toInt();
  ui->spinBoxPointWidth->setValue(openGLWidget->point_width);
  openGLWidget->point_round = settings->value("point_round", "0").toInt();
  ui->checkBoxRound->setChecked(openGLWidget->point_round);

  openGLWidget->joy_step = settings->value("joy_step", 100).toInt();
  openGLWidget->update();
}

void MainWindow::saveSettings() {
  settings->setValue("ortho", openGLWidget->ortho);
  settings->setValue("draw_style", openGLWidget->draw_style);
  settings->setValue("background_color", openGLWidget->background_color);

  settings->setValue("line_width", openGLWidget->line_width);
  settings->setValue("line_color", openGLWidget->line_color);

  settings->setValue("point_color", openGLWidget->point_color);
  settings->setValue("point_width", openGLWidget->point_width);
  settings->setValue("point_round", openGLWidget->point_round);
  settings->setValue("joy_step", openGLWidget->joy_step);
}

void MainWindow::on_boxOrtho_stateChanged(int arg1) {
  openGLWidget->ortho = arg1;
  openGLWidget->update();
}

void MainWindow::on_solidRadioButton_toggled(bool checked) {
  openGLWidget->draw_style = 0;
  if (checked) ui->statusBar->showMessage("SOLID", 3000);
  openGLWidget->update();
}

void MainWindow::on_strippleRadioButton_toggled(bool checked) {
  openGLWidget->draw_style = 1;
  if (checked) ui->statusBar->showMessage("STRIPPLE", 3000);
  openGLWidget->update();
}

void MainWindow::on_pointRadioButton_toggled(bool checked) {
  openGLWidget->draw_style = 2;
  if (checked) ui->statusBar->showMessage("POINTS", 3000);
  openGLWidget->update();
}

void MainWindow::on_checkBoxRound_stateChanged(int arg1) {
  openGLWidget->point_round = arg1;
  ui->statusBar->showMessage("Point rounded: " + QString::number(arg1), 3000);
  openGLWidget->update();
}

void MainWindow::on_moveRadioButton_toggled(bool checked) {
  if (checked) {
    joyMove();
  }
}

void MainWindow::on_rotateRadioButton_toggled(bool checked) {
  if (checked) {
    joyRotate();
  }
}

void MainWindow::on_zoomRadioButton_toggled(bool checked) {
  if (checked) {
    joyZoom();
  }
}

void MainWindow::joyMove() {
  ui->upPushButton->setEnabled(true);
  ui->downPushButton->setEnabled(true);
  ui->rightPushButton->setEnabled(true);
  ui->leftPushButton->setEnabled(true);
  ui->cwPushButton->setEnabled(true);
  ui->acwPushButton->setEnabled(true);
  ui->upPushButton->setText("Up");
  ui->downPushButton->setText("Down");
  ui->rightPushButton->setText("Right");
  ui->leftPushButton->setText("left");
  ui->cwPushButton->setText("CW");
  ui->acwPushButton->setText("ACW");
}

void MainWindow::joyRotate() {
  ui->upPushButton->setEnabled(true);
  ui->downPushButton->setEnabled(true);
  ui->rightPushButton->setEnabled(true);
  ui->leftPushButton->setEnabled(true);
  ui->cwPushButton->setEnabled(true);
  ui->acwPushButton->setEnabled(true);
  ui->upPushButton->setText("Up");
  ui->downPushButton->setText("Down");
  ui->rightPushButton->setText("Right");
  ui->leftPushButton->setText("left");
  ui->cwPushButton->setText("CW");
  ui->acwPushButton->setText("ACW");
}

void MainWindow::joyZoom() {
  ui->rightPushButton->setEnabled(false);
  ui->leftPushButton->setEnabled(false);
  ui->cwPushButton->setEnabled(false);
  ui->acwPushButton->setEnabled(false);
  ui->upPushButton->setText("incr");
  ui->downPushButton->setText("decreas");
  ui->rightPushButton->setText("");
  ui->leftPushButton->setText("");
  ui->cwPushButton->setText("");
  ui->acwPushButton->setText("");
}

void MainWindow::on_open_clicked() {
  QString filepath =
      QFileDialog::getOpenFileName(this, "Choose a file", "/", "*.obj");
  if (!filepath.isEmpty()) {
    QString fileName = QFileInfo(filepath).fileName();
    QByteArray ba = filepath.toLocal8Bit();
    openGLWidget->open_file(ba.data());
    ui->File_name->setText(fileName);
    ui->num_vertices->setText(QString::number(openGLWidget->vert_size / 3));
    ui->num_edges->setText(QString::number(openGLWidget->polygons_size / 2));
    ui->statusBar->showMessage(
        filepath + " [vertex: " + QString::number(openGLWidget->vert_size) +
        " poligons: " + QString::number(openGLWidget->polygons_size) + "]");
  }
}

void MainWindow::on_buttonBGColor_clicked() {
  QColor color = QColorDialog::getColor(Qt::white, this);
  if (color.isValid()) {
    openGLWidget->background_color = color.name();
    setBGColor(openGLWidget->background_color);
    ui->statusBar->showMessage(
        "Background color: " + openGLWidget->background_color, 3000);
    ui->frameBFColor->setStyleSheet(
        "background-color: " + openGLWidget->background_color + ";");
  }
}

void MainWindow::on_buttonLineColor_clicked() {
  QColor color = QColorDialog::getColor(Qt::white, this);
  if (color.isValid()) {
    openGLWidget->line_color = color.name();
    setLineColor(openGLWidget->line_color);
    ui->statusBar->showMessage("Line color: " + openGLWidget->line_color, 3000);
    ui->frameLineColor->setStyleSheet(
        "background-color: " + openGLWidget->line_color + ";");
  }
}

void MainWindow::on_buttonPointColor_clicked() {
  QColor color = QColorDialog::getColor(Qt::white, this);
  if (color.isValid()) {
    openGLWidget->point_color = color.name();
    setPointColor(openGLWidget->point_color);
    ui->statusBar->showMessage("Point color: " + openGLWidget->point_color,
                               3000);
    ui->framePointColor->setStyleSheet(
        "background-color: " + openGLWidget->point_color + ";");
  }
}

void MainWindow::on_saveAsJpeg_clicked() {
  if (!choiceScreenFolder()) {
    QPixmap pixmap = openGLWidget->grab();
    QDateTime date = QDateTime::currentDateTime();
    screenTime = date.toString("yyyy_MM_dd_hh_mm_ss");
    QFile file(screenFolder + "/Screen_Shot_" + screenTime + ".jpeg");
    file.open(QIODevice::WriteOnly);
    pixmap.save(&file, "JPEG");
  }
}

void MainWindow::on_saveAsBmp_clicked() {
  if (!choiceScreenFolder()) {
    QPixmap pixmap = openGLWidget->grab();
    QDateTime date = QDateTime::currentDateTime();
    screenTime = date.toString("yyyy_MM_dd_hh_mm_ss");
    QFile file(screenFolder + "/Screen_Shot_" + screenTime + ".bmp");
    file.open(QIODevice::WriteOnly);
    pixmap.save(&file, "BMP");
  }
}

void MainWindow::on_upPushButton_clicked() {
  if (openGLWidget->polygons_size != 0) {
    if (ui->moveRadioButton->isChecked()) {
      moveOY(&(openGLWidget->model), K * openGLWidget->joy_step);
      openGLWidget->update();
    } else if (ui->rotateRadioButton->isChecked()) {
      rotateOX(&(openGLWidget->model), -K * openGLWidget->joy_step);
      openGLWidget->update();
    } else if (ui->zoomRadioButton->isChecked()) {
      scale(&(openGLWidget->model), 1 + K * openGLWidget->joy_step);
      openGLWidget->update();
    }
  }
}

void MainWindow::on_downPushButton_clicked() {
  if (openGLWidget->polygons_size != 0) {
    if (ui->moveRadioButton->isChecked()) {
      moveOY(&(openGLWidget->model), -K * openGLWidget->joy_step);
      openGLWidget->update();
    } else if (ui->rotateRadioButton->isChecked()) {
      rotateOX(&(openGLWidget->model), K * openGLWidget->joy_step);
      openGLWidget->update();
    } else if (ui->zoomRadioButton->isChecked()) {
      scale(&(openGLWidget->model), 1 - K * openGLWidget->joy_step);
      openGLWidget->update();
    }
  }
}

void MainWindow::on_leftPushButton_clicked() {
  if (openGLWidget->polygons_size != 0) {
    if (ui->moveRadioButton->isChecked()) {
      moveOX(&(openGLWidget->model), -K * openGLWidget->joy_step);
      openGLWidget->update();
    } else if (ui->rotateRadioButton->isChecked()) {
      rotateOY(&(openGLWidget->model), K * openGLWidget->joy_step);
      openGLWidget->update();
    }
  }
}

void MainWindow::on_rightPushButton_clicked() {
  if (openGLWidget->polygons_size != 0) {
    if (ui->moveRadioButton->isChecked()) {
      moveOX(&(openGLWidget->model), K * openGLWidget->joy_step);
      openGLWidget->update();
    } else if (ui->rotateRadioButton->isChecked()) {
      rotateOY(&(openGLWidget->model), -K * openGLWidget->joy_step);
      openGLWidget->update();
    }
  }
}

void MainWindow::on_cwPushButton_clicked() {
  if (openGLWidget->polygons_size != 0) {
    if (ui->moveRadioButton->isChecked()) {
      moveOZ(&(openGLWidget->model), K * openGLWidget->joy_step);
      openGLWidget->update();
    } else if (ui->rotateRadioButton->isChecked()) {
      rotateOZ(&(openGLWidget->model), -K * openGLWidget->joy_step);
      openGLWidget->update();
    }
  }
}

void MainWindow::on_acwPushButton_clicked() {
  if (openGLWidget->polygons_size != 0) {
    if (ui->moveRadioButton->isChecked()) {
      moveOZ(&(openGLWidget->model), -K * openGLWidget->joy_step);
      openGLWidget->update();
    } else if (ui->rotateRadioButton->isChecked()) {
      rotateOZ(&(openGLWidget->model), K * openGLWidget->joy_step);
      openGLWidget->update();
    }
  }
}

void MainWindow::on_spinBoxLineWidth_valueChanged(int arg1) {
  openGLWidget->line_width = arg1;
  ui->statusBar->showMessage("Line width: " + QString::number(arg1), 3000);
  openGLWidget->update();
}

void MainWindow::on_spinBoxPointWidth_valueChanged(int arg1) {
  openGLWidget->point_width = arg1;
  ui->statusBar->showMessage("Point width: " + QString::number(arg1), 3000);
  openGLWidget->update();
}

void MainWindow::on_saveAsGif_clicked() {
  qDebug() << "push GIF";
  QString filter = "*.gif";
  file_name = "/0";
  file_name = QFileDialog::getSaveFileName(this, "Сохранить GIF",
                                           QDir::homePath(), filter);
  ptr_save_file = new QFile(file_name);
  if (!ptr_save_file->open(QIODevice::WriteOnly)) {
    QMessageBox::critical(this, "Warning", "Не удалось записать файл");
  } else {
    start_timer_gif();
  }
}

void MainWindow::start_timer_gif() {
  gif = new QGifImage(QSize(1200, 1160));
  timerGIF = new QTimer(this);
  connect(timerGIF, &QTimer::timeout, this, &MainWindow::make_gif);
  timerGIF->start(100);
  frame = 0;
}

void MainWindow::make_gif() {
  qDebug() << openGLWidget->width();
  qDebug() << openGLWidget->height();
  const QRect rect(0, 0, 600, 580);
  QPixmap pixmap = openGLWidget->grab(rect);
  qDebug() << "pixmap = " << pixmap;
  QImage myImage = pixmap.toImage();
  frame++;
  gif->addFrame(myImage, 100);
  if (frame == 50) {
    timerGIF->stop();
    gif->save(ptr_save_file);
    ptr_save_file->close();
    delete ptr_save_file;
    ptr_save_file = nullptr;
    delete gif;
    gif = nullptr;
    QMessageBox::about(this, "Статус", "GIF сохранена");
  }
}
