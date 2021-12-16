#include "control_plane_protocol/list.h"

void control_plane_protocol_list_init(struct control_plane_protocol_list *ls)
{
	ls->list = NULL;
	ls->size = 0;
}

void control_plane_protocol_list_free(struct control_plane_protocol_list *ls)
{
	for (int i = 0; i < ls->size; i++) {
	}
}
