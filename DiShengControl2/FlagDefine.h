#ifndef   __FlagDefine_h__
#define   __FlagDefine_h__
/************************************************************************/
/* ��һ��Ϊ���λ                                                       */
/************************************************************************/
typedef union   
{
	struct
	{
		unsigned bNet:1;
	};
	unsigned int AllFlag;
} GFlags;        // general flags
typedef union   
{
	struct
	{
		
	};
	unsigned int AllFlag;
} ErrFlags;        // general flags

#endif
