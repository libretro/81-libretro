/*
**
** DIS.H
**
** Well, nobody is going to write a 68000 disasembler for me
*/

#ifndef WORD
#define WORD short
#endif


typedef struct {
	char *op;	/*	pointer to assembly nemonic	*/
	unsigned WORD op_code;	/*	base op code	*/
	unsigned WORD mask;		/*	mask to get rid of variable data	*/
	unsigned WORD *(*d_funct)(unsigned WORD *c,char *s,int index);	/*	function to call	*/
}OP_DEF;

/*
** size tokens
*/

#define BYTE_SIZE	256
#define WORD_SIZE	257
#define LONG_SIZE	258

extern void *dis(void *c,char *s);

