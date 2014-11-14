/*
 *  Copyright (C) 2014 Marcel Lehwald
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.

 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <statemachine.h>
#include <QDebug>

using namespace hfsmexec;

/*
 * AbstractEvent
 */
AbstractEvent::AbstractEvent(Type type) :
    QEvent(type)
{

}

/*
 * AbstractTransition
 */
AbstractTransition::AbstractTransition(const QString transitionId, const QString sourceStateId, const QString targetStateId) :
        transitionId(transitionId),
        sourceStateId(sourceStateId),
        targetStateId(targetStateId),
        sourceState(NULL),
        targetState(NULL),
        stateMachine(NULL)
{
}

AbstractTransition::~AbstractTransition()
{

}

const QString& AbstractTransition::getId() const
{
    return transitionId;
}

AbstractState* AbstractTransition::getSourceState() const
{
    return sourceState;
}

AbstractState *AbstractTransition::getTargetState() const
{
    return targetState;
}

StateMachine* AbstractTransition::getStateMachine()
{
    return stateMachine;
}

bool AbstractTransition::initialize()
{
    if (sourceState == NULL)
    {
        qWarning() <<"transition initialization failed: couldn't find source state \"" <<sourceStateId <<"\"";

        return false;
    }

    if (targetState == NULL)
    {
        qWarning() <<"transition initialization failed: couldn't find target state \"" <<targetStateId <<"\"";

        return false;
    }

    //cast abstract state
    QState* state = dynamic_cast<QState*>(sourceState->getDelegate());
    if (state == NULL)
    {
        qWarning() <<"transition initialization failed: source delegate is not of type QState";

        return false;
    }

    qDebug() <<"add transition from " <<sourceState->getId() <<" to " <<targetState->getId();

    setTargetState(targetState->getDelegate());
    state->addTransition(this);

    return true;
}

void AbstractTransition::onTransition(QEvent* e)
{
    qDebug() <<"triggered transition" <<transitionId;
}

/*
 * AbstractState
 */
AbstractState::AbstractState(const QString& stateId, const QString& parentStateId) :
        stateId(stateId),
        parentStateId(parentStateId),
        stateMachine(NULL)
{
    setObjectName("AbstractState");
}

AbstractState::~AbstractState()
{
}


const QString& AbstractState::getId() const
{
    return stateId;
}

AbstractState* AbstractState::getParentState() const
{
    return qobject_cast<AbstractState*>(parent());
}

QList<AbstractTransition*> AbstractState::getTransitions() const
{
    return transitions;
}

AbstractState* AbstractState::getState(const QString& stateId)
{
    if (this->stateId == stateId)
    {
        return this;
    }

    return NULL;
}

StateMachine* AbstractState::getStateMachine()
{
    return stateMachine;
}

/*
 * AbstractComplexState
 */
AbstractComplexState::AbstractComplexState(const QString &stateId, const QString& parentStateId) :
    AbstractState(stateId, parentStateId),
    delegate(new QState())
{
    //connect signals
    connect(delegate, SIGNAL(entered()), this, SLOT(eventEntered()));
    connect(delegate, SIGNAL(exited()), this, SLOT(eventExited()));
    connect(delegate, SIGNAL(finished()), this, SLOT(eventFinished()));
}

AbstractComplexState::~AbstractComplexState()
{

}

const QList<AbstractState*> AbstractComplexState::getChildStates() const
{
    QList<AbstractState*> childsCast;
    QObjectList childs = children();
    for (int i = 0; i < childs.size(); i++)
    {
        childsCast.append(qobject_cast<AbstractState*>(childs[i]));
    }

    return childsCast;
}

AbstractState* AbstractComplexState::getState(const QString& stateId)
{
    if (this->stateId == stateId)
    {
        return this;
    }

    //find state recursively
    QList<AbstractState*> states = getChildStates();
    for (int i = 0; i < states.size(); i++)
    {
        AbstractState* state = states[i]->getState(stateId);
        if (state != NULL)
        {
            return state;
        }
    }

    return NULL;
}

QState* AbstractComplexState::getDelegate() const
{
    return NULL;
}

void AbstractComplexState::eventEntered()
{
    qDebug() <<toString() <<"--> entered";
}

void AbstractComplexState::eventExited()
{
    qDebug() <<toString() <<"--> exited";
}

void AbstractComplexState::eventFinished()
{
    qDebug() <<toString() <<"--> finished";
}
