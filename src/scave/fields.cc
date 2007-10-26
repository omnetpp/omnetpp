//==========================================================================
//  FIELDS.CC - part of
//
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2005 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifdef _MSC_VER
#pragma warning(disable:4786)
#endif

#include <algorithm>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "commonutil.h"
#include "stringutil.h"
#include "fields.h"

using namespace std;

/*----------------------------------------
 *              RunAttribute
 *----------------------------------------*/
char *const RunAttribute::EXPERIMENT  = "experiment";
char *const RunAttribute::MEASUREMENT = "measurement";
char *const RunAttribute::REPLICATION = "replication";
char *const RunAttribute::CONFIG      = "config";
char *const RunAttribute::RUNNUMBER   = "run-number";
char *const RunAttribute::NETWORKNAME = "network";
char *const RunAttribute::DATETIME    = "dateTime";

StringVector RunAttribute::getAttributeNames()
{
    StringVector names = StringVector();
    names.push_back(EXPERIMENT);
    names.push_back(MEASUREMENT);
    names.push_back(REPLICATION);
    names.push_back(CONFIG);
    names.push_back(RUNNUMBER);
    names.push_back(NETWORKNAME);
    names.push_back(DATETIME);
    return names;
}

/*----------------------------------------
 *              ResultItemField
 *----------------------------------------*/
char *const ResultItemField::FILE   = "file";
char *const ResultItemField::RUN    = "run";
char *const ResultItemField::MODULE = "module";
char *const ResultItemField::NAME   = "name";

ResultItemField::ResultItemField(const string fieldName)
{
	this->id = getFieldID(fieldName);
	this->name = fieldName;
}

int ResultItemField::getFieldID(const string fieldName)
{
	if (fieldName == ResultItemField::FILE) return ResultItemField::FILE_ID;
    else if (fieldName == ResultItemField::RUN) return ResultItemField::RUN_ID;
    else if (fieldName == ResultItemField::MODULE) return ResultItemField::MODULE_ID;
    else if (fieldName == ResultItemField::NAME) return ResultItemField::NAME_ID;
    else return ResultItemField::RUN_ATTR_ID;
}


/*----------------------------------------
 *              ResultItemFields
 *----------------------------------------*/

StringVector ResultItemFields::getFieldNames()
{
    StringVector names = StringVector();
    names.push_back(ResultItemField::FILE);
    names.push_back(ResultItemField::RUN);
    names.push_back(ResultItemField::MODULE);
    names.push_back(ResultItemField::NAME);
    StringVector attrNames = RunAttribute::getAttributeNames();
    names.insert(names.end(), attrNames.begin(), attrNames.end());
    return names;
}

ResultItemFields::ResultItemFields(ResultItemField field)
{
	this->fields.push_back(field);
}

ResultItemFields::ResultItemFields(const string fieldName)
{
	this->fields.push_back(ResultItemField(fieldName));
}

ResultItemFields::ResultItemFields(const StringVector &fieldNames)
{
    for (StringVector::const_iterator fieldName = fieldNames.begin(); fieldName != fieldNames.end(); ++fieldName)
    	this->fields.push_back(ResultItemField(*fieldName));
}

bool ResultItemFields::hasField(ResultItemField field)
{
	return hasField(field.getName());
}

bool ResultItemFields::hasField(const string fieldName)
{
	for (vector<ResultItemField>::iterator field = fields.begin(); field != fields.end(); ++field)
		if (field->getName() == fieldName)
			return true;
	return false;
}

ResultItemFields ResultItemFields::complement()
{
	StringVector complementFields;
	StringVector fieldNames = getFieldNames();
	for (StringVector::iterator fieldName = fieldNames.begin(); fieldName != fieldNames.end(); ++fieldName)
		if (!hasField(*fieldName))
			complementFields.push_back(*fieldName);
	return ResultItemFields(complementFields);
}

bool ResultItemFields::equal(ID id1, ID id2, ResultFileManager *manager) const
{
    if (id1==-1 || id2==-1 || id1 == id2) return id1==id2;
    const ResultItem& d1 = manager->getItem(id1);
    const ResultItem& d2 = manager->getItem(id2);
    return equal(d1, d2);
}

bool ResultItemFields::equal(const ResultItem& d1, const ResultItem& d2) const
{
	for (vector<ResultItemField>::const_iterator field = fields.begin(); field != fields.end(); ++field)
		if (!field->equal(d1, d2))
			return false;
    return true;
}

bool ResultItemFields::less(ID id1, ID id2, ResultFileManager *manager) const
{
    if (id1==-1 || id2==-1) return id2!=-1; // -1 is the smallest
    if (id1 == id2) return false;
    const ResultItem& d1 = manager->getItem(id1);
    const ResultItem& d2 = manager->getItem(id2);
    return less(d1, d2);
}

bool ResultItemFields::less(const ResultItem &d1, const ResultItem &d2) const
{
	for (vector<ResultItemField>::const_iterator field = fields.begin(); field != fields.end(); ++field)
	{
		int cmp = field->compare(d1, d2); 
		if (cmp)
			return cmp < 0;
	}
    return false; // ==
}

