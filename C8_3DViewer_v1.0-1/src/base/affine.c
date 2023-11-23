#include "affine.h"

void center(Model *obj) {                                   // LCOV_EXCL_LINE
  double xShift = (obj->minMaxX[0] + obj->minMaxX[1]) / 2;  // LCOV_EXCL_LINE
  double yShift = (obj->minMaxY[0] + obj->minMaxY[1]) / 2;  // LCOV_EXCL_LINE
  double zShift = (obj->minMaxZ[0] + obj->minMaxZ[1]) / 2;  // LCOV_EXCL_LINE
  moveOX(obj, -xShift);                                     // LCOV_EXCL_LINE
  moveOY(obj, -yShift);                                     // LCOV_EXCL_LINE
  moveOZ(obj, -zShift);                                     // LCOV_EXCL_LINE
}

void moveOX(Model *obj, double value) {
  for (unsigned int i = 0; i < obj->v_count; i++) {
    if (i % 3 == 0) obj->vertex_arr[i] += value;
  }
}

void moveOY(Model *obj, double value) {
  for (unsigned int i = 0; i < obj->v_count; i++) {
    if (i % 3 == 1) obj->vertex_arr[i] += value;
  }
}

void moveOZ(Model *obj, double value) {
  for (unsigned int i = 0; i < obj->v_count; i++) {
    if (i % 3 == 2) obj->vertex_arr[i] += value;
  }
}

// LCOV_EXCL_START
void normalize_size(Model *obj) {
  double scope = 1;
  scope = fabs(obj->minMaxX[0]);
  if (fabs(obj->minMaxX[1]) > scope) scope = obj->minMaxX[1];
  if (fabs(obj->minMaxY[0]) > scope) scope = obj->minMaxY[0];
  if (fabs(obj->minMaxY[1]) > scope) scope = obj->minMaxY[1];
  if (fabs(obj->minMaxZ[0]) > scope) scope = obj->minMaxZ[0];
  if (fabs(obj->minMaxZ[1]) > scope) scope = obj->minMaxZ[1];
  scale(obj, 2.5 / scope);
}

void scale(Model *obj, double value) {
  if (value == 0) value = 1;
  for (unsigned int i = 0; i < obj->v_count; i++) {
    obj->vertex_arr[i] *= value;
  }
  obj->minMaxX[0] *= value;
  obj->minMaxX[1] *= value;
  obj->minMaxY[0] *= value;
  obj->minMaxY[1] *= value;
  obj->minMaxZ[0] *= value;
  obj->minMaxZ[1] *= value;
}

void rotateOX(Model *obj, double angle) {
  for (unsigned int i = 0; i < obj->v_count - 2; i += 3) {
    double temp_y = obj->vertex_arr[i + 1];
    double temp_z = obj->vertex_arr[i + 2];
    obj->vertex_arr[i + 1] = (cos(angle) * temp_y - sin(angle) * temp_z);
    obj->vertex_arr[i + 2] = (sin(angle) * temp_y + cos(angle) * temp_z);
  }
}

void rotateOY(Model *obj, double angle) {
  for (unsigned int i = 1; i < obj->v_count - 1; i += 3) {
    double temp_x = obj->vertex_arr[i - 1];
    double temp_z = obj->vertex_arr[i + 1];
    obj->vertex_arr[i - 1] = (cos(angle) * temp_x - sin(angle) * temp_z);
    obj->vertex_arr[i + 1] = (sin(angle) * temp_x + cos(angle) * temp_z);
  }
}

void rotateOZ(Model *obj, double angle) {
  for (unsigned int i = 2; i < obj->v_count; i += 3) {
    double temp_x = obj->vertex_arr[i - 2];
    double temp_y = obj->vertex_arr[i - 1];
    obj->vertex_arr[i - 2] = (cos(angle) * temp_x - sin(angle) * temp_y);
    obj->vertex_arr[i - 1] = (sin(angle) * temp_x + cos(angle) * temp_y);
  }
}
// LCOV_EXCL_STOP
