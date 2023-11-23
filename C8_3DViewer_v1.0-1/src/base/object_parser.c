#include "object_parser.h"

int parser(Model *obj, char *fp) {
  FILE *file = fopen(fp, "rb");
  int error = OK;
  if (file == NULL) {
    error = ERROR;
  } else {
    initializeStruct(obj);
    lineCounter(obj, file);
    if (!createArrayVertexesAndFacets(obj)) {
      error = flagsParser(obj, file);
    } else {
      cleanStructs(*obj);
    }
    fclose(file);
  }
  return error;
}

void cleanStructs(Model obj) {
  free(obj.facets_arr);
  free(obj.vertex_arr);
}

int createArrayVertexesAndFacets(Model *obj) {
  int error = OK;
  obj->facets_arr = (unsigned int *)calloc(obj->f_count, sizeof(unsigned int));
  if (obj->facets_arr == NULL) {
    return ERROR;
  }
  obj->vertex_arr = (double *)calloc(obj->v_count, sizeof(double));
  if (obj->vertex_arr == NULL) {
    return ERROR;
  }
  return error;
}

void initializeStruct(Model *obj) {
  obj->f_count = 0;
  obj->v_count = 0;
  obj->vertex_arr = 0;
  obj->facets_arr = 0;
}

int flagsParser(Model *obj, FILE *file) {
  int err = OK;
  char line[BUFFER_SIZE] = {'\0'};
  int idx = 0;
  unsigned int f_idx = 0;
  while (fgets(line, BUFFER_SIZE, file) != NULL && !err) {
    if (line[0] == 'v' && line[1] == ' ') {
      int valuesRead =
          sscanf(line, "v %lf %lf %lf", &obj->vertex_arr[idx],
                 &obj->vertex_arr[idx + 1], &obj->vertex_arr[idx + 2]);
      if (valuesRead != 3) {
        err = ERROR;
      }
      idx += 3;
    } else if (line[0] == 'f' && line[1] == ' ') {
      fLineParser(line, obj, &f_idx);
    }
  }
  setExtrema(obj);
  fseek(file, 0, SEEK_SET);
  return err;
}

void setExtrema(Model *obj) {
  if (obj->v_count <= 3) {
    obj->minMaxX[0] = obj->vertex_arr[0];
    obj->minMaxX[1] = obj->vertex_arr[0];
    obj->minMaxY[0] = obj->vertex_arr[1];
    obj->minMaxY[1] = obj->vertex_arr[1];
    obj->minMaxZ[0] = obj->vertex_arr[2];
    obj->minMaxZ[1] = obj->vertex_arr[2];
  } else {
    obj->minMaxX[0] = obj->minMaxX[1] = obj->vertex_arr[0];
    obj->minMaxY[0] = obj->minMaxY[1] = obj->vertex_arr[1];
    obj->minMaxZ[0] = obj->minMaxZ[1] = obj->vertex_arr[2];

    for (unsigned int i = 1; i < obj->v_count / 3; i++) {
      double elemX = obj->vertex_arr[i * 3];
      double elemY = obj->vertex_arr[i * 3 + 1];
      double elemZ = obj->vertex_arr[i * 3 + 2];

      if (elemX < obj->minMaxX[0]) {
        obj->minMaxX[0] = elemX;
      }
      if (elemX > obj->minMaxX[1]) {
        obj->minMaxX[1] = elemX;
      }
      if (elemY < obj->minMaxY[0]) {
        obj->minMaxY[0] = elemY;
      }
      if (elemY > obj->minMaxY[1]) {
        obj->minMaxY[1] = elemY;
      }
      if (elemZ < obj->minMaxZ[0]) {
        obj->minMaxZ[0] = elemZ;
      }
      if (elemZ > obj->minMaxZ[1]) {
        obj->minMaxZ[1] = elemZ;
      }
    }
  }
}

void fLineParser(char *line, Model *obj, unsigned int *idx) {
  int closure_val = '\0';
  int i_flag = 0;
  char *p_end = NULL;

  for (size_t i = 0; i < strlen(line); i++) {
    long int value = 0;

    if (line[i] == ' ' && (isdigit(line[i + 1]) || line[i + 1] == '-')) {
      i += 1;
      ++i_flag;

      char str[10] = {'\0'};
      int is_negative = 0;
      if (line[i] == '-') {
        is_negative = 1;
        i++;
      }
      int j = 0;
      while (isdigit(line[i])) {
        str[j] = line[i];
        i++;
        j++;
      }

      i--;
      value = strtol(str, &p_end, 10);
      if (is_negative) {
        value = -value;
      }

      if (value < 0) {
        value = (obj->v_count) / 3 + 1 + value;
      } else {
        value = value - 1;
      }

      obj->facets_arr[*idx] = value;

      if (i_flag == 1) {
        closure_val = value;
        ++(*idx);
      } else {
        obj->facets_arr[++(*idx)] = value;
        ++(*idx);
      }
    }
  }
  obj->facets_arr[(*idx)++] = closure_val;
}

void lineCounter(Model *obj, FILE *file) {
  char line[BUFFER_SIZE] = {'\0'};
  while (fgets(line, BUFFER_SIZE, file) != NULL) {
    if (line[0] == 'v' && line[1] == ' ') {
      obj->v_count += 3;
    } else if (line[0] == 'f' && line[1] == ' ') {
      for (size_t i = 0; i < strlen(line); i++) {
        if (line[i] == ' ' && (isdigit(line[i + 1]) || line[i + 1] == '-')) {
          obj->f_count++;
        }
      }
    }
  }
  obj->f_count *= 2;
  fseek(file, 0, SEEK_SET);
}
