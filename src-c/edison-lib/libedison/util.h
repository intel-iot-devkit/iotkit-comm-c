#include <stdbool.h>
#include <cJSON.h>

/** @brief Check whether JSON item is a string. */
static inline bool isJsonString(cJSON *item) {
    if (item == NULL || item->type != cJSON_String)
        return false;
    return true;
}

/** @brief Check whether JSON item is a number. */
static inline bool isJsonNumber(cJSON *item) {
    if (item == NULL || item->type != cJSON_Number)
        return false;
    return true;
}

/** @brief Check whether JSON item is an object. */
static inline bool isJsonObject(cJSON *item) {
    if (item == NULL || item->type != cJSON_Object)
        return false;
    return true;
}

/** @brief Check whether JSON item is an array. */
static inline bool isJsonArray(cJSON *item) {
    if (item == NULL || item->type != cJSON_Array)
	return false;
    return true;
}

static inline bool checkDLError() {
    char *error = dlerror();
    if (error != NULL) {
	fprintf(stderr, "DL error %s\n", error);
	return false;
    }
    return true;
}
