//==========================================================================
//   CDISPSTR.H  - part of
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//
//  Declaration of the following classes:
//    cDisplayString : utility class for display string manipulation
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2005 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __CDISPSTR_H
#define __CDISPSTR_H

#include "cobject.h"


#define MAXARGS 16


/**
 * Class for storage and easy manipulation of display strings. Display strings
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
 *   cDisplayString dispstr("a=1,2;p=alpha,,3");
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
class SIM_API cDisplayString
{
  private:
    struct Tag {
       char *name;
       int numargs;
       char *args[MAXARGS];
    };
    char *buffer;       // holds pieces of display string (sliced with zeroes)
    char *bufferend;    // points to last byte of buffer allocated
    Tag *tags;          // table of tags
    int numtags;        // number of tags

    mutable char *dispstr; // cached copy of assembled display string
    mutable bool needsassemble; // if dispstr is up-to-date

    // needed to notify Envir
    cObject *object;     // a cModule or cGate pointer
    enum {NONE, CONNECTION, MODULE, MODULEBACKGROUND} role; // what

  private:
    // helper functions
    bool parse();
    void assemble() const;
    int gettagindex(const char *tagname) const;
    void cleartags();
    bool isinbuffer(char *s) {return s>=buffer && s<=bufferend;}
    static void strcatescaped(char *d, const char *s);

  private:
    // internal: called when the stored display string changes, and notifies Envir in turn.
    void notify();

  public:
    // internal:
    void setRoleToConnection(cGate *gate) {object=gate; role=CONNECTION;}
    void setRoleToModule(cModule *submodule) {object=submodule; role=MODULE;}
    void setRoleToModuleBackground(cModule *module) {object=module; role=MODULEBACKGROUND;}

  public:
    /** Constructors, destructor. */
    //@{

    /**
     * Constructor.
     */
    cDisplayString();

    /**
     * Constructor.
     */
    cDisplayString(const char *dispstr);

    /**
     * Copy constructor.
     */
    cDisplayString(const cDisplayString& ds);

    /**
     * Destructor.
     */
    ~cDisplayString();
    //@}

    /** Assignment, conversions. */
    //@{
    /**
     * Assignment operator.
     */
    cDisplayString& operator=(const cDisplayString& ds) {parse(ds.getString()); return *this;}

    /**
     * Conversion from string.
     */
    cDisplayString& operator=(const char *s)  {parse(s); return *this;}

    /**
     * Conversion to string.
     */
    operator const char *() const  {return getString();}
    //@}

    /** Getting and setting the stored display string. */
    //@{

    /**
     * Returns the display string.
     */
    const char *getString() const;

    /**
     * Sets the display string to the given value. The return value is false
     * if there was an error parsing the string.
     */
    bool parse(const char *displaystr);
    //@}

    /** Manipulating tags by name. */
    //@{

    /**
     * Returns true if the stored display string contains the given tag.
     */
    bool existsTag(const char *tagname) const;

    /**
     * Returns the number of arguments a tag actually has in the display
     * string. The count includes empty arguments too. For example,
     * for the "x=8,,15,2;y=2" display string getNumArgs("x") returns 4,
     * and getNumArgs("y") returns 1. If the display string doesn't
     * contain the given tag, 0 is returned.
     */
    int getNumArgs(const char *tagname) const;

    /**
     * Returns pointer to the indexth argument of the given tag.
     * If the tag doesn't exist or the index is out of range,
     * empty string ("") is returned.
     */
    const char *getTagArg(const char *tagname, int index) const;

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
     * Convenience function to set a tag argument to a numeric value. Converts
     * 'value' to string, then calls setTagArg(const char *, int, const char *).
     */
    bool setTagArg(const char *tagname, int index, long value);

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
    int getNumTags() const;

    /**
     * Returns the name of the tag given with its index.
     * If the tag index is out of range, NULL is returned.
     */
    const char *getTagName(int tagindex) const;

    /**
     * Returns the number of arguments a tag actually has in the display
     * string. The count includes empty arguments too. For example,
     * for the "x=8,,15,2;y=2" display string getNumArgs("x") returns 4,
     * and getNumArgs("y") returns 1. If the display string doesn't
     * contain the given tag, 0 is returned.
     */
    int getNumArgs(int tagindex) const;

    /**
     * Returns pointer to the indexth argument of the given tag.
     * If the tag doesn't exist or the index is out of range,
     * NULL is returned.
     */
    const char *getTagArg(int tagindex, int index) const;

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
     * of the string. Return value is the index of the tag.
     * If the display string already contains a tag with the given tagname,
     * nothing is changed and the index of the existing tag is returned.
     */
    int insertTag(const char *tagname, int atindex=0);

    /**
     * Removes the given tag with all its arguments from the display
     * string. The result is true if the tag was actually deleted
     * (it existed before), false otherwise.
     */
    bool removeTag(int tagindex);
    //@}
};


#endif


