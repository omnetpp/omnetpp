//=========================================================================
//  COMPOUNDFILTER.H - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2005 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef _COMPOUNDFILTER_H_
#define _COMPOUNDFILTER_H_

#include <string>
#include <vector>

#include "nodetype.h"
#include "commonnodes.h"


class CompoundFilterType;


/**
 * FilterNode nodes combined into a single filter by cascading them.
 */
class CompoundFilter : public FilterNode
{
        friend class CompoundFilterType;
    protected:
        FilterNode *first;
        FilterNode *last;
        StringMap _attrvalues;  // _attrs[name] = value
    public:
        CompoundFilter() {}
        virtual ~CompoundFilter() {}
        FilterNode *firstNode() {return first;}
        FilterNode *lastNode() {return last;}

        virtual bool isReady() const  {return false;}
        virtual void process()  {}
        virtual bool finished() const {return true;}
};


/**
 * FilterNode nodes combined into a single filter by cascading them.
 */
class CompoundFilterType : public FilterNodeType
{
    public:
        /** Describes one of the cascaded filters */
        class Subfilter
        {
            private:
                std::string _nodetype;  // refers to NodeType name
                std::string _comment;   // comment
                StringMap _attrassignments;  // _attrs[name] = value
            public:
                const char *nodeType() const  {return _nodetype.c_str();}
                const char *comment() const  {return _comment.c_str();}
                void setNodeType(const char *s)  {_nodetype = s;}
                void setComment(const char *s)  {_comment = s;}
                /** Allows update, too */
                StringMap& attrAssignments() {return _attrassignments;}
                bool operator==(const Subfilter& other) const;
        };

    private:
        std::string _name;
        std::string _description;
        bool _hidden;
        StringMap _attrs;  // _attrs[name] = description
        StringMap _defaults;  // _defaults[name] = default value

        typedef std::vector<Subfilter> Subfilters;
        Subfilters _subfilters;  // filters are now connected in cascade

    public:
        CompoundFilterType() {_hidden=false;}
        virtual ~CompoundFilterType() {}
        CompoundFilterType& operator=(const CompoundFilterType& other);
        bool equals(const CompoundFilterType& other);

        /** Name, description etc. */
        //@{
        virtual const char *name() const;
        virtual const char *category() const  {return "custom filter";}
        virtual const char *description() const;
        virtual bool isHidden() const {return _hidden;}
        virtual void setName(const char *);
        virtual void setDescription(const char *);
        virtual void setHidden(bool hidden) {_hidden=hidden;}
        //@}

        /** Creation */
        //@{
        virtual Node *create(DataflowManager *, StringMap& attrs) const;
        virtual Port *getPort(Node *node, const char *portname) const;
        //@}

        /** Attributes, ports */
        //@{
        virtual void getAttributes(StringMap& attrs) const;
        virtual void getAttrDefaults(StringMap& attrs) const;
        virtual void setAttr(const char *name, const char *desc, const char *defaultvalue);
        virtual void removeAttr(const char *name);
        //@}

        /** Subfilters */
        //@{
        virtual int numSubfilters() const;
        /**
         * Allows update too, via object reference
         */
        virtual Subfilter& subfilter(int pos);
        /**
         * Insert subfilter at given position (old filter at pos and following ones
         * are shifted up).
         */
        virtual void insertSubfilter(int pos, const Subfilter& f);
        /**
         * Remove subfilter at given position (following ones are shifted down).
         */
        virtual void removeSubfilter(int pos);
        //@}
};


#endif
