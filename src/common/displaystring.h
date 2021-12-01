//==========================================================================
//   DISPLAYSTRING.H  - part of
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __OMNETPP_COMMON_DISPLAYSTRING_H
#define __OMNETPP_COMMON_DISPLAYSTRING_H

#include "commondefs.h"

namespace omnetpp {
namespace common {

#define MAXARGS 16

/**
 * Class for storage and easy manipulation of display strings. Display strings
 * are used to control the graphical presentation of network elements
 * when a GUI execution environment is used.
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
 *   DisplayString dispstr("a=1,2;p=alpha,,3");
 *   dispstr.insertTag("x");
 *   dispstr.setTagArg("x",0,"joe");
 *   dispstr.setTagArg("x",2,"jim");
 *   dispstr.setTagArg("p",0,"beta");
 *   EV << dispstr.str();  // result: "x=joe,,jim;a=1,2;p=beta,,3"
 * </pre>
 *
 * Limitation: a tag may have at most 16 arguments.
 *
 * NOTE: this is a modified copy of the cDisplayString class of the simulation
 * kernel, created because some components not dependent on the simkernel also
 * need to manipulate display strings.
 */
class COMMON_API DisplayString
{
  private:
    struct Tag {
       char *name;
       int numArgs;
       char *args[MAXARGS];
    };
    char *buffer = nullptr;     // holds pieces of display string (sliced with zeroes)
    char *bufferEnd = nullptr;  // points to last byte of buffer allocated
    Tag *tags = nullptr;        // table of tags
    int numTags = 0;            // number of tags

    mutable char *assembledString = nullptr;    // cached copy of assembled display string
    mutable bool assembledStringValid = false;  // whether cached copy is up-to-date

  private:
    // helper functions
    void copy(const DisplayString& ds) {parse(ds.str());}
    bool parse();
    void assemble() const;
    int getTagIndex(const char *tagname) const;
    void clearTags();
    bool isInBuffer(char *s) const {return s>=buffer && s<=bufferEnd;}
    static void strcatEscaped(char *d, const char *s);

  public:
    /** @name Constructors, destructor. */
    //@{

    /**
     * Constructor.
     */
    DisplayString() {}

    /**
     * Constructor.
     */
    DisplayString(const char *dispstr);

    /**
     * Copy constructor.
     */
    DisplayString(const DisplayString& ds);

    /**
     * Destructor.
     */
    ~DisplayString();
    //@}

    /** @name Assignment, conversions. */
    //@{
    /**
     * Assignment operator.
     */
    DisplayString& operator=(const DisplayString& other);

    /**
     * Conversion from string.
     */
    DisplayString& operator=(const char *s)  {parse(s); return *this;}

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
     * Sets the display string to the given value. The return value is false
     * if there was an error parsing the string.
     */
    bool parse(const char *displaystr);

    /**
     * Update with the contents of another display string. Corresponding
     * elements from the new display string will overwrite existing values.
     */
    void updateWith(const DisplayString& ds);

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

}  // namespace common
}  // namespace omnetpp


#endif


