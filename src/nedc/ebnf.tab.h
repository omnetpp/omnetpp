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

#define	INCLUDE	258
#define	SIMPLE	259
#define	CHANNEL	260
#define	DELAY	261
#define	ERROR	262
#define	DATARATE	263
#define	MODULE	264
#define	PARAMETERS	265
#define	GATES	266
#define	GATESIZES	267
#define	SUBMODULES	268
#define	CONNECTIONS	269
#define	DISPLAY	270
#define	IN	271
#define	OUT	272
#define	NOCHECK	273
#define	LEFT_ARROW	274
#define	RIGHT_ARROW	275
#define	FOR	276
#define	TO	277
#define	DO	278
#define	IF	279
#define	LIKE	280
#define	NETWORK	281
#define	ENDSIMPLE	282
#define	ENDMODULE	283
#define	ENDCHANNEL	284
#define	ENDNETWORK	285
#define	ENDFOR	286
#define	MACHINES	287
#define	ON	288
#define	IO_INTERFACES	289
#define	IFPAIR	290
#define	INTCONSTANT	291
#define	REALCONSTANT	292
#define	NAME	293
#define	STRING	294
#define	_TRUE	295
#define	_FALSE	296
#define	INPUT	297
#define	REF	298
#define	ANCESTOR	299
#define	NED_CONST	300
#define	NUMERICTYPE	301
#define	STRINGTYPE	302
#define	BOOLTYPE	303
#define	ANYTYPE	304
#define	PLUS	305
#define	MIN	306
#define	MUL	307
#define	DIV	308
#define	MOD	309
#define	EXP	310
#define	SIZEOF	311
#define	SUBMODINDEX	312
#define	EQ	313
#define	NE	314
#define	GT	315
#define	GE	316
#define	LS	317
#define	LE	318
#define	AND	319
#define	OR	320
#define	XOR	321
#define	NOT	322
#define	BIN_AND	323
#define	BIN_OR	324
#define	BIN_XOR	325
#define	BIN_COMPL	326
#define	SHIFT_LEFT	327
#define	SHIFT_RIGHT	328
#define	INVALID_CHAR	329
#define	UMIN	330


extern YYSTYPE yylval;
