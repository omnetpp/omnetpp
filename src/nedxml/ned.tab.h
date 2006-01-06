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
#define	XMLDOC	294
#define	REF	295
#define	ANCESTOR	296
#define	CONSTDECL	297
#define	NUMERICTYPE	298
#define	STRINGTYPE	299
#define	BOOLTYPE	300
#define	XMLTYPE	301
#define	ANYTYPE	302
#define	CPLUSPLUS	303
#define	CPLUSPLUSBODY	304
#define	MESSAGE	305
#define	CLASS	306
#define	STRUCT	307
#define	ENUM	308
#define	NONCOBJECT	309
#define	EXTENDS	310
#define	FIELDS	311
#define	PROPERTIES	312
#define	ABSTRACT	313
#define	READONLY	314
#define	CHARTYPE	315
#define	SHORTTYPE	316
#define	INTTYPE	317
#define	LONGTYPE	318
#define	DOUBLETYPE	319
#define	UNSIGNED_	320
#define	SIZEOF	321
#define	SUBMODINDEX	322
#define	PLUSPLUS	323
#define	EQ	324
#define	NE	325
#define	GT	326
#define	GE	327
#define	LS	328
#define	LE	329
#define	AND	330
#define	OR	331
#define	XOR	332
#define	NOT	333
#define	BIN_AND	334
#define	BIN_OR	335
#define	BIN_XOR	336
#define	BIN_COMPL	337
#define	SHIFT_LEFT	338
#define	SHIFT_RIGHT	339
#define	INVALID_CHAR	340
#define	UMIN	341


extern YYSTYPE nedyylval;
