//==========================================================================
//   CDISPLAYSTRING.H  - part of
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2017 Andras Varga
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __OMNETPP_CDISPLAYSTRING_H
#define __OMNETPP_CDISPLAYSTRING_H

#include "cobject.h"
#include "cgate.h"
#include "cmodule.h"

namespace omnetpp {


/**
 * @brief Represents a display string.
 *
 * Display strings are used to control the graphical presentation of network
 * elements when a graphical runtime environment is used.
 *
 * A display string consist of several named tags, where each tag may have
 * several arguments (parameters). The syntax: `tag1=value1,val2,val3;tag2=val4,val5`.
 *
 * The special characters `,`, `;`, and `=` may be included in tag argument
 * values by preceding (escaping) them with a `\`. A literal backslash may
 * be written as `\\`.
 *
 * Tag values may also contain expressions in the following form: `${expression}`.
 * The three special characters mentioned above lose their meaning within these
 * expressions even without escaping. A literal dollar sign may be written as `$$`.
 *
 * This class facilitates tasks such as adding new tags, adding arguments to
 * existing tags, removing tags or replacing arguments. The internal storage
 * method allows very fast operation; it will generally be faster than
 * direct string manipulation.
 *
 * The this class does not know about the meaning or semantics of various
 * display string tags, nor does it care about what is written in the embedded
 * expressions, it merely parses the string as data elements separated by
 * semicolons, equal signs and commas.
 *
 * An example:
 * 
 * ```
 * cDisplayString dispstr("a=1,2;p=alpha,,3");
 * dispstr.insertTag("x");
 * dispstr.setTagArg("x",0,"joe");
 * dispstr.setTagArg("x",2,"jim");
 * dispstr.setTagArg("p",0,"beta");
 * EV << dispstr.str();  // result: "x=joe,,jim;a=1,2;p=beta,,3"
 * ```
 *
 * Limitation: a tag may have at most 16 arguments.
 *
 * @ingroup ModelComponents
 */
class SIM_API cDisplayString
{
  private:
    enum { MAXARGS = 16 };  // maximum number of arguments per tag

    // holds one tag
    struct Tag {
       char *name;
       int numArgs;
       char *args[MAXARGS];
       Tag() {name=nullptr; numArgs=0;}
    };

    char *buffer = nullptr;     // holds pieces of display string (sliced with zeroes)
    char *bufferEnd = nullptr;  // points to last byte of buffer allocated
    Tag *tags = nullptr;        // table of tags
    int numTags = 0;            // number of tags

    mutable char *assembledString = nullptr; // cached copy of assembled display string
    mutable bool assembledStringValid = true; // if cached copy is up-to-date

    // needed for notifications
    cComponent *ownerComponent = nullptr;

  private:
    void copy(const cDisplayString& other) {parse(other.str());}

    // helper functions
    void doParse(const char *s);
    void doUpdateWith(const cDisplayString& ds);
    bool doSetTagArg(int tagindex, int index, const char *value);
    bool doSetTagArg(const char *tagname, int index, const char *value);
    int doInsertTag(const char *tagname, int atindex=0);
    bool doRemoveTag(int tagindex);

    void doParse();
    void assemble() const;
    void clearTags();
    bool pointsIntoBuffer(char *s) const {return s>=buffer && s<=bufferEnd;}
    static void strcatescaped(char *d, const char *s);

    // internal: called before the stored display string changes
    void beforeChange();
    // internal: called when the stored display string changes, and notifies Envir in turn.
    void afterChange();

  public:
    // internal:
    void setHostObject(cComponent *o) {ownerComponent=o;}
    void dump(std::ostream& out) const;

  public:
    /** @name Constructors, destructor. */
    //@{

    /**
     * Constructor.
     */
    cDisplayString() {}

    /**
     * Constructor that initializes the object with the given string.
     * See parse() for details. Throws an error if there was an error
     * parsing the string.
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

    /** @name Assignment, conversions. */
    //@{
    /**
     * Assignment operator.
     */
    cDisplayString& operator=(const cDisplayString& ds);

    /**
     * Conversion from string.
     */
    cDisplayString& operator=(const char *s)  {parse(s); return *this;}

    /**
     * Conversion to string.
     */
    operator const char *() const  {return str();}
    //@}

    /** @name Getting and setting the stored display string. */
    //@{

    /**
     * Returns the display string.
     */
    const char *str() const;

    /**
     * Same as parse().
     */
    void set(const char *displaystr)  {parse(displaystr);}

    /**
     * Sets the display string to the given value. Throws an error if there
     * was an error parsing the string.
     *
     * If a tag argument contains a "," or ";", it needs to be escaped with
     * a backslash to take away its special meaning. Other backslashes are
     * are ignored (i.e. not stored). To add a backslash into a tag argument,
     * it needs to be duplicated. Example: the 't=foo\,bar\;bs=\\' string
     * (i.e. the "t=foo\\,bar\\;bs=\\\\" C++ string constant) will be parsed
     * as a "t" tag having the single argument "foo,bar;bs=\".
     */
    void parse(const char *displaystr);

    /**
     * Update with the contents of another display string. Corresponding
     * elements from the new display string will overwrite existing values.
     */
    void updateWith(const cDisplayString& ds);

    /**
     * Update with the contents of another display string. Corresponding
     * elements from the new display string will overwrite existing values.
     */
    void updateWith(const char *s);
    //@}

    /** @name Manipulating tags by name. */
    //@{

    /**
     * Returns true if the stored display string contains the given tag.
     */
    bool containsTag(const char *tagname) const;

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
     * Sets an argument for the given tag. The value may be nullptr.
     * If index points beyond the last argument, the list of arguments will
     * be extended by inserting intervening empty arguments if necessary.
     *
     * The result is true if the operation was successful. False is returned
     * if the given tag doesn't exist, or index is invalid (negative or
     * greater than the maximum number of arguments, currently 16).
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

    /** @name Manipulating tags by index. */
    //@{

    /**
     * Returns the number of tags in the display string. Tags are indexed
     * starting from 0.
     */
    int getNumTags() const;

    /**
     * Returns the name of the tag given with its index.
     * If the tag index is out of range, nullptr is returned.
     */
    const char *getTagName(int tagindex) const;

    /**
     * Returns the tag index for the given tag, or -1 if the display string
     * does not contain the given tag.
     */
    int getTagIndex(const char *tagname) const;

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
     * nullptr is returned.
     */
    const char *getTagArg(int tagindex, int index) const;

    /**
     * Sets an argument for the given tag. The value may be nullptr.
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


}  // namespace omnetpp


#endif


