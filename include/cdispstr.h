//==========================================================================
//   CDISPSTR.H   - header for
//                             OMNeT++
//            Discrete System Simulation in C++
//
//
//  Declaration of the following classes:
//    cDisplayStringParser : utility class for display string manipulation
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2003 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __CDISPSTR_H
#define __CDISPSTR_H

#include "cobject.h"


#define MAXARGS 16


/**
 * Utility class for easy manipulation of display strings. Display strings
 * are used to control the graphical presentation of network elements
 * when a GUI execution environment is used (Tkenv).
 *
 * A display string consist of several named tags, where each tag may have
 * several arguments (parameters). The syntax: "tag1=value1,val2,val3;tag2=val4,val5".
 *
 * This class facilitates tasks such as adding new tags, adding arguments to
 * existing tags, removing tags or replacing arguments. The internal storage
 * method allows very fast operation; it will generally be faster than
 * direct string manipulation.
 *
 * The object doesn't try to interpret the display string in any way,
 * nor does it know the meaning of the different tags; it merely
 * parses the string as data elements separated by semicolons, equal
 * signs and commas.
 *
 * An example:
 * <pre>
 *   cDisplayStringParser dispstr("a=1,2;p=alpha,,3");
 *   dispstr.insertTag("x");
 *   dispstr.setTagArg("x",0,"joe");
 *   dispstr.setTagArg("x",2,"jim");
 *   dispstr.setTagArg("p",0,"beta");
 *   ev << dispstr.getString();  // result: "x=joe,,jim;a=1,2;p=beta,,3"
 * </pre>
 *
 * Limitation: a tag may have at most 16 arguments.
 *
 * @ingroup SimSupport
 */
class cDisplayStringParser
{
  private:
    struct Tag {
       char *name;
       int numargs;
       char *args[MAXARGS];
    };
    char *dispstr;      // copy of display string
    char *buffer;       // holds pieces of display string (sliced with zeroes)
    char *bufferend;    // points to last byte of buffer allocated
    Tag *tags;          // table of tags
    int numtags;        // number of tags
  private:
    // helper functions
    bool parse();
    void assemble();
    int gettagindex(const char *tagname);
    void cleartags();
    bool isinbuffer(char *s) {return s>=buffer && s<=bufferend;}
    void strcatescaped(char *d, const char *s);

  public:
    /** Constructors, destructor. */
    //@{

    /**
     * Constructor.
     */
    cDisplayStringParser();

    /**
     * Constructor.
     */
    cDisplayStringParser(const char *dispstr);

    /**
     * Destructor.
     */
    ~cDisplayStringParser();
    //@}

    /** Getting and setting the stored display string. */
    //@{

    /**
     * Returns the display string.
     */
    const char *getString();

    /**
     * Sets the stored display string.
     */
    bool parse(const char *displaystr);
    //@}

    /** Manipulating tags by name. */
    //@{

    /**
     * Returns true if the stored display string contains the given tag.
     */
    bool existsTag(const char *tagname);

    /**
     * Returns the number of arguments a tag actually has in the display
     * string. The count includes empty arguments too. For example,
     * for the "x=8,,15,2;y=2" display string getNumArgs("x") returns 4,
     * and getNumArgs("y") returns 1. If the display string doesn't
     * contain the given tag, 0 is returned.
     */
    int getNumArgs(const char *tagname);

    /**
     * Returns pointer to the indexth argument of the given tag.
     * If the tag doesn't exist or the index is out of range,
     * NULL is returned.
     */
    const char *getTagArg(const char *tagname, int index);

    /**
     * Sets an argument for the given tag. The value may be NULL pointer.
     * If index points beyond the last argument, the list of arguments will
     * be extended by inserting intervening empty arguments if necessary.
     *
     * The result is true if the operation was successful. False is returned
     * if the given tag doesn't exist, or index is invalid (negative or
     * greater than the maximum numer of arguments, currently 16).
     */
    bool setTagArg(const char *tagname, int index, const char *value);

    /**
     * Removes the given tag with all its arguments from the display
     * string. The result is true if the tag was actually deleted
     * (it existed before), false otherwise.
     */
    bool removeTag(const char *tagname);
    //@}

    /** Manipulating tags by index. */
    //@{

    /**
     * Returns the number of tags in the display string. Tags are indexed
     * starting from 0.
     */
    int getNumTags();

    /**
     * Returns the name of the tag given with its index.
     * If the tag index is out of range, NULL is returned.
     */
    const char *getTagName(int tagindex);

    /**
     * Returns the number of arguments a tag actually has in the display
     * string. The count includes empty arguments too. For example,
     * for the "x=8,,15,2;y=2" display string getNumArgs("x") returns 4,
     * and getNumArgs("y") returns 1. If the display string doesn't
     * contain the given tag, 0 is returned.
     */
    int getNumArgs(int tagindex);

    /**
     * Returns pointer to the indexth argument of the given tag.
     * If the tag doesn't exist or the index is out of range,
     * NULL is returned.
     */
    const char *getTagArg(int tagindex, int index);

    /**
     * Sets an argument for the given tag. The value may be NULL pointer.
     * If index points beyond the last argument, the list of arguments will
     * be extended by inserting intervening empty arguments if necessary.
     *
     * The result is true if the operation was successful. False is returned
     * if the given tag doesn't exist, or index is invalid (negative or
     * greater than the maximum numer of arguments, currently 16).
     */
    bool setTagArg(int tagindex, int index, const char *value);

    /**
     * Inserts a tag into the display string, optionally at the given
     * index. If no index is given, the tag is inserted at the beginning
     * of the string.
     */
    bool insertTag(const char *tagname, int atindex=0);

    /**
     * Removes the given tag with all its arguments from the display
     * string. The result is true if the tag was actually deleted
     * (it existed before), false otherwise.
     */
    bool removeTag(int tagindex);
    //@}
};

#endif


