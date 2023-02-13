#include <system.h>
#include "R5.h"

enum cmcb_type {FREE, ALLOCATED};


struct cmcb
{
	u32int addr;
	u32int size_of_cmbc;
	char name_cmbc[15];
	enum cmcb_type type;
	struct cmcb* next;
	struct cmcb* previous;

};




