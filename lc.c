/* lc - light command: a brightness setter
 * Copyright (C) 2020 - 2020  Christian Egon Sørensen
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#define MAXPATHLENGTH 128

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

void print_help(){
  printf("Usage: lc <device> <brighness>\n");
  printf("Where brightness is a percentage of max(e.g. 1-100)\n");
  printf("#DEVICES#\n");
  system("ls /sys/class/backlight/\n");
}

int main(int argc, char *argv[]) {
  // Reduce complexity by early exit
  if (argc != 3) {
    printf("Not enough arguments provided\n");
    print_help();
    return EXIT_FAILURE;
  }
  int t_b = atoi(argv[2]);
  if (t_b == 0 || t_b > 100){
    printf("Invalid brightness argument\n");
    print_help();
    return EXIT_FAILURE;
  }

  char *pre = "/sys/class/backlight/";
  // Declare neccesary variables
  char basepath[MAXPATHLENGTH];
  char maxpath[MAXPATHLENGTH];
  char brightpath[MAXPATHLENGTH];
  strcpy(basepath, pre); // This is safe. PRE is of known length
  // User input, validate length
  strncat(basepath, argv[1], MAXPATHLENGTH - strlen(pre) - 1);
  strncat(maxpath, basepath, MAXPATHLENGTH - 1);
  strncat(brightpath, basepath, MAXPATHLENGTH - 1);
  // Not really safe, but not exploitable either. Will just crash if length
  // exceeds
  strcat(maxpath, "/max_brightness");
  strcat(brightpath, "/brightness");

  FILE *m_fp = fopen(maxpath, "r");
  if (m_fp == NULL){// Probably permissions error
    printf("Error in open Max file: %s\n", strerror(errno));
    return errno;
  }
  FILE *b_fp = fopen(brightpath, "w");
  if (b_fp == NULL){// Probably permissions error
    printf("Error in open brightness file: %s\n", strerror(errno));
    return errno;
  }

  int m_b;
  assert(0 < fscanf(m_fp, "%d", &m_b));
  // Calculate brightness as a percentage of max
  int calculated_brightness = t_b * (m_b / 100);
  int err = fprintf(b_fp, "%d", calculated_brightness);
  if (err == 0){
    printf("Error in writing brightness file: %s\n", strerror(errno));
    return EXIT_FAILURE;
  }
  // Again, these shouldn't really fail, but error handling could be improved
  assert(0 == fclose(m_fp));
  assert(0 == fclose(b_fp));
  return EXIT_SUCCESS;
}
// vim: noai:ts=2:sw=2
