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

NAMESPACE_BEGIN


class SCAVE_API ResultItemField
{
	public:
		static const char * const FILE;
		static const char * const RUN;
		static const char * const MODULE;
		static const char * const NAME;
		
		static const int FILE_ID 		= 1;
		static const int RUN_ID			= 2;
		static const int MODULE_ID		= 3;
		static const int NAME_ID		= 4;
		static const int ATTR_ID		= 5;
		static const int RUN_ATTR_ID	= 6;
		static const int RUN_PARAM_ID	= 7;
	private:
        int id;
		std::string name; // constant names above + attribute/param names
        static int getFieldID(const std::string fieldName);
	public:
		ResultItemField(const std::string fieldName);
		const std::string getName() { return name; };
		std::string getFieldValue(const ResultItem &d);
		bool equal(const ResultItem &d1, const ResultItem &d2) const;
		int compare(const ResultItem &d1, const ResultItem &d2) const;
};

class SCAVE_API ResultItemAttribute
{
	public:
		static const char * const TYPE;
		static const char * const ENUM;
		
		static StringVector getAttributeNames();
		static bool isAttributeName(const std::string name);
};


class SCAVE_API RunAttribute
{
	public:
		static const char * const EXPERIMENT;
		static const char * const MEASUREMENT;
		static const char * const REPLICATION;
		static const char * const CONFIG;
		static const char * const RUNNUMBER;
		static const char * const NETWORKNAME;
		static const char * const DATETIME;
		
		static StringVector getAttributeNames();
		static bool isAttributeName(const std::string name);
};

inline const char *getAttribute(const ResultItem &d, const std::string attrName)
{
    const char *value = d.getAttribute(attrName.c_str()); 
    return value ? value : ""; 
}

inline const char *getRunAttribute(const ResultItem &d, const std::string attrName)
{
    const char *value = d.fileRunRef->runRef->getAttribute(attrName.c_str()); 
    return value ? value : ""; 
}

inline const char *getRunParam(const ResultItem &d, const std::string paramName)
{
    const char *value = d.fileRunRef->runRef->getModuleParam(paramName.c_str()); 
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
	case ATTR_ID:		return getAttribute(d, name);
	case RUN_ATTR_ID:	return getRunAttribute(d, name);
	case RUN_PARAM_ID:	return getRunParam(d, name);
	default:			return "";
	}
}

inline int strcmpFIXME(const char *str1, const char *str2)
{
    return strcmp(str1, str2);
}


inline bool ResultItemField::equal(const ResultItem &d1, const ResultItem &d2) const
{
	switch (id)
	{
	case FILE_ID:		return d1.fileRunRef->fileRef == d2.fileRunRef->fileRef;
	case RUN_ID:		return d1.fileRunRef->runRef == d2.fileRunRef->runRef;
	case MODULE_ID:		return d1.moduleNameRef == d2.moduleNameRef;
	case NAME_ID:		return d1.nameRef == d2.nameRef;
                        // KLUDGE using strcmp() here causes an INTERNAL COMPILER ERROR with MSVC71, i don't know why
	case ATTR_ID:	return strcmpFIXME(getAttribute(d1, name), getAttribute(d2, name)) == 0;
	case RUN_ATTR_ID:	return strcmpFIXME(getRunAttribute(d1, name), getRunAttribute(d2, name)) == 0;
	case RUN_PARAM_ID:	return strcmpFIXME(getRunParam(d1, name), getRunParam(d2, name)) == 0;
	}
	// not reached
	return true;
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
	case ATTR_ID:
		return strdictcmp(getAttribute(d1, name), getAttribute(d2, name));
	case RUN_ATTR_ID:
		return strdictcmp(getRunAttribute(d1, name), getRunAttribute(d2, name));
	case RUN_PARAM_ID:
		return strdictcmp(getRunParam(d1, name), getRunParam(d2, name));
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

#ifndef SWIG
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

NAMESPACE_END


#endif

