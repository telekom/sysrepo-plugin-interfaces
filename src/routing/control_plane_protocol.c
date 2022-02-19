#include "control_plane_protocol.h"
#include "utils/memory.h"

#include <string.h>

void control_plane_protocol_init(struct control_plane_protocol *cpp)
{
	cpp->type = NULL;
	cpp->description = NULL;
	memset(cpp->name, 0, sizeof(cpp->name));
	cpp->initialized = 0;
}

void control_plane_protocol_free(struct control_plane_protocol *cpp)
{
	if (cpp->type) {
		FREE_SAFE(cpp->type);
	}
	if (cpp->description) {
		FREE_SAFE(cpp->description);
	}
	control_plane_protocol_init(cpp);
}
