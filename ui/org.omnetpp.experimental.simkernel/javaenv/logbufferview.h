//==========================================================================
//  LOGBUFFERVIEW.H - part of
//
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2008 Andras Varga
  Copyright (C) 2006-2008 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __LOGBUFFERVIEW_H
#define __LOGBUFFERVIEW_H

#include <set>
#include "logbuffer.h"

NAMESPACE_BEGIN

/**
 * This class is the underlying implementation for the Module Output View
 * of the runtime env.
 *
 * This class provides a filtered view of a LogBuffer. Filtering is done by
 * module IDs. When you specify a module ID, messages from that module and
 * all submodules (ie the submodule tree) will be show, except those specified
 * in the excludedModuleIDs set.
 *
 * Provides a line-based query interface. The API allows random access, but
 * the implementation, being interator-based, is optimized for accessing
 * contiguous ranges of lines.
 *
 */
class LogBufferView : private LogBuffer::IListener
{
    private:
      LogBuffer *log;

      // display messages from the "root" module's module tree,
      // except those from modules in excludedModuleIds.
      int rootModuleId;
      std::set<int> excludedModuleIds;

      // total size of filtered view
      size_t totalLines;
      size_t totalChars;  // including newlines

      // current position
      bool currentPosValid;
      size_t currentLineIndex; // 0-based
      size_t currentLineOffset; // always points to beginning of line
      std::list<LogBuffer::Entry>::const_iterator currentEntry;
      size_t entryLineNo;
      size_t entryLineOffset;

    private:
      void countLines();
      void verifyTotals();
      bool isGood(const LogBuffer::Entry& entry) const;
      void gotoLine(size_t lineIndex);
      void gotoOffset(size_t offset);
      void gotoBeginning();
      void gotoEnd();
      void gotoNextLineInEntry();
      void gotoPreviousLineInEntry();
      void incCurrentEntry();
      void decCurrentEntry();

      void entryAdded(const LogBuffer::Entry& entry);
      void lineAdded(const LogBuffer::Entry& entry, size_t numLineChars);
      void discardingEntry(const LogBuffer::Entry& entry);

    public:
      LogBufferView(LogBuffer *log, int moduleId, const std::vector<int>& excludedModuleIds);
      ~LogBufferView();

      size_t getNumLines() {return totalLines;}
      size_t getNumChars() {return totalChars;}

      const char *getLine(size_t lineIndex);
      int getLineType(size_t lineIndex); // LINE_INFO, LINE_BANNER, LINE_LOG
      size_t getLineAtOffset(size_t offset);
      size_t getOffsetAtLine(size_t lineIndex);
};

NAMESPACE_END


#endif


