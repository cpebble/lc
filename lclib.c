#include <lclib.h>


#ifdef DDC_SUPPORT
// This really should be cached and now it is <3
static DDCA_Display_Info_List* dlist = NULL;
#endif

// TODO Eventually, this will be moved from lc.c
void ferr(char *err, char *argv0, char *fstr, char *errstr) {
  fprintf(stderr, err, argv0, fstr, errstr);
  exit(EXIT_FAILURE);
}

int get_device_list(device** out){
    int n = 0;

    // Get list of BUILTIN displays
    DIR* sys_class_backlight = opendir(BUILTIN_PATH);
    struct dirent* dp;
    while (sys_class_backlight) {
        errno = 0;
        if ((dp = readdir(sys_class_backlight)) != NULL) {
            if (strcmp(dp->d_name, ".") != 0 && 
                strcmp(dp->d_name, "..") != 0) {
                // We found a valid backlight
                char* name = calloc(sizeof(char), NAME_MAX + 1);
                strcpy(name, "Built-in: ");
                strncat(name, dp->d_name, NAME_MAX - strlen(name));
                char* id = calloc(sizeof(char), NAME_MAX+1);
                strcpy(id, dp->d_name);
                device* new_device = malloc(sizeof(device));
                new_device->name = name;
                new_device->d_type = BUILTIN;
                new_device->id = id;
                out[n++] = new_device;
            }
        } else {
            if (errno != 0){
                fprintf(stderr, 
                        "Directory listing error: %s\n", 
                        strerror(errno)
                        );
            }
            break;
        }
    }
    free(sys_class_backlight);
#ifdef DDC_SUPPORT
    // Get list of ddc-displays
    // see ddcutil/src/sample_clients for better understanding
    if (dlist == NULL){
        ddca_get_display_info_list2(false, &dlist);
    }

    for(int i = 0; i < dlist->ct; i++){
        DDCA_Display_Info _dp = dlist->info[i];
        char* name = calloc(sizeof(char), NAME_MAX + 1);
        strcpy(name, "DDC: ");
        strncat(name, _dp.model_name, NAME_MAX - strlen(name));
        char* id = calloc(sizeof(char), NAME_MAX+1);
        strcpy(id, _dp.sn);
        device* new_device = malloc(sizeof(device));
        new_device->name = name;
        new_device->d_type = DDCDISPLAY;
        new_device->id = id;
        out[n++] = new_device;
        
    }
#endif
        


    return n;
}
device* get_device_by_id(char* id){
    device* res = 0x0;
    device* list[MAX_DEVICES];
    int n = get_device_list(list);
    for(int i = 0; i < n; i++){
        if (strcmp(id, list[i]->id) == 0)
            res = list[i];
        else
            free_device(list[i]);
    }

    return res;
}

int free_device_list(device** out, int n){
    for(int i = 0; i < n; i++){
        free_device(out[i]);
    }
    return 0;
}

// I mostly split this up so i could free all but one in a list
void free_device(device* dev){
    // Free the strings
    free(dev->name);
    free(dev->id);
    // Free the pointer
    free(dev);
}

int get_device_brightness(device* device){
    char* brightpath = malloc(PATH_MAX + 1);
#ifdef DDC_SUPPORT
    DDCA_Display_Handle dh = NULL;
#endif
    int res = -1;
    switch(device->d_type){
        case BUILTIN:
            // Build path
            snprintf(brightpath, 
                     PATH_MAX, 
                     BUILTIN_PATH "/%s/brightness",
                     device->id);
            // Open file
            FILE *b_fp = fopen(brightpath, "r");
            if (b_fp == NULL)
                ferr(OPEN_ERR, device->name, brightpath, strerror(errno));
            // Parse file contents
            fscanf(b_fp, "%d", &res);

            assert(0 == fclose(b_fp)); // but error handling could be improved
            break;
#ifdef DDC_SUPPORT
        case DDCDISPLAY:
            // Find display by serial_number
            if (dlist == NULL)
                ddca_get_display_info_list2(false, &dlist);
            for(int i = 0; i <= dlist->ct; i++){
                if (strcmp(dlist->info[i].sn,device->id) == 0){
                    // open a device handler
                    ddca_open_display2(dlist->info[i].dref, false, &dh);
                    // Read a value
                    DDCA_Non_Table_Vcp_Value valrec;
                    int ddrcr = ddca_get_non_table_vcp_value(
                       dh,
                       DDC_BRIGHTNESS_FEATURE,
                       &valrec);
                    if (ddrcr != 0){
                        printf("AAAAAAAH\n");
                    }
                    // I cannot believe that this sort of byte-shifting is
                    // necessary in 2020
                    uint16_t cur_val = valrec.sh << 8 | valrec.sl;
                    res = cur_val;
                    ddca_close_display(dh);
                }
            }
            break;
#endif
    }
    free(brightpath);

    return res;
}
int get_device_max_brightness(device* device){
    char* path = malloc(PATH_MAX + 1);
    int res;
#ifdef DDC_SUPPORT
    DDCA_Display_Handle dh = NULL;
#endif
    switch(device->d_type){
        case BUILTIN:
            // Build path
            snprintf(path, 
                     PATH_MAX, 
                     BUILTIN_PATH "/%s/max_brightness",
                     device->id);
            // Open file
            FILE *b_fp = fopen(path, "r");
            if (b_fp == NULL)
                ferr(OPEN_ERR, device->name, path, strerror(errno));
            // Parse file contents
            fscanf(b_fp, "%d", &res);

            assert(0 == fclose(b_fp));
            break;
#ifdef DDC_SUPPORT
        case DDCDISPLAY:
            // Find display by serial_number
            if (dlist == NULL)
                ddca_get_display_info_list2(false, &dlist);
            for(int i = 0; i <= dlist->ct; i++){
                if (strcmp(dlist->info[i].sn,device->id) == 0){
                    // open a device handler
                    ddca_open_display2(dlist->info[i].dref, false, &dh);
                    // Read a value
                    DDCA_Non_Table_Vcp_Value valrec;
                    int ddrcr = ddca_get_non_table_vcp_value(
                       dh,
                       DDC_BRIGHTNESS_FEATURE,
                       &valrec);
                    if (ddrcr != 0){
                        printf("AAAAAAAH\n");
                    }
                    // I cannot believe that this sort of byte-shifting is
                    // necessary in 2020
                    uint16_t cur_val = valrec.mh << 8 | valrec.ml;
                    res = cur_val;
                    ddca_close_display(dh);
                }
            }
            break;
#endif
    }
    free(path);
    return res;
}
int set_device_brightness(device* device, int brightness){
    char* path = malloc(PATH_MAX + 1);
#ifdef DDC_SUPPORT
    DDCA_Display_Handle dh = NULL;
#endif
    int res = 0;
    switch(device->d_type){
        case BUILTIN:
            // Build path
            snprintf(path, 
                     PATH_MAX, 
                     BUILTIN_PATH "/%s/brightness",
                     device->id);
            // Open file
            FILE *b_fp = fopen(path, "w");
            if (b_fp == NULL)
                ferr(OPEN_ERR, device->name, path, strerror(errno));

            // Write new brightness value
            fprintf(b_fp, "%d", brightness);

            assert(0 == fclose(b_fp));
            break;
#ifdef DDC_SUPPORT
        case DDCDISPLAY:
            // Find display by serial_number
            if (dlist == NULL)
                ddca_get_display_info_list2(false, &dlist);
            for(int i = 0; i <= dlist->ct; i++){
                if (strcmp(dlist->info[i].sn,device->id) == 0){
                    // open a device handler
                    ddca_open_display2(dlist->info[i].dref, false, &dh);
                    // Set a value
                    uint8_t new_sh = brightness >> 8;
                    uint8_t new_sl = brightness & 0xff;
                    DDCA_Status ddcrc = ddca_set_non_table_vcp_value(
                            dh,
                            DDC_BRIGHTNESS_FEATURE,
                            new_sh,
                            new_sl
                            );
                    if (ddcrc != 0){
                        printf("AAAAAAAH\n");
                    }
                    // I cannot believe that this sort of byte-shifting is
                    // necessary in 2020
                    ddca_close_display(dh);
                }
            }
            break;
#endif
    }
    free(path);
    return res;
}
