//==========================================================================
//   CPOLYMORPHIC.H - header for
//                             OMNeT++
//            Discrete System Simulation in C++
//
//  Declaration of the following classes:
//    cPolymorphic : general base class
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2004 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __CPOLYMORPHIC_H
#define __CPOLYMORPHIC_H

#include "defs.h"

/**
 * This is the ultimate base class for cObject, and thus for nearly all
 * OMNeT++ classes. cPolymorphic is a <b>lightweight common base class</b>:
 * it only contains virtual member functions but no data members,
 * hence the memory footprint of a cPolymorphic instance consists of the
 * <i>vptr</i> (pointer to the virtual member functions table) only.
 *
 * This also means that deriving your classes from cPolymorphic
 * (as opposed to having no base class) comes for free.
 * (The vptr is present anyway if you have at least one virtual member
 * function.)
 *
 * The purpose of introducing cPolymorphic is to strengthen type safety.
 * <tt>cPolymorphic *</tt> pointers should replace <tt>void *</tt> in most places
 * where you need pointers to "any data structure".
 * Using cPolymorphic will allow the safe downcasts using <tt>dynamic_cast</tt>,
 * and also some reflection using className().
 *
 * @ingroup SimCore
 */
class SIM_API cPolymorphic
{
  public:
    /**
     * Constructor. It has an empty body. (The class doesn't have data members
     * and there's nothing special to do at construction time.)
     */
    cPolymorphic() {}

    /**
     * Destructor. It has an empty body (the class doesn't have data members.)
     * It is declared here only to make the class polymorphic and make its
     * destructor virtual.
     */
    virtual ~cPolymorphic() {}

    /**
     * Returns a pointer to the class name string. This method is implemented
     * using typeid (C++ RTTI), and it does not need to be overridden
     * in subclasses.
     */
    virtual const char *className() const;
};

#endif

