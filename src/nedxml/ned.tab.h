#ifndef BISON_NED_TAB_H
# define BISON_NED_TAB_H

# ifndef YYSTYPE
#  define YYSTYPE int
# endif

#ifndef YYLTYPE
typedef struct yyltype
{
  int first_line;
  int first_column;

  int last_line;
  int last_column;
} yyltype;

# define YYLTYPE yyltype
#endif

# define	INCLUDE	257
# define	SIMPLE	258
# define	CHANNEL	259
# define	DELAY	260
# define	ERROR	261
# define	DATARATE	262
# define	MODULE	263
# define	PARAMETERS	264
# define	GATES	265
# define	GATESIZES	266
# define	SUBMODULES	267
# define	CONNECTIONS	268
# define	DISPLAY	269
# define	IN	270
# define	OUT	271
# define	NOCHECK	272
# define	LEFT_ARROW	273
# define	RIGHT_ARROW	274
# define	FOR	275
# define	TO	276
# define	DO	277
# define	IF	278
# define	LIKE	279
# define	NETWORK	280
# define	ENDSIMPLE	281
# define	ENDMODULE	282
# define	ENDCHANNEL	283
# define	ENDNETWORK	284
# define	ENDFOR	285
# define	MACHINES	286
# define	ON	287
# define	IO_INTERFACES	288
# define	IFPAIR	289
# define	INTCONSTANT	290
# define	REALCONSTANT	291
# define	NAME	292
# define	STRINGCONSTANT	293
# define	CHARCONSTANT	294
# define	_TRUE	295
# define	_FALSE	296
# define	INPUT	297
# define	REF	298
# define	ANCESTOR	299
# define	CONSTDECL	300
# define	NUMERICTYPE	301
# define	STRINGTYPE	302
# define	BOOLTYPE	303
# define	ANYTYPE	304
# define	CPPINCLUDE	305
# define	SYSINCFILENAME	306
# define	STRUCT	307
# define	COBJECT	308
# define	NONCOBJECT	309
# define	ENUM	310
# define	EXTENDS	311
# define	MESSAGE	312
# define	CLASS	313
# define	FIELDS	314
# define	PROPERTIES	315
# define	VIRTUAL	316
# define	CHARTYPE	317
# define	SHORTTYPE	318
# define	INTTYPE	319
# define	LONGTYPE	320
# define	DOUBLETYPE	321
# define	SIZEOF	322
# define	SUBMODINDEX	323
# define	EQ	324
# define	NE	325
# define	GT	326
# define	GE	327
# define	LS	328
# define	LE	329
# define	AND	330
# define	OR	331
# define	XOR	332
# define	NOT	333
# define	BIN_AND	334
# define	BIN_OR	335
# define	BIN_XOR	336
# define	BIN_COMPL	337
# define	SHIFT_LEFT	338
# define	SHIFT_RIGHT	339
# define	INVALID_CHAR	340
# define	UMIN	341


extern YYSTYPE yylval;

#endif /* not BISON_NED_TAB_H */
