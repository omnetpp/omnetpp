#ifndef YYSTYPE
#define YYSTYPE int
#endif

#ifndef YYLTYPE
typedef
  struct yyltype
    {
      int timestamp;
      int first_line;
      int first_column;
      int last_line;
      int last_column;
      char *text;
   }
  yyltype;

#define YYLTYPE yyltype
#endif

#define	INCLUDE	257
#define	SIMPLE	258
#define	CHANNEL	259
#define	MODULE	260
#define	PARAMETERS	261
#define	GATES	262
#define	GATESIZES	263
#define	SUBMODULES	264
#define	CONNECTIONS	265
#define	DISPLAY	266
#define	IN	267
#define	OUT	268
#define	NOCHECK	269
#define	LEFT_ARROW	270
#define	RIGHT_ARROW	271
#define	FOR	272
#define	TO	273
#define	DO	274
#define	IF	275
#define	LIKE	276
#define	NETWORK	277
#define	ENDSIMPLE	278
#define	ENDMODULE	279
#define	ENDCHANNEL	280
#define	ENDNETWORK	281
#define	ENDFOR	282
#define	MACHINES	283
#define	ON	284
#define	CHANATTRNAME	285
#define	INTCONSTANT	286
#define	REALCONSTANT	287
#define	NAME	288
#define	STRINGCONSTANT	289
#define	CHARCONSTANT	290
#define	TRUE_	291
#define	FALSE_	292
#define	INPUT_	293
#define	REF	294
#define	ANCESTOR	295
#define	CONSTDECL	296
#define	NUMERICTYPE	297
#define	STRINGTYPE	298
#define	BOOLTYPE	299
#define	ANYTYPE	300
#define	CPPINCLUDE	301
#define	SYSINCFILENAME	302
#define	STRUCT	303
#define	COBJECT	304
#define	NONCOBJECT	305
#define	ENUM	306
#define	EXTENDS	307
#define	MESSAGE	308
#define	CLASS	309
#define	FIELDS	310
#define	PROPERTIES	311
#define	ABSTRACT	312
#define	CHARTYPE	313
#define	SHORTTYPE	314
#define	INTTYPE	315
#define	LONGTYPE	316
#define	DOUBLETYPE	317
#define	SIZEOF	318
#define	SUBMODINDEX	319
#define	EQ	320
#define	NE	321
#define	GT	322
#define	GE	323
#define	LS	324
#define	LE	325
#define	AND	326
#define	OR	327
#define	XOR	328
#define	NOT	329
#define	BIN_AND	330
#define	BIN_OR	331
#define	BIN_XOR	332
#define	BIN_COMPL	333
#define	SHIFT_LEFT	334
#define	SHIFT_RIGHT	335
#define	INVALID_CHAR	336
#define	UMIN	337


extern YYSTYPE yylval;
