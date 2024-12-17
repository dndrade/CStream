#include "models.h"


const char *ERROR_CODE_STRINGS[] = {
    [ERR_OK] = "OK",
    [ERR_BAD_REQUEST] = "Bad Request",
    [ERR_NOT_FOUND] = "Not Found",
    [ERR_INTERNAL] = "Internal Server Error",
    [ERR_NOT_IMPLEMENTED] = "Not Implemented",
    [ERR_UNAVAILABLE] = "Service Unavailable"
};