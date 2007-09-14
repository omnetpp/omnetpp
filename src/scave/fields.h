//==========================================================================
//  FIELDS.H - part of
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

#ifndef _FIELDS_H_
#define _FIELDS_H_

#include <functional>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "resultfilemanager.h"
#include "commonutil.h"
#include "patternmatcher.h"
#include "stringutil.h"
#include "scaveutils.h"


class SCAVE_API ResultItemField
{
	public:
		static char *const FILE;
		static char *const RUN;
		static char *const MODULE;
		static char *const NAME;
		
		static const int FILE_ID 		= 1;
		static const int RUN_ID			= 2;
		static const int MODULE_ID		= 3;
		static const int NAME_ID		= 4;
		static const int RUN_ATTR_ID	= 5;
	private:
        int id;
		std::string name; // constant names above + run attribute names
        static int getFieldID(const std::string fieldName);
	public:
		ResultItemField(const std::string fieldName);
		const std::string getName() { return name; };
		std::string getFieldValue(const ResultItem &d);
		bool equal(const ResultItem &d1, const ResultItem &d2) const;
		int compare(const ResultItem &d1, const ResultItem &d2) const;
};


class SCAVE_API RunAttribute
{
	public:
		static char *const EXPERIMENT;
		static char *const MEASUREMENT;
		static char *const REPLICATION;
		static char *const CONFIG;
		static char *const RUNNUMBER;
		static char *const NETWORKNAME;
		static char *const DATETIME;
		
		static StringVector getAttributeNames();
};

inline const char *getAttribute(const ResultItem &d, const std::string attrName)
{
	const char *value = d.fileRunRef->runRef->getAttribute(attrName.c_str()); 
    return value ? value : ""; 
}

inline std::string ResultItemField::getFieldValue(const ResultItem &d)
{
	switch(id)
	{
	case FILE_ID:		return d.fileRunRef->fileRef->filePath;
	case RUN_ID:		return d.fileRunRef->runRef->runName;
	case MODULE_ID:		return *d.moduleNameRef;
	case NAME_ID:		return *d.nameRef;
	case RUN_ATTR_ID:	return getAttribute(d, name);
	default:			return "";
	}
}

inline bool ResultItemField::equal(const ResultItem &d1, const ResultItem &d2) const
{
	switch (id)
	{
	case FILE_ID:		return d1.fileRunRef->fileRef != d2.fileRunRef->fileRef;
	case RUN_ID:		return d1.fileRunRef->runRef != d2.fileRunRef->runRef;
	case MODULE_ID:		return d1.moduleNameRef != d2.moduleNameRef;
	case NAME_ID:		return d1.nameRef != d2.nameRef;
	case RUN_ATTR_ID:	return strcmp(getAttribute(d1, name), getAttribute(d2, name)) == 0;
	default:			return true;
	}
}

inline int ResultItemField::compare(const ResultItem &d1, const ResultItem &d2) const
{
	switch (id)
	{
	case FILE_ID:
		return strdictcmp(d1.fileRunRef->fileRef->filePath.c_str(),
				          d2.fileRunRef->fileRef->filePath.c_str());
	case RUN_ID:
		return strdictcmp(d1.fileRunRef->runRef->runName.c_str(),
				          d2.fileRunRef->runRef->runName.c_str());
	case MODULE_ID:
		return strdictcmp(d1.moduleNameRef->c_str(),
				          d2.moduleNameRef->c_str());
	case NAME_ID:
		return strdictcmp(d1.nameRef->c_str(), d2.nameRef->c_str());
	case RUN_ATTR_ID:
		return strdictcmp(getAttribute(d1, name), getAttribute(d2, name));
	default:
		return false;
	}
}

class SCAVE_API ResultItemFields
{
    private:
        std::vector<ResultItemField> fields;
    public:
        static StringVector getFieldNames();

        ResultItemFields() {};
        ResultItemFields(ResultItemField field);
        ResultItemFields(const std::string fieldName);
        ResultItemFields(const StringVector &fieldNames);
        
        ResultItemFields complement();
        bool hasField(ResultItemField field);
        bool hasField(const std::string fieldName);
        bool less(ID id1, ID id2, ResultFileManager *manager) const;
        bool less(const ResultItem &d1, const ResultItem &d2) const;
        bool equal(ID id1, ID id2, ResultFileManager *manager) const;
        bool equal(const ResultItem& d1, const ResultItem& d2) const;
};

/*
 * Equality test of ResultItems that compares the specified fields.
 */
struct ResultItemFieldsEqual : public std::binary_function<ResultItem, ResultItem, bool>
{
    ResultItemFields fields;
    ResultItemFieldsEqual(const ResultItemFields fields) : fields(fields) {}
    bool operator()(const ResultItem &d1, const ResultItem &d2) const { return fields.equal(d1, d2); }
};

struct IDFieldsEqual : public std::binary_function<ID,ID,bool>
{
    ResultItemFields fields;
    ResultFileManager *manager;
    IDFieldsEqual(ResultItemFields fields, ResultFileManager *manager)
        : fields(fields), manager(manager) {}
    bool operator()(ID id1, ID id2) const { return fields.equal(id1, id2, manager); }
};

/*
 * Comparator of ResultItems that compares the specified fields.
 */
struct ResultItemFieldsLess : public std::binary_function<ResultItem, ResultItem, bool>
{
    ResultItemFields fields;
    ResultItemFieldsLess() {}
    ResultItemFieldsLess(const ResultItemFields fields) : fields(fields) {}
    bool operator()(const ResultItem &d1, const ResultItem &d2) const { return fields.less(d1, d2); }
};

struct IDFieldsLess : public std::binary_function<ID,ID,bool>
{
	ResultItemFields fields;
	ResultFileManager *manager;
    IDFieldsLess(ResultItemFields fields, ResultFileManager *manager)
        : fields(fields), manager(manager) {}
    bool operator()(ID id1, ID id2) const { return fields.less(id1, id2, manager); }
};

#endif

