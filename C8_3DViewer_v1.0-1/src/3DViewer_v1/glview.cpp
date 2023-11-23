#define GL_SILENCE_DEPRECATION
#include "glview.h"

glview::glview(QWidget *parent) : QOpenGLWidget(parent) {
  model.f_count = 0;
  model.v_count = 0;
  model.facets_arr = nullptr;
  model.vertex_arr = nullptr;
  polygons_size = 0;
  vert_size = 0;

  left = -1.0 * WIDTH / HEIGHT;
  right = 1.0 * WIDTH / HEIGHT;
  bottom = -1.0;
  top = 1.0;

  draw_style = 0;
  line_width = 0;
  point_width = 0;
  point_round = 0;

  joy_step = 1;

  background_color = "#000000";
  bg_red = 0;
  bg_green = 0;
  bg_blue = 0;
  bg_alpha = 0;

  line_color = "#ffffff";
  line_red = 0;
  line_green = 0;
  line_blue = 0;
  line_alpha = 0;

  point_color = "#ffffff";
  point_red = 0;
  point_green = 0;
  point_blue = 0;
  point_alpha = 0;
}

void glview::initializeGL() {
  glEnable(GL_DEPTH_TEST);
  glLineStipple(1, 0x00FF);
}

void glview::resizeGL(int width, int height) {
  glViewport(0, 0, width, height);
}

void glview::paintGL() {
  glClearColor(bg_red, bg_green, bg_blue, bg_alpha);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();

  if (ortho)
    glOrtho(left * K_ORTHO, right * K_ORTHO, bottom * K_ORTHO, top * K_ORTHO,
            -100.0, 100.0);
  else
    glFrustum(left, right, bottom, top, 1, 100.0);

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  glTranslatef(0, 0, -3);

  if (draw_style == 2) {
    drawPointView();
  } else {
    drawLineView();
  }
}

void glview::mousePressEvent(QMouseEvent *mouseEvent) {
  lastMousePosition = mouseEvent->pos();
  if (mouseEvent->button() == Qt::RightButton) {
    updateStatusBarText(" [right] ");
    update();
  }
}

void glview::mouseMoveEvent(QMouseEvent *mouseEvent) {
  if (mouseEvent->buttons() == Qt::LeftButton) {
    updateStatusBarText(" [rotate] " + QString::number(mouseEvent->pos().x()) +
                        ":" + QString::number(mouseEvent->pos().y()));

    rotateOX(&model, K * static_cast<double>(mouseEvent->pos().y() -
                                             lastMousePosition.y()));
    rotateOY(&model, K * static_cast<double>(lastMousePosition.x() -
                                             mouseEvent->pos().x()));
    update();
  } else if (mouseEvent->buttons() == Qt::MiddleButton) {
    updateStatusBarText(" [move] " + QString::number(mouseEvent->pos().x()) +
                        ":" + QString::number(mouseEvent->pos().y()));

    moveOX(&model, 10 * K *
                       static_cast<double>(mouseEvent->pos().x() -
                                           lastMousePosition.x()));
    moveOY(&model, 10 * K *
                       static_cast<double>(lastMousePosition.y() -
                                           mouseEvent->pos().y()));
  }

  lastMousePosition = mouseEvent->pos();
  update();
}

void glview::wheelEvent(QWheelEvent *wheelEvent) {
  QString str;
  int step = wheelEvent->angleDelta().y();
  if (step > 0) {
    str = "In";
    scale(&model, 1.1);
  } else {
    str = "Out";
    scale(&model, 0.9);
  }
  updateStatusBarText("Zoom " + str + " [" + QString::number(step) + "]");
  update();
}

void glview::updateStatusBarText(QString text) { emit signalTextToForm(text); }

void glview::open_file(char *filename) {
  if (model.facets_arr || model.vertex_arr) {
    cleanStructs(model);
    model.f_count = 0;
    model.v_count = 0;
    model.facets_arr = nullptr;
    model.vertex_arr = nullptr;
  }

  if (!parser(&model, filename)) {
    center(&model);
    normalize_size(&model);
    vert_size = model.v_count;
    polygons_size = model.f_count;
  } else {
    QMessageBox::critical(this, "", "File open error.");
  }
}

void glview::drawLineView() {
  glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

  if (draw_style == 1) glEnable(GL_LINE_STIPPLE);

  GLfloat color[] = {line_red, line_green, line_blue, line_alpha};
  glColor4fv(color);
  glLineWidth(line_width);

  glVertexPointer(3, GL_DOUBLE, 0, model.vertex_arr);
  glEnableClientState(GL_VERTEX_ARRAY);

  if (model.f_count != 0 && model.facets_arr != NULL) {
    glDrawElements(GL_LINES, model.f_count, GL_UNSIGNED_INT, model.facets_arr);
  }

  glDisable(GL_LINE_STIPPLE);
}

void glview::drawPointView() {
  glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);

  if (point_round) {
    glDisable(GL_POINT);
    glEnable(GL_POINT_SMOOTH);
    glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);
  } else {
    glDisable(GL_POINT_SMOOTH);
    glEnable(GL_POINT);
  }

  glPointSize(point_width);

  GLfloat color[] = {point_red, point_green, point_blue, point_alpha};
  glColor4fv(color);

  glVertexPointer(3, GL_DOUBLE, 0, model.vertex_arr);
  glEnableClientState(GL_VERTEX_ARRAY);

  glDrawElements(GL_POINTS, model.f_count, GL_UNSIGNED_INT, model.facets_arr);

  glDisable(GL_VERTEX_PROGRAM_POINT_SIZE);
  glDisable(GL_POINT_SMOOTH);
  glDisable(GL_POINT);
}
