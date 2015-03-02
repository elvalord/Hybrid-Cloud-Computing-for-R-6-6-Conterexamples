#include "mfifo.h"

MFIFO make_mfifo(int n)
{
	MFIFO new_mfifo = (MFIFO)malloc(sizeof(struct mfifo));
	new_mfifo->mfifo_data = (Jval*)malloc(n*sizeof(Jval));
	memset(new_mfifo->mfifo_data,0,n*sizeof(Jval));
	new_mfifo->mfifo_size = n;
	new_mfifo->mfifo_currnt = 0;
	return new_mfifo;
}

int delete_mfifo(MFIFO mfifo_to_del)//not delete data
{
	if(mfifo_to_del==0)
		return 1;
	if(mfifo_to_del->mfifo_data!=0)
		free(mfifo_to_del->mfifo_data);
	free(mfifo_to_del);
	return 1;
}
int insert_mfifo(MFIFO mfifo_to_insert,Jval data)
{
	mfifo_to_insert->mfifo_data[(mfifo_to_insert->mfifo_currnt+1)%(mfifo_to_insert->mfifo_size)] = data;
	mfifo_to_insert->mfifo_currnt=(mfifo_to_insert->mfifo_currnt+1)%(mfifo_to_insert->mfifo_size);
}
Jval find_next_mfifo(MFIFO to_find_next)
{
	return to_find_next->mfifo_data[(to_find_next->mfifo_currnt+1)%(to_find_next->mfifo_size)];
}
int find_mfifo(MFIFO mfifo_to_find,int (*isequal)(Jval,Jval),Jval to_find)
{
	int i;
	for(i=0;i<mfifo_to_find->mfifo_size;i++)
	{
		if(isequal(mfifo_to_find->mfifo_data[i],to_find)==1)
			return 1;
	}
	return 0;
}
