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
# define	MODULE	260
# define	PARAMETERS	261
# define	GATES	262
# define	GATESIZES	263
# define	SUBMODULES	264
# define	CONNECTIONS	265
# define	DISPLAY	266
# define	IN	267
# define	OUT	268
# define	NOCHECK	269
# define	LEFT_ARROW	270
# define	RIGHT_ARROW	271
# define	FOR	272
# define	TO	273
# define	DO	274
# define	IF	275
# define	LIKE	276
# define	NETWORK	277
# define	ENDSIMPLE	278
# define	ENDMODULE	279
# define	ENDCHANNEL	280
# define	ENDNETWORK	281
# define	ENDFOR	282
# define	MACHINES	283
# define	ON	284
# define	IO_INTERFACES	285
# define	IFPAIR	286
# define	INTCONSTANT	287
# define	REALCONSTANT	288
# define	NAME	289
# define	STRINGCONSTANT	290
# define	CHARCONSTANT	291
# define	TRUE_	292
# define	FALSE_	293
# define	INPUT_	294
# define	REF	295
# define	ANCESTOR	296
# define	CONSTDECL	297
# define	NUMERICTYPE	298
# define	STRINGTYPE	299
# define	BOOLTYPE	300
# define	ANYTYPE	301
# define	CPPINCLUDE	302
# define	SYSINCFILENAME	303
# define	STRUCT	304
# define	COBJECT	305
# define	NONCOBJECT	306
# define	ENUM	307
# define	EXTENDS	308
# define	MESSAGE	309
# define	CLASS	310
# define	FIELDS	311
# define	PROPERTIES	312
# define	ABSTRACT	313
# define	CHARTYPE	314
# define	SHORTTYPE	315
# define	INTTYPE	316
# define	LONGTYPE	317
# define	DOUBLETYPE	318
# define	SIZEOF	319
# define	SUBMODINDEX	320
# define	EQ	321
# define	NE	322
# define	GT	323
# define	GE	324
# define	LS	325
# define	LE	326
# define	AND	327
# define	OR	328
# define	XOR	329
# define	NOT	330
# define	BIN_AND	331
# define	BIN_OR	332
# define	BIN_XOR	333
# define	BIN_COMPL	334
# define	SHIFT_LEFT	335
# define	SHIFT_RIGHT	336
# define	INVALID_CHAR	337
# define	UMIN	338


extern YYSTYPE yylval;

#endif /* not BISON_NED_TAB_H */
