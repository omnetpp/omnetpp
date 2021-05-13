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

class QTENV_API LogInspector : public Inspector
{
    Q_OBJECT
   private:
      QAction *runUntilAction;
      QAction *fastRunUntilAction;

      QToolBar *createToolbar(bool isTopLevel);
      void addOwnActions(QToolBar *toolBar);

      QStringList gatherAllMessagePrinterTags();

   private slots:
      void runUntil();
      void fastRunUntil();
      void onFontChanged();

   public:
      enum Mode {LOG, MESSAGES};

   protected:
      static const QString PREF_COLUMNWIDTHS;
      static const QString PREF_MODE;
      static const QString PREF_EXCLUDED_MODULES;
      static const QString PREF_SAVE_FILENAME;
      static const QString PREF_MESSAGEPRINTER_TAGS;

      LogBuffer *logBuffer; // not owned
      ComponentHistory *componentHistory; // not owned
      TextViewerWidget *textWidget;
      ModuleOutputContentProvider *contentProvider; // owned by textWidget

      std::set<int> excludedModuleIds;
      cMessagePrinter::Options messagePrinterOptions;
      Mode mode;

      bool isMatchingComponent(int componentId);
      bool isAncestorModule(int componentId, int potentialAncestorModuleId);

      QString lastFindText;
      TextViewerWidget::FindOptions lastFindOptions;

      QAction *toMessagesModeAction;
      QAction *toLogModeAction;

      QAction *configureMessagePrinterAction;

      QSize sizeHint() const override { return QSize(700, 300); }

signals:
      void selectionChanged(cObject*);

   protected slots:
      void onFindButton(); // opens a dialog
      void findAgain(); // when F3 is pressed, uses the last set options
      void findAgainReverse(); // same, but in the other direction (with shift)

      void onFilterButton();
      void onMessagePrinterTagsButton();

      // displays the selected message in the object inspector if in messages mode
      void onCaretMoved(int lineIndex, int column);
      void onRightClicked(QPoint globalPos, int lineIndex, int column);

      void toMessagesMode();
      void toLogMode();

      void saveColumnWidths();
      void restoreColumnWidths();

      void saveExcludedModules();
      void restoreExcludedModules();

      void saveMessagePrinterOptions();
      void restoreMessagePrinterOptions();

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
