#ifndef BISON_NED_TAB_H
# define BISON_NED_TAB_H

# ifndef YYSTYPE
#  define YYSTYPE int
#  define YYSTYPE_IS_TRIVIAL 1
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
# define YYLTYPE_IS_TRIVIAL 1
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
# define	STRING	293
# define	_TRUE	294
# define	_FALSE	295
# define	INPUT	296
# define	REF	297
# define	ANCESTOR	298
# define	CONSTDECL	299
# define	NUMERICTYPE	300
# define	STRINGTYPE	301
# define	BOOLTYPE	302
# define	ANYTYPE	303
# define	PLUS	304
# define	MIN	305
# define	MUL	306
# define	DIV	307
# define	MOD	308
# define	EXP	309
# define	SIZEOF	310
# define	SUBMODINDEX	311
# define	EQ	312
# define	NE	313
# define	GT	314
# define	GE	315
# define	LS	316
# define	LE	317
# define	AND	318
# define	OR	319
# define	XOR	320
# define	NOT	321
# define	BIN_AND	322
# define	BIN_OR	323
# define	BIN_XOR	324
# define	BIN_COMPL	325
# define	SHIFT_LEFT	326
# define	SHIFT_RIGHT	327
# define	INVALID_CHAR	328
# define	UMIN	329


extern YYSTYPE yylval;

#endif /* not BISON_NED_TAB_H */
