class DisplayString {
    char* stringToParse;
    int pos;

    char  isConnection;
    char  isSubmodule;

    // A connectionhoz tartozo valtozok.
    char  isDrawModeAuto;
    char  isDrawModeManual;
    char* drawMode;
    int   con_src_x;
    int   con_src_y;
    int   con_dest_x;
    int   con_dest_y;

    // A submodulehoz tartozo valtozok.
    char  isIcon;
    char  isBox;
    char* icon;
    int   x_pos;
    int   y_pos;
    int   x_size;
    int   y_size;

    // Opcionalis parameterek.
    char* fill_color;
    char* outline_color;
    int   linethickness;

    // private fuggvenyek
    int  getNextToken(char *);
    void parseToken(char *);
    void parseIcon(char *);
    void parsePosition(char *);
    void parseBox(char *);
    void parseOpt(char *);
    void parseMode(char *);

  public:
    DisplayString(char *);
    ~DisplayString();
    char * get_drawMode();
    int    get_con_src_x();
    int    get_con_src_y();
    int    get_con_dest_x();
    int    get_con_dest_y();
    char*  get_icon();
    int    get_x_pos();
    int    get_y_pos();
    int    get_x_size();
    int    get_y_size();
    char*  get_fill_color();
    char*  get_outline_color();
    int    get_linethickness();
    void toString();
};

