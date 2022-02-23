/* lc - light command: a brightness setter
 * Copyright (C) 2020 - 2020  Christian Egon Sørensen
 * Copyright (C) 2021 - 2022  Christian Påbøl Jacobsen
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

#include <assert.h> // for assert
#include <errno.h>  // for errno, strerror
#include <stdio.h>  // for printf, etc...
#include <stdlib.h> // for EXIT_FAILURE, EXIT_SUCCESS, etc...
#include <string.h> // for strcat, strcpy
#include <lclib.h>
#include <linux/limits.h>


void print_devices(){
    device** devices = calloc(sizeof(device*), MAX_DEVICES);
    int n = get_device_list(devices);
    if (n != 0){
        for(int i = 0; i < n; i++){
            printf("[%s]: \n\t%s\n\tCurrent lvl: %d\n\tMax lvl: %d\n", 
                   devices[i]->name,
                   devices[i]->id,
                   get_device_brightness(devices[i]),
                   get_device_max_brightness(devices[i])
                  );
        }
    } else
        printf("No devices found\n");

    free_device_list(devices, n);
    free(devices);
}

/*
 * synopsis: prints some help to the user
 *
 * TODO consider if the placement of ls is appropriate
 * TODO documentation, waited because it is not finalized
 * */
void help() {
  fprintf(stderr, "Usage: lc <device> <brighness>\n");
  fprintf(stderr, "Where brightness is a integer percentage of max(e.g. 1-100)\n");
  fprintf(stderr, "#DEVICES#\n");
  print_devices();
  exit(EXIT_SUCCESS);
}


/*
 * synopsis:   generic arguments error function
 *
 * err:        the error "type"    (char *),
 * argv0:      the command name    (char *)
 * errmsg:     the error           (char *)
 * more:       further information (char *)
 *
 * return:     EXIT_FALURE
 *
 * desc: Takes a predefined error string (containing formatting for three
 * strings), and inserts the arguments argv[0] (should be calling command),
 * errmsg (the error as human readable string), and more (containing a small
 * tip about the error).Then prints this formatted string to stderr, and exits
 * with EXIT_FAILURE.
 */
void argerr(char *err, char *argv0, char *errmsg, char *more) {
  fprintf(stderr, ARG_ERR, argv0, errmsg, more);
  exit(EXIT_FAILURE);
}

int isRelative = 0;
int index_num_arg = 2;

int main(int argc, char *argv[]) {
  // CHECK INPUT
  if (argc == 2){
    if ((!(strcmp(argv[1], "--help"))) || (!(strcmp(argv[1], "help"))) ||
        (!(strcmp(argv[1], "-h"))))
      help();
    else if (!strcmp(argv[1], "-l")) {
      print_devices();
      return 0;
    }
  }
  if (argc < 2){
    fprintf(stderr, "Not enough arguments\n");
    return 1;
  }
  if (argc >= 4)
    argerr(ARG_ERR, argv[0], "too many arguments", "try running -help");

  // TRY TO GET DEVICE
  device* dev;
  int unused;
  // No device was provided
  if (sscanf(argv[1], "%d", &unused) == 1){
    // Get ind
    int succ = get_device_by_index(0, &dev);
    if(!succ)
      argerr(ARG_ERR, argv[0], "No devices found, do you have a screen?\n", "");

    fprintf(stderr, "Device not given. Using default device %s\n", dev->name);
    index_num_arg = 1;
  }
  else if (!get_device_by_id(argv[1], &dev)) {
    // This works since if this also fails, &dev is null and we fail, but if it succeeds, 
    // dev is set properly
    argerr(ARG_ERR, argv[0], "Invalid device id", argv[1]);
  }

  // Handle Number arg
  if (argv[index_num_arg][0] == '-' || argv[index_num_arg][0] == '+'){
    isRelative = 1;
  }
  else if (atoi(argv[index_num_arg]) == 0 || atoi(argv[index_num_arg]) > 100)
    argerr(ARG_ERR, argv[0], "invalid brightness [1-100]", argv[index_num_arg]);

  // READ MAX BRIGHTNESS
  int m_b = get_device_max_brightness(dev);
  // CALCULATE TARGET BRIGHTNESS AS % OF MAX
  // NB: If argv[2] isn't an int, this turns off the screen
  int target = 512;
  if (isRelative){
    int d_b = get_device_brightness(dev);
    float target_p = ((d_b*1.0f / m_b)*100) + atoi(argv[index_num_arg]);
    if (target_p > 100.0f) 
      target_p = 100.0f;
    else if (target_p < 1.0f)
      target_p = 1.0f;
    target = target_p * (m_b/100);
  }
  else
    target = atoi(argv[index_num_arg]) * (m_b / 100);
  printf("%d\n", target);

    

  set_device_brightness(dev, target);
  free_device(dev);

  return EXIT_SUCCESS;
}
// vim: expandtab:ts=2:sw=2
