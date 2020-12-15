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

// GNU Coreutils ls is lowercase and has command at start
#define OPEN_ERR "%s: cannot open %s: %s\n"
#define WRITE_ERR "%s: cannot write %s: %s\n"
#define ARG_ERR "%s: %s: %s\n"

#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void print_help() {
  printf("Usage: lc <device> <brighness>\n");
  printf("Where brightness is a integer percentage of max(e.g. 1-100)\n");
  printf("#DEVICES#\n");
  system("ls /sys/class/backlight/\n");
}

void ferr(char *err, char *argv0, char *fstr, char *errstr) {
  fprintf(stderr, err, argv0, fstr, errstr);
  exit(EXIT_FAILURE);
}

void argerr(char *err, char *argv0, char *errmsg, char *more) {
  fprintf(stderr, ARG_ERR, argv0, errmsg, more);
  // print_help();
  exit(EXIT_FAILURE);
}

int main(int argc, char *argv[]) {
  // Reduce complexity by early exit
  if (argc != 3)
    argerr(ARG_ERR, argv[0], "not enoguh arguments",
           "try running help"); // FIXME no --help

  if (atoi(argv[2]) == 0 || atoi(argv[2]) > 100)
    argerr(ARG_ERR, argv[0], "invalid brightness [1-100]", argv[2]);

  // Declare neccesary variables
  char *pre = "/sys/class/backlight/";
  char basepath[MAXPATHLENGTH] = {0};
  char maxpath[MAXPATHLENGTH] = {0};
  char brightpath[MAXPATHLENGTH] = {0};
  strcpy(basepath, pre); // This is safe. PRE is of known length

  // User input, validate length
  strncat(basepath, argv[1], MAXPATHLENGTH - strlen(pre) - 1);
  strncat(maxpath, basepath, MAXPATHLENGTH - 1);
  strncat(brightpath, basepath, MAXPATHLENGTH - 1);

  // Not really safe, but not exploitable either.
  // Will just crash if length exceeds
  strcat(maxpath, "/max_brightness");
  strcat(brightpath, "/brightness");

  FILE *m_fp = fopen(maxpath, "r");
  if (m_fp == NULL)
    ferr(OPEN_ERR, argv[0], maxpath, strerror(errno));

  FILE *b_fp = fopen(brightpath, "w");
  if (b_fp == NULL)
    ferr(OPEN_ERR, argv[0], brightpath, strerror(errno));

  int m_b;
  assert(0 < fscanf(m_fp, "%d", &m_b));

  // Calculate brightness as a percentage of max
  if (!fprintf(b_fp, "%d", atoi(argv[2]) * (m_b / 100)))
    ferr(WRITE_ERR, argv[0], brightpath, strerror(errno));

  // Again, these shouldn't really fail, but error handling could be improved
  assert(0 == fclose(m_fp));
  assert(0 == fclose(b_fp));
  return EXIT_SUCCESS;
}
