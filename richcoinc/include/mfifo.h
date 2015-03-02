#include "jval.h"
typedef struct mfifo{
	Jval *mfifo_data;
	int mfifo_size;
	int mfifo_currnt;
}*MFIFO;

MFIFO make_mfifo(int);
int insert_mfifo(MFIFO,Jval);
int delete_mfifo(MFIFO);
int find_mfifo(MFIFO,int (*isequal)(Jval,Jval),Jval);
Jval find_next_mfifo(MFIFO);