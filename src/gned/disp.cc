#include <stdio.h>
#include <string.h>
#include "disp.h"

DisplayString::DisplayString(char* in){
   stringToParse = in;
   int len = 0;
   pos = 0;

   len = strlen(stringToParse)+1;

   isConnection = 0;
   isSubmodule  = 0;

   // A connectionhoz tartozo valtozok.
   isDrawModeAuto   = 0;
   isDrawModeManual = 0;
   drawMode         = new char[len];
   drawMode[0]      = '\0';
   con_src_x        = -1;
   con_src_y        = -1;
   con_dest_x       = -1;
   con_dest_y       = -1;

   // A submodulehoz tartozo valtozok.
   isIcon  = 0;
   isBox   = 0;
   icon    = new char[len];
   icon[0] = '\0';
   x_pos   = -1;
   y_pos   = -1;
   x_size  = -1;
   y_size  = -1;

   // Opcionalis parameterek.
   fill_color     = new char[len];
   outline_color  = new char[len];
   fill_color[0]     = '\0';
   outline_color[0]  = '\0';
   linethickness  = -1;

   char* token = new char[len];
   while(getNextToken(token))
         parseToken(token);
   delete token;
}

DisplayString::~DisplayString()
{
   delete drawMode;
   delete icon;
   delete fill_color;
   delete outline_color;
}

int DisplayString::getNextToken(char* out){
   if(stringToParse[pos]=='\0') return 0;
   int i = 0;

   while(stringToParse[pos]!=';' && stringToParse[pos]!='\0') {
       out[i++] = stringToParse[pos++];
   }
   out[i]='\0';
   if(stringToParse[pos]==';') pos++;
   return 1;
}

void DisplayString::parseToken(char* in){
   switch(in[0]){
       case 'i': parseIcon(in+2);     break;
       case 'p': parsePosition(in+2); break;
       case 'b': parseBox(in+2);      break;
       case 'o': parseOpt(in+2);      break;
       case 'm': parseMode(in+2);     break;
   }
}

void DisplayString::parseIcon(char* in){
  isSubmodule = 1;
  isIcon = 1;
  sscanf(in,"%s",icon);
}

void DisplayString::parsePosition(char* in){
  sscanf(in,"%d,%d", &x_pos, &y_pos);
  isSubmodule = 1;
}

void DisplayString::parseBox(char* in){
  sscanf(in,"%d,%d", &x_size, &y_size);
  isSubmodule = 1;
  isBox = 1;
}

void DisplayString::parseOpt(char* in){
  int i = 0;
  int j = 0;
  int k = 0;
  for(i = 0; in[i]!='\0';i++) if(in[i]==',') j++;
  switch(j){
     case 2: isSubmodule = 1;
             for(i = 0; in[k]!=',';i++,k++)fill_color[i]=in[k];
             k++;
             fill_color[i]='\0';
             for(i=0; in[k]!=',';i++,k++)outline_color[i]=in[k];
             outline_color[i] = '\0';
             sscanf(in+k+1,"%d",&linethickness);break;
     case 1: isConnection = 1;
             for(i = 0; in[k]!=',';i++,k++)fill_color[i]=in[k];
             fill_color[i]='\0';
             sscanf(in+k+1,"%d",&linethickness);break;
  }
}

void DisplayString::parseMode(char* in){
    isConnection = 1;
    if(in[0]=='m' && in[1]==',') {
         isDrawModeManual = 1;
         strcpy(drawMode,"m");
         sscanf(in+2,"%d,%d,%d,%d",&con_src_x,&con_src_y,&con_dest_x,&con_dest_y );
    }else sscanf(in,"%s", drawMode);
}

char * DisplayString::get_drawMode(){
   if( drawMode != NULL ) {
       int i = 0;
       while(drawMode[i++]!='\0');
       char* tmp = new char[i];
       i = 0;
       while(drawMode[i]!='\0') {tmp[i]=drawMode[i];i++;}
       tmp[i]='\0';
       return tmp;
   } else return NULL;
}

int DisplayString::get_con_src_x(){
    return con_src_x;
}

int DisplayString::get_con_src_y(){
    return con_src_y;
}

int DisplayString::get_con_dest_x(){
    return con_dest_x;
}

int DisplayString::get_con_dest_y(){
    return con_dest_y;
}

char* DisplayString::get_icon(){
   if( icon != NULL ) {
       int i = 0;
       while(icon[i++]!='\0');
       char* tmp = new char[i];
       i = 0;
       while(icon[i]!='\0') {tmp[i]=icon[i];i++;}
       tmp[i]='\0';

       return tmp;
   } else return NULL;
}

int DisplayString::get_x_pos(){
    return x_pos;
}

int DisplayString::get_y_pos(){
    return y_pos;
}

int DisplayString::get_x_size(){
    return x_size;
}

int DisplayString::get_y_size(){
    return y_size;
}

char* DisplayString::get_fill_color(){
   if( fill_color != NULL ) {
       int i = 0;
       while(fill_color[i++]!='\0');
       char* tmp = new char[i];
       i = 0;
       while(fill_color[i]!='\0') {tmp[i]=fill_color[i];i++;}
       tmp[i]='\0';
       return tmp;
   } else return NULL;
}

char* DisplayString::get_outline_color(){
   if( outline_color != NULL ) {
       int i = 0;
       while(outline_color[i++]!='\0');
       char* tmp = new char[i];
       i = 0;
       while(outline_color[i]!='\0') {tmp[i]=outline_color[i];i++;}
       tmp[i]='\0';
       return tmp;
   } else return NULL;
}

int DisplayString::get_linethickness(){
    return linethickness;
}


void DisplayString::toString(){
   printf("This Display String has the folowing attributes.\n");

   if( isConnection ) printf(" This is a Connection.\n");
   else               printf(" This is not a Connection.\n");
   if(  isSubmodule ) printf(" This is Submodule.\n");
   else               printf(" This is not a Submodule\n");

   // A connectionhoz tartozo valtozok.
   if( isDrawModeAuto )   printf(" Draw mode is Auto.\n");
   else                   printf(" Draw mode is not Auto.\n");
   if( isDrawModeManual ) printf(" Draw mode is Manual.\n");
   else                   printf(" Draw mode is not Manula.\n");
   if( drawMode != NULL ) printf(" ");printf(drawMode);
   printf(" Connection source_x: %d\n",con_src_x);
   printf(" Connection source_y: %d\n",con_src_y);
   printf(" Connection destination_x: %d\n",con_dest_x);
   printf(" Connection destination_y: %d\n",con_dest_y);

   // A submodulehoz tartozo valtozok.
   if( isIcon ) printf(" Icon is defined.\n");
   else printf(" No icon is defined");
   if( isBox ) printf(" This is a box\n");
   else printf(" This is not a box.\n");
   printf(" Icon name: %s\n", icon );
   printf(" Position x: %d\n", x_pos );
   printf(" Position y: %d\n", y_pos );
   printf(" Size x: %d\n", x_size );
   printf(" Size y: %d\n", y_size );

   // Opcionalis parameterek.
   if( fill_color != NULL) printf(" Fill color is: %s\n", fill_color);
   if( outline_color != NULL) printf(" Out Line color is: %s\n", outline_color);
   printf(" Linethicknes is: %d\n", linethickness);
};


