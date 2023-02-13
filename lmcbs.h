#include <system.h>
#include "R5.h"


enum lmcb_type {FREE, ALLOCATED};

struct lmcb
{
	u32int size_of_lmbc;
	enum lmcb_type type;
	struct cmcb* next;
	struct cmcb* previous;

};