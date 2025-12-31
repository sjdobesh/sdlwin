/* sort.c
 * pixel sorting module for pxl library
 * samantha jane
 *--------------------------------------------------------------------------80*/
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "sort.h"

#define PI 3.1415926535

#define pixel_ptr_from_int(x) \
  &(pixel) {x, x, x, 255} \

#define pixel_ptr_from_float(x) \
  &(pixel) {x * 255, x * 255, x * 255, 255} \


int compare_brightness(const void* a, const void* b) {
  pixel* p_a = (pixel*)a;
  pixel* p_b = (pixel*)b;
  return ((p_a->r + p_a->g + p_a->b) > (p_b->r + p_b->g + p_b->b));
}

int threshold_brightness(pixel p, float t) {
  return (p.r + p.g +p.b) > (t * 255.0 * 3.0);
}

int threshold_difference(pixel a, pixel b, float t) {
  return (abs(a.r - b.r) + abs(a.g - b.g) + abs(a.b - b.b) > (t * 255.0 * 3.0));
}

/* sort a pixel bufffer */
void sort(pixel* p, int length, int (*compare)(const void*, const void*)) {
  qsort(p, length, sizeof(pixel), compare);
}

/**
 * sort orthogonally according to an info struct
 * @see sort_info
 **/
void orthogonal_sort(sort_info si) {

  pixel* row;
  int rows, row_size;

  /* determine sort direction */
  if (si.vertical_sort) {
    rows = si.img_ref->width;
    row_size = si.img_ref->height;
    row = malloc(si.img_ref->height * sizeof(pixel));
  } else {
    rows = si.img_ref->height;
    row_size = si.img_ref->width;
    row = malloc(si.img_ref->width * sizeof(pixel));
  }

  /* process each row */
  for (int i = 0; i < rows; i++) {
    /* collect a horizontal row */
    for (int j = 0; j < row_size; j++)
      row[j] = si.vertical_sort
        ? si.img_ref->pixels[j][i]
        : si.img_ref->pixels[i][j];

    /* iteratively sort subsections using threshold */
    int start = 0;
    int count = 0;
    for (int j = 0; j < row_size; j++) {
      if (start)
        count++;

      if (
        !start
        && j < row_size - 1
        && (si.random_start ? !(rand() % si.random_start) : 0)
        && threshold_difference(row[j], row[j+1], si.start_threshold)
      ) {
        start = j;
      }

      if (
        ( /* must terminate */
          start
          && (
            j == row_size - 1
            || (count >= si.max_length)
          )
        ) || ( /* chance terminate */
          start
          && count > si.min_length
          && (
            (!(si.random_stop ? (rand() % si.random_stop) : 1))
            || threshold_difference(row[j], row[j+1], si.stop_threshold)
          )
        )
      ) {
        sort(&row[start], count, si.comparison_function);
        start = 0; count = 0;
      }
    }

    /* replace */
    for (int j = 0; j < row_size; j++)
      si.img_ref->pixels[si.vertical_sort?j:i][si.vertical_sort?i:j] = row[j];

  }
  free(row);
  return;
}


void fuck_sort(sort_info si) {

  pixel* row;
  int rows, row_size;

  /* determine sort direction */
  if (si.vertical_sort) {
    rows = si.img_ref->width;
    row_size = si.img_ref->height;
    row = malloc(si.img_ref->height * sizeof(pixel));
  } else {
    rows = si.img_ref->height;
    row_size = si.img_ref->width;
    row = malloc(si.img_ref->width * sizeof(pixel));
  }

  /* process each row */
  for (int i = 0; i < rows; i++) {
    /* collect a horizontal row */
    for (int j = 0; j < row_size; j++)
      row[j] = si.vertical_sort
        ? si.img_ref->pixels[j][i]
        : si.img_ref->pixels[i][j];

    /* iteratively sort subsections using threshold */
    int start = 0;
    int count = 0;
    for (int j = 0; j < row_size; j++) {
      if (start)
        count++;

      if (
        !start
        && j < row_size - 1
        && (si.random_start ? !(rand() % si.random_start) : 1)
        && threshold_difference(row[j], row[j+1], si.start_threshold)
      ) {
        start = j;
      }

      if (
        ( /* must terminate */
          start
          && (
            j == row_size - 1
            || (count >= si.max_length)
          )
        ) || ( /* chance terminate */
          start
          && count > si.min_length
          && (
            (si.random_stop ? !(rand() % si.random_stop) : 1)
            || threshold_difference(row[j], row[j+1], si.stop_threshold)
          )
        )
      ) {
        sort(&row[start], count, si.comparison_function);
        start = 0; count = 0;
      }
    }

    /* replace */
    for (int j = 0; j < row_size; j++)
      si.img_ref->pixels[si.vertical_sort?j:i][si.vertical_sort?i:j] = row[j];

  }
  free(row);
  return;
}


/* debug print */
void print_sort_info(sort_info si) {
  printf("SORT INFO STRUCT [\n");
  printf("- img_ref : %s\n", (si.img_ref ?  "loaded" : "NULL" ));
  printf("- vertical sort : %s\n", (si.vertical_sort ? "vertical" : "horizontal"));
  printf("- angle : %f\n", si.angle);
  printf("- start threshold : %f\n", si.start_threshold);
  printf("- stop threshold : %f\n", si.stop_threshold);
  printf("- random start : %d\n", si.random_start);
  printf("- random stop : %d\n", si.random_stop);
  printf("- min length : %d\n", si.min_length);
  printf("- max length : %d\n", si.max_length);
  printf("]\n");
}
