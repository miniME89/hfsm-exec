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
#include <statemachine_impl.h>

using namespace hfsmexec;

/*
 * AbstractEvent
 */
const Logger* AbstractEvent::logger = Logger::getLogger(LOGGER_STATEMACHINE);

AbstractEvent::AbstractEvent(Type type) :
    QEvent(type)
{

}

/*
 * AbstractTransition
 */
const Logger* AbstractTransition::logger = Logger::getLogger(LOGGER_STATEMACHINE);

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
    logger->info(QString("%1 initialize").arg(toString()));

    if (sourceState == NULL)
    {
        logger->warning(QString("%1 transition initialization failed: couldn't find source state \"%2\"").arg(toString()).arg(sourceStateId));

        return false;
    }

    if (targetState == NULL)
    {
        logger->warning(QString("%1 transition initialization failed: couldn't find target state \"%2\"").arg(toString()).arg(targetStateId));

        return false;
    }

    //cast abstract state
    QState* state = dynamic_cast<QState*>(sourceState->getDelegate());
    if (state == NULL)
    {
        logger->warning(QString("%1 transition initialization failed: source delegate is not of type QState").arg(toString()));

        return false;
    }

    logger->info(QString("%1 add transition from \"%2\" to \"%3\"").arg(toString()).arg(sourceState->getId()).arg(targetState->getId()));

    setTargetState(targetState->getDelegate());
    state->addTransition(this);

    return true;
}

/*
 * AbstractState
 */
const Logger* AbstractState::logger = Logger::getLogger(LOGGER_STATEMACHINE);

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

StateMachine* AbstractState::getStateMachine()
{
    return stateMachine;
}

AbstractState* AbstractState::getState(const QString& stateId)
{
    if (this->stateId == stateId)
    {
        return this;
    }

    return NULL;
}

Value& AbstractState::getInputParameters()
{
    return inputParameters;
}

Value& AbstractState::getOutputParameters()
{
    return outputParameters;
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
    return delegate;
}

void AbstractComplexState::eventEntered()
{
    logger->info(QString("%1 --> entered").arg(toString()));
}

void AbstractComplexState::eventExited()
{
    logger->info(QString("%1 --> exited").arg(toString()));
}

void AbstractComplexState::eventFinished()
{
    logger->info(QString("%1 --> finished").arg(toString()));

    NamedEvent* event = new NamedEvent("done." + stateId);
    stateMachine->postEvent(event);
}
