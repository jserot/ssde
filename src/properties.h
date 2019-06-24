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

#ifndef PROPERTIES_H
#define PROPERTIES_H

#include <QComboBox>
#include <QFrame>
#include <QGraphicsItem>
#include <QGroupBox>
#include <QLineEdit>
#include <QListView>
#include <QString>
#include <QStringListModel>

class MainWindow;
class State;
class Transition;

class PropertiesPanel : public QFrame
{
    Q_OBJECT

  private:
    MainWindow* main_window;

    QGraphicsItem* selected_item;

    QGroupBox* state_panel;
    QLineEdit* state_name_field;

    QGroupBox* transition_panel;
    QComboBox* transition_start_state_field;
    QComboBox* transition_end_state_field;
    QLineEdit* transition_label_field;

    QGroupBox* itransition_panel;
    QComboBox* itransition_end_state_field;
    QLineEdit* itransition_label_field;

  public:
    explicit PropertiesPanel(MainWindow* parent);
    ~PropertiesPanel();

    void unselectItem();
    void setSelectedItem(State* state);
    void setSelectedItem(Transition* transition);

    void toggleStimuliPanel();                                                    

  public slots:
    void setStateName(const QString& name);

    void setTransitionSrcState(int index);
    void setTransitionDstState(int index);
    void setTransitionLabel(const QString& label);
    void setITransitionDstState(int index);

    void clear();

  private:
    void createStatePanel();
    void createTransitionPanel();
    void createInitTransitionPanel();
};

#endif
