//==========================================================================
// nedstring.h  - part of the OMNeT++ Discrete System Simulation System
//
// Contents:
//   class NEDString
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2002 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __NEDSTRING_H
#define __NEDSTRING_H


/**
 * Very simple string class. It has only one data member,
 * a char* pointer. Allocation/deallocation of the contents takes place
 * via opp_strdup() and operator delete
 *
 * @ingroup Data
 */
class NEDString
{
  private:
    char *str;
    char *_strdup(const char *s);

  public:
    /**
     * Constructor.
     */
    NEDString()               {str = 0;}

    /**
     * Constructor.
     */
    NEDString(const char *s)  {str = _strdup(s);}

    /**
     * Copy constructor.
     */
    NEDString(NEDString& s)  {str = _strdup(s.str);}

    /**
     * Destructor.
     */
    ~NEDString()              {delete [] str;}

    /**
     * Returns pointer to the string.
     */
    operator const char *() const    {return str;}

    /**
     * Returns pointer to the internal buffer where the string is stored.
     * It is allowed to write into the string via this pointer, but the
     * length of the string should not be exceeded.
     */
    char *buffer() const        {return str;}

    /**
     * Allocates a buffer of the given size.
     */
    char *allocate(unsigned size)
                               {delete [] str; str=new char[size]; return str;}

    /**
     * Deletes the old value and _strdup()'s the new value
     * to create the object's own copy.
     */
    NEDString& operator=(const char *s)
                               {delete [] str; str=_strdup(s); return *this;}

    /**
     * Assignment.
     */
    NEDString& operator=(const NEDString& s)
                               {delete [] str; str=_strdup(s.str); return *this;}

    /**
     * String comparison.
     */
    bool operator==(const char *s);

    /**
     * Concatenation.
     */
    NEDString& operator+=(const char *s);

};

#endif

