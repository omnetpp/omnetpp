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
#define	DELAY	260
#define	ERROR	261
#define	DATARATE	262
#define	MODULE	263
#define	PARAMETERS	264
#define	GATES	265
#define	GATESIZES	266
#define	SUBMODULES	267
#define	CONNECTIONS	268
#define	DISPLAY	269
#define	IN	270
#define	OUT	271
#define	NOCHECK	272
#define	LEFT_ARROW	273
#define	RIGHT_ARROW	274
#define	FOR	275
#define	TO	276
#define	DO	277
#define	IF	278
#define	LIKE	279
#define	NETWORK	280
#define	ENDSIMPLE	281
#define	ENDMODULE	282
#define	ENDCHANNEL	283
#define	ENDNETWORK	284
#define	ENDFOR	285
#define	MACHINES	286
#define	ON	287
#define	IO_INTERFACES	288
#define	IFPAIR	289
#define	INTCONSTANT	290
#define	REALCONSTANT	291
#define	NAME	292
#define	STRING	293
#define	_TRUE	294
#define	_FALSE	295
#define	INPUT	296
#define	REF	297
#define	ANCESTOR	298
#define	NED_CONST	299
#define	NUMERICTYPE	300
#define	STRINGTYPE	301
#define	BOOLTYPE	302
#define	XMLTYPE	303
#define	ANYTYPE	304
#define	PLUS	305
#define	MIN	306
#define	MUL	307
#define	DIV	308
#define	MOD	309
#define	EXP	310
#define	SIZEOF	311
#define	SUBMODINDEX	312
#define	PLUSPLUS	313
#define	EQ	314
#define	NE	315
#define	GT	316
#define	GE	317
#define	LS	318
#define	LE	319
#define	AND	320
#define	OR	321
#define	XOR	322
#define	NOT	323
#define	BIN_AND	324
#define	BIN_OR	325
#define	BIN_XOR	326
#define	BIN_COMPL	327
#define	SHIFT_LEFT	328
#define	SHIFT_RIGHT	329
#define	INVALID_CHAR	330
#define	UMIN	331


extern YYSTYPE yylval;
