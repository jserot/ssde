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

#include "model.h"
#include "transition.h"
#include "include/nlohmann_json.h"
#include <QMessageBox>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsScene>
#include <QGraphicsView>
#include "QGVScene.h"
#include "QGVNode.h"
#include "QGVEdge.h"
#include <QDebug>

int Model::stateCounter = 0;
QColor Model::lineColor = Qt::lightGray;
QColor Model::boxColor = Qt::black;

Model::Model(QWidget *parent)
    : QGraphicsScene(parent)
{
    mode = SelectItem;
    mainWindow = parent;
    scene = NULL;
}

void Model::setMode(Mode mode)
{
    this->mode = mode;
}

Model::Mode Model::getMode(void)
{
  return mode;
}

State* Model::addState(QPointF pos, QString id)
{
  State* state = new State(id);
  state->setBrush(boxColor);
  addItem(state);
  state->setPos(pos);
  return state;
}

State* Model::addPseudoState(QPointF pos)
{
   State* state = new State();
   state->setBrush(boxColor);
   addItem(state);
   state->setPos(pos);
   return state;
}

QList<State*> Model::states()
{
  QList<State*> states;
  for ( const auto item: items() )
    if ( item->type() == State::Type )
      states.append(qgraphicsitem_cast<State *>(item));
  return states;
}

QList<Transition*> Model::transitions()
{
  QList<Transition*> transitions;
  for ( const auto item: items() )
    if ( item->type() == Transition::Type )
      transitions.append(qgraphicsitem_cast<Transition *>(item));
  return transitions;
}

State* Model::getState(QString id)
{
  foreach ( State* s, states() )
    if ( s->getId() == id ) return s;
  return NULL;
}

bool Model::hasPseudoState()
{
  foreach ( State* s, states() )
    if ( s->isPseudo() ) return true;
  return false;
}

Transition* Model::addTransition(State* srcState, State* dstState, QString label, State::Location location)
{
  Transition *transition = new Transition(srcState, dstState, label, location);
  srcState->addTransition(transition);
  if ( dstState != srcState ) dstState->addTransition(transition); // Do not add self-transitions twice !
  transition->setZValue(-1000.0);
  addItem(transition);
  return transition;
}

bool Model::event(QEvent *event)
{
  //qDebug() << "Got event " << event->type();
  switch ( event->type() ) {
    // Note. The [Enter] and [Leave] events cannot be handled by the model itself
    // because the associated action [setCursor] can only be applied to the _enclosing_ view...
    // This workaround uses signals to delegate 
    case QEvent::Enter:
      qDebug() << "Entering scene ";
      emit mouseEnter();
      return true;
    case QEvent::GraphicsSceneLeave:
      qDebug() << "Leaving scene";
      emit mouseLeave();
      return true;
    default:
        break;
    }
  return QGraphicsScene::event(event);
}

void Model::mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
    if (mouseEvent->button() != Qt::LeftButton) return;
    State *state;
    Transition *transition;
    QGraphicsItem *item;
    switch ( mode ) {
        case InsertState:
            state = addState(mouseEvent->scenePos(), QString::number(stateCounter++));
            //emit stateInserted(state);
            emit modelModified();
            break;
        case InsertPseudoState:
          if ( ! hasPseudoState() ) {
            startState = addPseudoState(mouseEvent->scenePos());
            line = new QGraphicsLineItem(QLineF(mouseEvent->scenePos(), mouseEvent->scenePos()));
            line->setPen(QPen(lineColor, 2));
            addItem(line);
            emit modelModified();
            }
          else
            QMessageBox::warning(mainWindow, "Error",
                                 "There's alreay one initial transition !\nDelete it first to add another one");
          break;
        case InsertTransition:
            line = new QGraphicsLineItem(QLineF(mouseEvent->scenePos(), mouseEvent->scenePos()));
            line->setPen(QPen(lineColor, 2));
            addItem(line);
            emit modelModified();
            break;
        case InsertLoopTransition:
          item = itemAt(mouseEvent->scenePos(), QTransform());
          if ( item != NULL && item->type() == State::Type ) {
            state = qgraphicsitem_cast<State *>(item);
            if ( ! state->isPseudo() ) {
              State::Location location = state->locateEvent(mouseEvent);
              Transition *transition = addTransition(state, state, "", location);
              transition->updatePosition();
              emit modelModified();
              //emit transitionInserted(transition);
              }
            }
            break;
        case DeleteItem:
          item = itemAt(mouseEvent->scenePos(), QTransform());
          if ( item != NULL ) {
            switch ( item->type() ) {
            case Transition::Type: {
              transition = qgraphicsitem_cast<Transition *>(item);
              State *srcState = transition->srcState();
              State *dstState = transition->dstState();
              if ( srcState->isPseudo() ) {
                srcState->removeTransitions();
                removeItem(srcState);
                }
              else {
                srcState->removeTransition(transition);
                dstState->removeTransition(transition);
                removeItem(item);
                delete item;
                }
              emit modelModified();
              }
              break;
            case State::Type:
              state = qgraphicsitem_cast<State *>(item);
              state->removeTransitions();
              removeItem(item);
              emit modelModified();
              delete item;
              break;
            default:
              break;
            }
          }
          break;
        case SelectItem:
          item = itemAt(mouseEvent->scenePos(), QTransform());
          if ( item != NULL ) {
            switch ( item->type() ) {
              case State::Type:
                emit(stateSelected(qgraphicsitem_cast<State *>(item)));
                break;
              case Transition::Type:
                emit(transitionSelected(qgraphicsitem_cast<Transition *>(item)));
                break;
              default:
                break;
              }
            }
          QGraphicsScene::mousePressEvent(mouseEvent);
          break;
       }
}

void Model::mouseMoveEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
  if ( (mode == InsertTransition || mode == InsertPseudoState) && line != 0 ) {
    QLineF newLine(line->line().p1(), mouseEvent->scenePos());
    line->setLine(newLine);
    }
  else if (mode == SelectItem) {
    QGraphicsScene::mouseMoveEvent(mouseEvent);
    }
}

void Model::mouseReleaseEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
  if ( line != 0 && (mode == InsertTransition || mode == InsertPseudoState) ) {
    QList<QGraphicsItem *> srcStates = items(line->line().p1());
    if (srcStates.count() && srcStates.first() == line) srcStates.removeFirst();
    QList<QGraphicsItem *> dstStates = items(line->line().p2());
    if (dstStates.count() && dstStates.first() == line) dstStates.removeFirst();
    removeItem(line);
    delete line;
    if (srcStates.count() > 0 && dstStates.count() > 0 &&
        srcStates.first()->type() == State::Type &&
        dstStates.first()->type() == State::Type) {
      State *srcState = qgraphicsitem_cast<State *>(srcStates.first());
      State *dstState = qgraphicsitem_cast<State *>(dstStates.first());
      if ( srcState != dstState ) {
        State::Location location = srcState == dstState ? srcState->locateEvent(mouseEvent) : State::None;
        Transition *transition = addTransition(srcState, dstState, "", location);
        transition->updatePosition();
        // emit transitionInserted(transition);
        emit modelModified();
        }
      }
    else if ( mode == InsertPseudoState && startState != NULL ) {
      // An initial pseudo-state has been created but not connected
      removeItem(startState);
      delete startState;
      }
    }
  line = 0;
  QGraphicsScene::mouseReleaseEvent(mouseEvent);
}

bool Model::isItemChange(int type)
{
    foreach (QGraphicsItem *item, selectedItems()) {
        if (item->type() == type)
            return true;
    }
    return false;
}

void Model::fromString(QString& json_text)
{
    auto json = nlohmann::json::parse(json_text.toStdString());

    clear(); 
    
    QMap<std::string, State*> states;
    stateCounter = 0;

    for ( auto json_state : json.at("states") ) {
      std::string id = json_state.at("id");
      State* state;
      if ( id == State::initPseudoId.toStdString() )
        state = addPseudoState(QPointF(json_state.at("x"), json_state.at("y")));
      else 
        state = addState(QPointF(json_state.at("x"), json_state.at("y")), QString::fromStdString(id));
      states.insert(id, state);
      stateCounter++;
      }   

    for ( auto json_transition : json.at("transitions") ) {
      std::string src_state = json_transition.at("src_state");
      std::string dst_state = json_transition.at("dst_state");
      std::string label = json_transition.at("label");
      State::Location location;
      switch ( (int)json_transition.at("location") ) {
        case 1: location = State::North; break;
        case 2: location = State::South; break;
        case 3: location = State::East; break;
        case 4: location = State::West; break;
        default: location = State::None; break;
        }
      if ( ! states.contains(src_state) || ! states.contains(dst_state) )
        throw std::invalid_argument("Model::fromString: invalid state id");
      State *srcState = states.value(src_state);
      State *dstState = states.value(dst_state);
      Transition *transition = addTransition(srcState, dstState, QString::fromStdString(label), location);
      transition->updatePosition();
      }
}

QString Model::toString()
{
    nlohmann::json json_res;

    json_res["states"] = nlohmann::json::array();
    for ( const auto item: items() ) {
      if ( item->type() == State::Type ) {
        State* state = qgraphicsitem_cast<State *>(item);
        nlohmann::json json;
        json["id"] = state->getId().toStdString(); 
        json["x"] = state->scenePos().x(); 
        json["y"] = state->scenePos().y(); 
        json_res["states"].push_back(json);
        }
      }

    json_res["transitions"] = nlohmann::json::array();
    for ( const auto item: items() ) {
      if ( item->type() == Transition::Type ) {
        Transition* transition = qgraphicsitem_cast<Transition *>(item);
        nlohmann::json json;
        json["src_state"] = transition->srcState()->getId().toStdString();
        json["dst_state"] = transition->dstState()->getId().toStdString();
        json["label"] = transition->getLabel().toStdString();
        json["location"] = transition->location();
        json_res["transitions"].push_back(json);
        }
      }
    return QString::fromStdString(json_res.dump(2));
}

QString dotTransitionLabel(QString label, QString lrpad)
{
  QStringList l = label.split("/");
  if ( l.length() != 2 ) return label;
  int n = std::max(l.at(0).length(), l.at(1).length());
  return lrpad + l.at(0) + lrpad
       + "\n" + lrpad + QString(n, '_') + lrpad  + "\n"
       + lrpad + l.at(1) + lrpad;
}

void Model::exportDot(QString fname)
{
  QFile file(fname);
  file.open(QIODevice::WriteOnly | QIODevice::Text);
  if ( file.error() != QFile::NoError ) {
    QMessageBox::warning(mainWindow, "","Cannot open file " + file.fileName());
    return;
  }
  QTextStream os(&file);
  os << "digraph main {\n";
  os << "layout = dot\n";
  os << "rankdir = UD\n";
  os << "size = \"8.5,11\"\n";
  os << "center = 1\n";
  os << "nodesep = \"0.350000\"\n";
  os << "ranksep = \"0.400000\"\n";
  os << "fontsize = 14\n";
  os << "mindist=1.0\n";
  for ( const auto item: items() ) {
    if ( item->type() == State::Type ) {
      State* state = qgraphicsitem_cast<State *>(item);
      QString id = state->getId();
      if ( state->isPseudo() ) 
      os << id << " [shape=point]\n";
      else
        os << id << " [label=\"" << id << "\", shape=circle, style=solid]\n";
      }
    }
  for ( const auto item: items() ) {
    if ( item->type() == Transition::Type ) {
      Transition* transition = qgraphicsitem_cast<Transition *>(item);
      QString src_id = transition->srcState()->getId();
      QString dst_id = transition->dstState()->getId();
      QString label = dotTransitionLabel(transition->getLabel(),"");
      if ( transition->isInitial() ) 
        os << src_id << " -> " << dst_id << "\n";
      else
        os << src_id << " -> " << dst_id << " [label=\"" << label << "\"]\n";
      }
    }
  os << "}\n";
}

void Model::renderDot(QGraphicsView *view, int width, int height)
{
  // TODO: factorize code with exportDot (?)
  // Build scene
  if ( scene ) delete scene; // QGraphicScene::clear does not reset the scene state :(
  QGVScene* scene = new QGVScene("DOT", this);
  scene->setSceneRect(QRectF(0, 0, width, height));
  
  scene->setGraphAttribute("rankdir", "UD");
  scene->setGraphAttribute("nodesep", "0.55");
  scene->setGraphAttribute("ranksep", "0.95");
  scene->setGraphAttribute("fontsize", "14");
  scene->setGraphAttribute("mindist", "1.0");
  scene->setNodeAttribute("shape", "circle");
  scene->setNodeAttribute("style", "solid");

  QMap<QString,QGVNode*> nodes;

  for ( const auto item: items() ) {
    if ( item->type() == State::Type ) {
      State* state = qgraphicsitem_cast<State *>(item);
      QString id = state->getId();
      QGVNode *node = scene->addNode(id);
      if ( state->isPseudo() ) {
        node->setAttribute("shape", "none"); 
        node->setAttribute("label", "");
        }
      nodes.insert(id,node);
      }
    }
  for ( const auto item: items() ) {
    if ( item->type() == Transition::Type ) {
      Transition* transition = qgraphicsitem_cast<Transition *>(item);
      QString src_id = transition->srcState()->getId();
      QString dst_id = transition->dstState()->getId();
      QString label = transition->isInitial() ? "" : dotTransitionLabel(transition->getLabel(),"  ");
      if ( nodes.contains(src_id) && nodes.contains(dst_id) ) {
        scene->addEdge(nodes[src_id], nodes[dst_id], label);
        }
      }
    }
  // Render it in the specified view
  scene->applyLayout();
  view->setScene(scene);
  view->ensureVisible(scene->itemsBoundingRect());
}

