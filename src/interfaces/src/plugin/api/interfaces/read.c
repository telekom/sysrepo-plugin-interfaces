#include "plugin/common.h"
#include "srpc/common.h"

#include <errno.h>

int read_from_proc_file(const char *dir_path, char *interface, const char *fn, int *val)
{
	int error = 0;
	char tmp_buffer[PATH_MAX];
	FILE *fptr = NULL;
	char tmp_val[2] = {0};

	error = snprintf(tmp_buffer, sizeof(tmp_buffer), "%s/%s/%s", dir_path, interface, fn);
	if (error < 0) {
		// snprintf error
		SRPLG_LOG_ERR(PLUGIN_NAME, "snprintf failed");
		goto out;
	}

	// snprintf returns return the number of bytes that are written
	// reset error to 0
	error = 0;

	fptr = fopen((const char *) tmp_buffer, "r");

	if (fptr != NULL) {
		fgets(tmp_val, sizeof(tmp_val), fptr);

		*val = atoi(tmp_val);

		fclose(fptr);
	} else {
		SRPLG_LOG_ERR(PLUGIN_NAME, "failed to open %s: %s", tmp_buffer, strerror(errno));
		error = -1;
		goto out;
	}

out:
	return error;
}

int read_from_sys_file(const char* dir_path, char* interface, int* val)
{
    int error = 0;
    char tmp_buffer[PATH_MAX];
    FILE* fptr = NULL;
    char tmp_val[4] = { 0 };

    error = snprintf(tmp_buffer, sizeof(tmp_buffer), "%s/%s/type", dir_path, interface);
    if (error < 0) {
        // snprintf error
        SRPLG_LOG_ERR(PLUGIN_NAME, "%s: snprintf failed", __func__);
        goto out;
    }

    /* snprintf returns return the number of bytes that are written - reset error to 0 */
    error = 0;

    fptr = fopen((const char*)tmp_buffer, "r");

    if (fptr != NULL) {
        fgets(tmp_val, sizeof(tmp_val), fptr);

        *val = atoi(tmp_val);

        fclose(fptr);
    } else {
        SRPLG_LOG_ERR(PLUGIN_NAME, "%s: failed to open %s: %s", __func__, tmp_buffer, strerror(errno));
        error = -1;
        goto out;
    }

out:
    return error;
}
