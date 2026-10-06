#pragma once

#include <time.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

#define GNR_MAGIC "\0GNR"
#define GNR_MAGIC_LENGTH 4

int main_gnr(int argc, const char *argv[]);

typedef struct
{
    uint8_t major, minor, patch;
} GnrVersion;

typedef struct
{
    char *name;
    char *author;
    GnrVersion version;
    time_t published;
} GnrData;

void gnrdata_free(GnrData *data);

bool gnr_validate(FILE *file);

bool gnr_tell(FILE *file, GnrData *data);

bool gnr_execute(FILE *file, char *const *argv);
