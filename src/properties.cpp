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

#include "properties.h"
#include "mainwindow.h"
#include "state.h"
#include "transition.h"
#include "fsd.h"

#include <QComboBox>
#include <QFrame>
#include <QGraphicsItem>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QListView>
#include <QString>
#include <QStringList>
#include <QStringListModel>
#include <QTextEdit>
#include <QVBoxLayout>
#include <stdexcept>
#include <QMessageBox>
#include <qDebug>

PropertiesPanel::PropertiesPanel(MainWindow* parent) : QFrame(parent)
{
    main_window = parent;

    createStatePanel();
    createTransitionPanel();
    createInitTransitionPanel();

    QVBoxLayout* layout = new QVBoxLayout;
    layout->setAlignment(Qt::AlignTop);

    layout->addWidget(state_panel);
    layout->addWidget(transition_panel);
    layout->addWidget(itransition_panel);

    this->setLayout(layout);

    connect(state_name_field, &QLineEdit::textEdited, this, &PropertiesPanel::setStateName);
    connect(transition_start_state_field, QOverload<int>::of(&QComboBox::activated), this, &PropertiesPanel::setTransitionSrcState);
    connect(transition_end_state_field, QOverload<int>::of(&QComboBox::activated), this, &PropertiesPanel::setTransitionDstState);
    connect(transition_label_field, &QLineEdit::textEdited, this, &PropertiesPanel::setTransitionLabel);
    connect(itransition_end_state_field, QOverload<int>::of(&QComboBox::activated), this, &PropertiesPanel::setITransitionDstState);
    connect(itransition_label_field, &QLineEdit::textEdited, this, &PropertiesPanel::setTransitionLabel);
}

PropertiesPanel::~PropertiesPanel()
{
}


void PropertiesPanel::createStatePanel()
{
    state_panel = new QGroupBox("State properties");
    QVBoxLayout* statePanelLayout = new QVBoxLayout();

    QLabel* nameLabel = new QLabel("Name");
    state_name_field = new QLineEdit();
    statePanelLayout->addWidget(nameLabel);
    statePanelLayout->addWidget(state_name_field);

    state_panel->setLayout(statePanelLayout);

    state_panel->hide();
}

void PropertiesPanel::createTransitionPanel()
{
    transition_panel = new QGroupBox("Transition properties");
    QVBoxLayout* transitionLayout = new QVBoxLayout();

    QLabel* startLabel = new QLabel("Start State");
    transition_start_state_field = new QComboBox();
    transitionLayout->addWidget(startLabel);
    transitionLayout->addWidget(transition_start_state_field);
    QLabel* endLabel = new QLabel("End State");
    transition_end_state_field = new QComboBox();
    transitionLayout->addWidget(endLabel);
    transitionLayout->addWidget(transition_end_state_field);

    QLabel* labelLabel = new QLabel("Label");
    transition_label_field = new QLineEdit();
    transitionLayout->addWidget(labelLabel);
    transitionLayout->addWidget(transition_label_field);

    transition_panel->setLayout(transitionLayout);

    transition_panel->hide();
}

void PropertiesPanel::createInitTransitionPanel()
{
    itransition_panel = new QGroupBox("Transition properties");
    QVBoxLayout* transitionLayout = new QVBoxLayout();

    QLabel* endLabel = new QLabel("End State");
    itransition_end_state_field = new QComboBox();
    transitionLayout->addWidget(endLabel);
    transitionLayout->addWidget(itransition_end_state_field);

    QLabel* labelLabel = new QLabel("Label");
    itransition_label_field = new QLineEdit();
    transitionLayout->addWidget(labelLabel);
    transitionLayout->addWidget(itransition_label_field);

    itransition_panel->setLayout(transitionLayout);
    itransition_panel->hide();
}

void PropertiesPanel::unselectItem()
{
    selected_item = nullptr;
    state_panel->hide();
    transition_panel->hide();
    itransition_panel->hide();
}

void PropertiesPanel::setSelectedItem(State* state)
{
    qDebug() << "State " << state->getId() << " selected";
    transition_panel->hide();
    itransition_panel->hide();
    if ( ! state->isPseudo() ) {
      selected_item = state;
      state_panel->show();
      state_name_field->setText(state->getId());
      }
}

void PropertiesPanel::setSelectedItem(Transition* transition)
{
    qDebug() << "Transition " << transition->srcState()->getId() << "->" << transition->dstState()->getId()
             << "[" << transition->getLabel() << "]" << " selected";
    selected_item = transition;
    state_panel->hide();

    if ( transition->isInitial() ) {
      transition_panel->hide();
      itransition_panel->show();
      itransition_end_state_field->clear();
      foreach ( State* state, main_window->getFsd()->states() ) {
        if ( ! state->isPseudo() ) {
          QString id = state->getId();
          itransition_end_state_field->addItem(id, QVariant(id));
          if ( transition->dstState()->getId() == id ) 
            itransition_end_state_field->setCurrentIndex(itransition_end_state_field->count()-1);
          }
        }
      itransition_label_field->setText(transition->getLabel());
      }
    else {
      itransition_panel->hide();
      transition_panel->show();
      transition_start_state_field->clear();
      transition_end_state_field->clear();
      foreach ( State* state, main_window->getFsd()->states() ) {
        if ( ! state->isPseudo() ) {
          QString id = state->getId();
          transition_start_state_field->addItem(id, QVariant(id));
          transition_end_state_field->addItem(id, QVariant(id));
          if ( transition->srcState()->getId() == id )
            transition_start_state_field->setCurrentIndex(transition_start_state_field->count()-1);
          if ( transition->dstState()->getId() == id ) 
            transition_end_state_field->setCurrentIndex(transition_end_state_field->count()-1);
          }
        }
      transition_label_field->setText(transition->getLabel());
    }
}

void PropertiesPanel::setStateName(const QString& name)
{
    State* state = qgraphicsitem_cast<State*>(selected_item);
    if(state != nullptr) {
        state->setId(name);
        main_window->getFsd()->update();
        main_window->setUnsavedChanges(true);
    }
}

void PropertiesPanel::setTransitionSrcState(int index)
{
  if ( index == -1 ) return;
  Transition* transition = qgraphicsitem_cast<Transition*>(selected_item);
  if ( transition == nullptr ) return;
  QString state_id = transition_start_state_field->itemText(index);
  State* state = main_window->getFsd()->getState(state_id);
  if ( state == nullptr )
    throw std::invalid_argument(std::string("No state found with id : ") + state_id.toStdString());
  transition->setSrcState(state);
  main_window->getFsd()->update();
  main_window->setUnsavedChanges(true);
}

void PropertiesPanel::setTransitionDstState(int index)  // TODO: factorize with above code
{
  if ( index == -1 ) return;
  Transition* transition = qgraphicsitem_cast<Transition*>(selected_item);
  if ( transition == nullptr ) return;
  QString state_id = transition_end_state_field->itemText(index);
  State* state = main_window->getFsd()->getState(state_id);
  if ( state == nullptr )
    throw std::invalid_argument(std::string("No state found with id : ") + state_id.toStdString());
  transition->setDstState(state);
  main_window->getFsd()->update();
  main_window->setUnsavedChanges(true);
}

void PropertiesPanel::setITransitionDstState(int index)  // TODO: factorize with above code
{
  if ( index == -1 ) return;
  Transition* transition = qgraphicsitem_cast<Transition*>(selected_item);
  if ( transition == nullptr ) return;
  QString state_id = itransition_end_state_field->itemText(index);
  State* state = main_window->getFsd()->getState(state_id);
  if ( state == nullptr )
    throw std::invalid_argument(std::string("No state found with id : ") + state_id.toStdString());
  transition->setDstState(state);
  main_window->getFsd()->update();
  main_window->setUnsavedChanges(true);
}

void PropertiesPanel::setTransitionLabel(const QString& label)
{
  Transition* transition = qgraphicsitem_cast<Transition*>(selected_item);
  if ( transition == nullptr ) return;
  transition->setLabel(label);
  main_window->getFsd()->update();
  main_window->setUnsavedChanges(true);
}

void PropertiesPanel::clear()
{
    selected_item = nullptr;

    state_panel->hide();
    transition_panel->hide();
    itransition_panel->hide();
}
