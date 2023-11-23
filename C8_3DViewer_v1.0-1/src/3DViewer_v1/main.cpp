#include <QApplication>

#include "mainwindow.h"

int main(int argc, char *argv[]) {
  QApplication a(argc, argv);
  MainWindow mainWindow;
  mainWindow.setWindowTitle("3D Viewer v1.0");
  mainWindow.setGeometry(400, 200, 1080, 620);
  mainWindow.show();
  return a.exec();
}
