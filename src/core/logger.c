#include "logger.h"


logger_state log_request(const char *ip, const char *method, const char *uri,
                        const char *version, int code) {
    if (!ip || !method || !uri || !version) return LOG_INVALID_INPUT;

    int result = fprintf(stderr, "%s \"%s %s %s\" %d %s\n",
                        ip, method, uri, version,
                        code, ERROR_CODE_STRINGS[code]);

    return (result > 0) ? LOG_SUCCESS : LOG_WRITE_ERROR;
}


logger_state log_error(const char *ip, const char *msg, int code) {
    if (!msg) return LOG_INVALID_INPUT;

    int result = fprintf(stderr, "%s ERROR %d: %s\n",
                        ip ? ip : "SERVER",
                        code,
                        msg);

    return (result > 0) ? LOG_SUCCESS : LOG_WRITE_ERROR;
}


logger_state log_server(const char *msg) {
    if (!msg) return LOG_INVALID_INPUT;

    int result = fprintf(stderr, "[SERVER] %s\n", msg);
    return (result > 0) ? LOG_SUCCESS : LOG_WRITE_ERROR;
}


logger_state log_mdb_query(const char *query, const char *result) {
    if (!query) return LOG_INVALID_INPUT;

    int written = fprintf(stderr, "[MDB] Query: %s | Result: %s\n",
            query,
            result ? result : "No results");
    return (written > 0) ? LOG_SUCCESS : LOG_WRITE_ERROR;
}
