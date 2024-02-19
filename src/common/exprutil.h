//==========================================================================
//  EXPRUTIL.H  - part of
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

#ifndef __OMNETPP_COMMON_EXPRUTIL_H
#define __OMNETPP_COMMON_EXPRUTIL_H

#include <string>
#include "expression.h"

namespace omnetpp {
namespace common {
namespace expression {

typedef Expression::AstNode AstNode;

inline bool isOperator(AstNode *astNode) {
    return astNode->type == AstNode::OP;
}

inline bool isOperator(AstNode *astNode, const std::string& name) {
    return astNode->type == AstNode::OP && astNode->name == name;
}

inline bool isIdent(AstNode *astNode) {
    return astNode->type == AstNode::IDENT;
}

inline bool isIdent(AstNode *astNode, const std::string& name) {
    return astNode->type == AstNode::IDENT && astNode->name == name;
}

inline bool isMember(AstNode *astNode) {
    return astNode->type == AstNode::MEMBER;
}

inline bool isMember(AstNode *astNode, const std::string& name) {
    return astNode->type == AstNode::MEMBER && astNode->name == name;
}

inline bool isFunction(AstNode *astNode) {
    return astNode->type == AstNode::FUNCTION;
}

inline bool isFunction(AstNode *astNode, const std::string& name) {
    return astNode->type == AstNode::FUNCTION && astNode->name == name;
}

inline bool isFunction(AstNode *astNode, int argc) {
    return astNode->type == AstNode::FUNCTION && (int)astNode->children.size() == argc;
}

inline bool isIdentOrMember(AstNode *astNode) {
    return astNode->type == AstNode::IDENT || astNode->type == AstNode::MEMBER;
}

inline bool hasIndex(AstNode *astNode) {
    return astNode->type == AstNode::IDENT_W_INDEX || astNode->type == AstNode::MEMBER_W_INDEX;
}

inline bool hasChildren(AstNode *astNode, int minCount=1) {
    return (int)astNode->children.size() >= minCount;
}

inline AstNode *getIndexChild(AstNode *astNode) {
    return astNode->type == AstNode::IDENT_W_INDEX ? astNode->children.at(0) :
            astNode->type == AstNode::MEMBER_W_INDEX ? astNode->children.at(1) : nullptr;
}

inline bool isIdentOrIndexedIdent(AstNode *astNode) {
    return astNode->type == AstNode::IDENT || astNode->type == AstNode::IDENT_W_INDEX;
}

inline bool isMemberOrIndexedMember(AstNode *astNode) {
    return astNode->type == AstNode::MEMBER || astNode->type == AstNode::MEMBER_W_INDEX;
}

}  // namespace expression
}  // namespace common
}  // namespace omnetpp

#endif


