#ifndef SRC_BASE_AFFINE_H
#define SRC_BASE_AFFINE_H

#include "object_parser.h"

void center(Model *obj);
void moveOX(Model *obj, double value);
void moveOY(Model *obj, double value);
void moveOZ(Model *obj, double value);
void normalize_size(Model *obj);
void scale(Model *obj, double value);
void rotateOX(Model *obj, double angle);
void rotateOY(Model *obj, double angle);
void rotateOZ(Model *obj, double angle);

#endif  // SRC_BASE_AFFINE_H
