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

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[]) {
  if (argc == 3) {
    char *pre = "/sys/class/backlight/";
    char basepath[64] = {0};
    char maxpath[128] = {0};
    char brightpath[128] = {0};
    strcpy(basepath, pre);
    strcat(basepath, argv[1]);
    strcat(maxpath, basepath);
    strcat(brightpath, basepath);
    strcat(maxpath, "/max_brightness");
    strcat(brightpath, "/brightness");

    FILE *m_fp = fopen(maxpath, "r");
    FILE *b_fp = fopen(brightpath, "w");
    char buff[255 * 5];

    assert(0 < fscanf(m_fp, "%s", buff));

    int t_b = atoi(argv[2]);
    int m_b = atoi(buff);

    assert(0 < fprintf(b_fp, "%d", t_b * (m_b / 100)));

    assert(0 == fclose(m_fp));
    assert(0 == fclose(b_fp));
    return EXIT_SUCCESS;
  }
  printf("Usage: %s device 100\n", argv[0]);
  printf("#DEVICES#\n");
  system("ls /sys/class/backlight/\n");

  return EXIT_FAILURE;
}
