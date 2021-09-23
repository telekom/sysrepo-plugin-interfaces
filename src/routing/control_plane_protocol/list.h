#ifndef ROUTING_CONTROL_PLANE_PROTOCOL_LIST_H
#define ROUTING_CONTROL_PLANE_PROTOCOL_LIST_H

#include "control_plane_protocol.h"

struct control_plane_protocol_list {
	struct control_plane_protocol *list;
	int size;
};

void control_plane_protocol_list_init(struct control_plane_protocol_list *ls);
void control_plane_protocol_list_free(struct control_plane_protocol_list *ls);

#endif // ROUTING_CONTROL_PLANE_PROTOCOL_LIST_H
