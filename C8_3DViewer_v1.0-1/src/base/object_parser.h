//
// Created by Ziggs Dill on 8/31/23.
//

#ifndef C8_3DVIEWER_V1_0_1_OBJECT_PARSER_H
#define C8_3DVIEWER_V1_0_1_OBJECT_PARSER_H

#include <ctype.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFFER_SIZE 1024
#define ERROR 1
#define OK 0

typedef struct Model {
  unsigned int f_count;
  unsigned int v_count;
  double *vertex_arr;
  unsigned int *facets_arr;
  double minMaxX[2];
  double minMaxY[2];
  double minMaxZ[2];
} Model;

int parser(Model *obj, char *fp);
int flagsParser(Model *obj, FILE *file);
void lineCounter(Model *obj, FILE *file);
void fLineParser(char *line, Model *obj, unsigned int *idx);
void initializeStruct(Model *obj);
void setExtrema(Model *obj);
void cleanStructs(Model obj);

int createArrayVertexesAndFacets(Model *obj);

#endif  // C8_3DVIEWER_V1_0_1_OBJECT_PARSER_H
