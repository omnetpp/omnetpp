//==========================================================================
//  LOGINSPECTOR.H - part of
//
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

#ifndef __OMNETPP_QTENV_LOGINSPECTOR_H
#define __OMNETPP_QTENV_LOGINSPECTOR_H

#include <set>
#include "omnetpp/cmessageprinter.h"
#include "logbuffer.h"
#include "componenthistory.h"
#include "inspector.h"
#include "textviewerwidget.h"

namespace omnetpp {
namespace qtenv {

class ModuleOutputContentProvider;
class LineFilteringContentProvider;

class QTENV_API LogInspector : public Inspector
{
    Q_OBJECT
   private:
      QAction *runUntilAction;
      QAction *fastRunUntilAction;

      QAction *toMessagesModeAction;
      QAction *toLogModeAction;

      QAction *findAction;
      QAction *saveAction;
      QAction *filterAction;
      QAction *clearFilterAction;

      QAction *findAgainAction;
      QAction *findAgainReverseAction;
      QAction *copySelectionAction;
      QAction *copySelectionWithFormattingAction;

      QAction *setBookmarkAction;
      QAction *goToBookmarkAction;

      QAction *goToSimTimeAction;
      QAction *goToEventAction;

      QAction *configureMessagePrinterAction;

      QToolBar *createToolbar(bool isTopLevel);
      void addOwnActions(QToolBar *toolBar);
      void updateFilterActionIcon();

      QStringList gatherAllMessagePrinterTags();

   private Q_SLOTS:
      void runUntil();
      void fastRunUntil();
      void onFontChanged();

      void onGlobalMessageFormatChanged();
      void onGlobalMessageFormatChangedBroadcast();

   public:
      enum Mode {LOG, MESSAGES};

   protected:
      static const QString PREF_COLUMNWIDTHS;
      static const QString PREF_MODE;
      static const QString PREF_EXCLUDED_MODULES;
      static const QString PREF_LINE_FILTER_STRING;
      static const QString PREF_LINE_FILTER_IS_REGEX;
      static const QString PREF_LINE_FILTER_IS_CASE_SENSITIVE;
      static const QString PREF_SAVE_FILENAME;
      static const QString PREF_MESSAGEPRINTER_TAGS;

      LogBuffer *logBuffer; // not owned
      ComponentHistory *componentHistory; // not owned
      TextViewerWidget *textWidget; // owned

      // If there is no line filtering active (`lineFilterString` is empty),
      // this is nullptr; otherwise, this is the direct content provider
      // for the text viewer widget, and owns `sourceContentProvider`.
      LineFilteringContentProvider *filteringContentProvider = nullptr;
      // If there is no line filtering active (`lineFilterString` is empty),
      // this is the direct content provider for the text viewer widget;
      // otherwise, this is owned by `filteringContentProvider`.
      ModuleOutputContentProvider *sourceContentProvider = nullptr;

      std::set<int> excludedModuleIds;
      std::string lineFilterString; // no line filtering if empty
      bool lineFilterIsRegex = false;
      bool lineFilterIsCaseSensitive = false;

      cMessagePrinter::Options messagePrinterOptions;
      Mode mode;

      bool isMatchingComponent(int componentId);
      bool isAncestorModule(int componentId, int potentialAncestorModuleId);

      QString lastFindText;
      SearchFlags lastFindOptions;

      QSize sizeHint() const override { return QSize(700, 300); }

  Q_SIGNALS:
      void selectionChanged(cObject*);

      // Emitted by any log inspector, whenever any of the options
      // that globally affect how messages are printed (digit grouping,
      // backward arrows, reference time) are changed in it by the user,
      // and handled by the embedded inspector.
      void globalMessageFormatChanged();
      // Emitted by the embedded log inspector, to notify every open inspector
      // (including itself) to refresh the text if it's in messages mode.
      void globalMessageFormatChangedBroadcast();

   protected Q_SLOTS:
      void onFindButton(); // opens a dialog
      void findAgain(); // when F3 is pressed, uses the last set options
      void findAgainReverse(); // same, but in the other direction (with shift)

      void onGoToSimTimeAction();
      void onGoToEventAction();

      void onSetBookmarkAction();
      void onGoToBookmarkAction();

      void goToSimTime(SimTime t);
      void goToEvent(eventnumber_t e);

      void onFilterButton();
      void onClearFilterButton();
      void onMessagePrinterTagsButton();

      // displays the selected message in the object inspector if in messages mode
      void onCaretMoved(int lineIndex, int column);
      void onRightClicked(QPoint globalPos, int lineIndex, int column);

      void toMessagesMode();
      void toLogMode();

      void saveColumnWidths();
      void restoreColumnWidths();

      void saveFilterSettings();
      void restoreFilterSettings();

      void saveMessagePrinterOptions();
      void restoreMessagePrinterOptions();

      void recreateProviders();

      void saveContent();

   public:
      LogInspector(QWidget *parent, bool isTopLevel, InspectorFactory *f);
      virtual ~LogInspector();
      virtual void doSetObject(cObject *obj) override;
      virtual void refresh() override;

      virtual Mode getMode() const {return mode;}
      virtual void setMode(Mode mode);
};

}  // namespace qtenv
}  // namespace omnetpp

#endif
