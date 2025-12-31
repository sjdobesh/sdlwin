#ifndef _SORT_H_
#define _SORT_H_
#include "pngz.h"

/* declare sort parameters */
typedef struct sort_info {
  pngz* img_ref;            /* image to sort */
  unsigned vertical_sort:1; /* boolean for vert or horiz sort */
  float angle;              /* angle to sort along (0-2 Pi) */
  float start_threshold, stop_threshold; /* controls what gets sorted */
  int random_start, random_stop; /* random amount to dither threshold by */
  int min_length, max_length;    /* minimum and maximum sorted length */
  int (*comparison_function)(const void*, const void*); /* sort function */
  int (*threshold_function)(pixel, float);              /* determines what to sort */
} sort_info;

/* pixel comparison function */
int compare_brightness(const void* a, const void* b);
int threshold_brightness(pixel p, float t);
//int threshold_difference(pixel a, pixel b, float t);
/* sort a pixel bufffer */
void sort(pixel* p, int length, int (*compare)(const void*, const void*));

/* vert horiz sort */
void orthogonal_sort(sort_info);

/* vert horiz sort */
void fuck_sort(sort_info);

/* debug print */
void print_sort_info(sort_info si);

#endif /* _PIX_H_ */
