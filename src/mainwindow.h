/***********************************************************************/
/*                                                                     */
/* This file is part of the SSDE (Simple State Diagram Editor) package */
/*                                                                     */
/*  Copyright (c) 2019-present, Jocelyn SEROT (jocelyn.serot@uca.fr)   */
/*                       All rights reserved.                          */
/*                                                                     */
/*    This source code is licensed under the license found in the      */
/*      LICENSE file in the root directory of this source tree.        */
/*                                                                     */
/***********************************************************************/

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "state.h"
#include "properties.h"

#include <QMainWindow>
#include <QFrame>
#include "QGVScene.h"
#include "QGVNode.h"

class Model;

QT_BEGIN_NAMESPACE
class QAction;
class QToolBox;
class QSpinBox;
class QComboBox;
class QFontComboBox;
class QButtonGroup;
class QLineEdit;
class QGraphicsTextItem;
class QFont;
class QToolButton;
class QAbstractButton;
class QGraphicsView;
class SceneViewer;
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

  static int scene_width;
  static int scene_height;
  static double zoomInFactor;
  static double zoomOutFactor;
  static double minScaleFactor;
  static double maxScaleFactor;

public:
   MainWindow();

private slots:
    void toolButtonClicked(int id);
    void stateInserted(State *);
    void transitionInserted(Transition *);
    void stateSelected(State *);
    void transitionSelected(Transition *);
    void modelModified();
    void save();
    void saveAs();
    void openFile();
    void newDiagram();
    void quit();
    void about();
    void exportDot();
    void renderDot();
    void zoomIn();
    void zoomOut();

private:
    void createActions();
    void createMenus();
    void createToolbar();
    void createPropertiesPanel();

    void checkUnsavedChanges();
    void saveToFile(QString fname);

    void zoom(double factor);
    
    Model *model;
    double scaleFactor;

    QGraphicsView *editView;
    QGraphicsView *dotView;
    PropertiesPanel* properties_panel;

    QAction *newDiagramAction;
    QAction *openFileAction;
    QAction *saveFileAction;
    QAction *saveFileAsAction;
    QAction *aboutAction;
    QAction *exitAction;
    QAction *exportDotAction;
    QAction *renderDotAction;
    QAction *zoomInAction;
    QAction *zoomOutAction;

    QMenu *aboutMenu;
    QMenu *fileMenu;
    QMenu *dotMenu;

    QToolBar *toolBar;
    QButtonGroup *toolSet;

    bool unsaved_changes;
    QString currentFileName;

    static QString title;
    
public:
   Model* getModel() const { return model; }
    void setUnsavedChanges(bool unsaved_changes = true);

};

#endif // MAINWINDOW_H
