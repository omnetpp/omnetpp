//=========================================================================
//  COMPOUNDFILTER.H - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//  Author: Andras Varga
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2017 Andras Varga
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __OMNETPP_SCAVE_COMPOUNDFILTER_H
#define __OMNETPP_SCAVE_COMPOUNDFILTER_H

#include <string>
#include <vector>

#include "nodetype.h"
#include "commonnodes.h"

namespace omnetpp {
namespace scave {


class CompoundFilterType;


/**
 * FilterNode nodes combined into a single filter by cascading them.
 */
class SCAVE_API CompoundFilter : public FilterNode
{
        friend class CompoundFilterType;
    protected:
        FilterNode *first;
        FilterNode *last;
        StringMap attrValues;  // _attrs[name] = value
    public:
        CompoundFilter() {}
        virtual ~CompoundFilter() {}
        FilterNode *getFirstNode() {return first;}
        FilterNode *getLastNode() {return last;}

        virtual bool isReady() const override  {return false;}
        virtual void process() override  {}
        virtual bool isFinished() const override {return true;}
};


/**
 * FilterNode nodes combined into a single filter by cascading them.
 */
class SCAVE_API CompoundFilterType : public FilterNodeType
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
                const char *getNodeType() const  {return _nodetype.c_str();}
                const char *getComment() const  {return _comment.c_str();}
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

        /** @name Name, description etc. */
        //@{
        virtual const char *getName() const override;
        virtual const char *getCategory() const override  {return "custom filter";}
        virtual const char *getDescription() const override;
        virtual bool isHidden() const override {return _hidden;}
        virtual void setName(const char *);
        virtual void setDescription(const char *);
        virtual void setHidden(bool hidden) {_hidden=hidden;}
        //@}

        /** @name Creation */
        //@{
        virtual Node *create(DataflowManager *mgr, StringMap& attrs) const override;
        virtual Port *getPort(Node *node, const char *portname) const override;
        //@}

        /** @name Attributes, ports */
        //@{
        virtual void getAttributes(StringMap& attrs) const override;
        virtual void getAttrDefaults(StringMap& attrs) const override;
        virtual void setAttr(const char *name, const char *desc, const char *defaultvalue);
        virtual void removeAttr(const char *name);
        //@}

        /** @name Subfilters */
        //@{
        virtual int getNumSubfilters() const;
        /**
         * Allows update too, via object reference
         */
        virtual Subfilter& getSubfilter(int pos);
        /**
         * Access subfilter.
         */
        virtual const Subfilter& getSubfilter(int pos) const;
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

        /** @name Vector attribute mapping */
        //@{
        /**
         * Maps attributes of the input vector to attributes of the output vector.
         */
        virtual void mapVectorAttributes(/*inout*/StringMap& attrs, /*out*/StringVector& warnings) const override;
        //@}
};


} // namespace scave
}  // namespace omnetpp


#endif
