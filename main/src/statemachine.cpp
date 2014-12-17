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
#include <logger.h>

#include <easylogging++.h>

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
    CLOG(INFO, LOG_STATEMACHINE) <<toString() <<" initialize";

    if (sourceState == NULL)
    {
        CLOG(WARNING, LOG_STATEMACHINE) <<toString() <<" transition initialization failed: couldn't find source state \"" <<sourceStateId <<"\"";

        return false;
    }

    if (targetState == NULL)
    {
        CLOG(WARNING, LOG_STATEMACHINE) <<toString() <<" transition initialization failed: couldn't find target state \"" <<targetStateId <<"\"";

        return false;
    }

    //cast abstract state
    QState* state = dynamic_cast<QState*>(sourceState->getDelegate());
    if (state == NULL)
    {
        CLOG(WARNING, LOG_STATEMACHINE) <<toString() <<" transition initialization failed: source delegate is not of type QState";

        return false;
    }

    CLOG(INFO, LOG_STATEMACHINE) <<toString() <<" add transition from \"" <<sourceState->getId() <<"\" to \"" <<targetState->getId() + "\"";

    setTargetState(targetState->getDelegate());
    state->addTransition(this);

    return true;
}

void AbstractTransition::onTransition(QEvent* e)
{
    CLOG(INFO, LOG_STATEMACHINE) <<toString() <<" triggered transition";
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
    CLOG(INFO, LOG_STATEMACHINE) <<toString() <<" --> entered";
}

void AbstractComplexState::eventExited()
{
    CLOG(INFO, LOG_STATEMACHINE) <<toString() <<" --> exited";
}

void AbstractComplexState::eventFinished()
{
    CLOG(INFO, LOG_STATEMACHINE) <<toString() <<" --> finished";

    NamedEvent* event = new NamedEvent("done." + stateId);
    stateMachine->postEvent(event);
}

/*
 * StateMachineBuilder
 */
StateMachineBuilder::StateMachineBuilder() :
    stateMachine(NULL)
{

}

StateMachineBuilder::~StateMachineBuilder()
{

}

void StateMachineBuilder::addState(StateMachine* stateMachine)
{
    if (this->stateMachine != NULL)
    {
        CLOG(WARNING, LOG_BUILDER) <<"can't add state machine: another state machine was already provided";

        return;
    }

    if (stateMachine->stateMachine != NULL)
    {
        CLOG(WARNING, LOG_BUILDER) <<"can't add state machine: state machine is already part of another state machine";

        return;
    }

    this->stateMachine = stateMachine;
}

void StateMachineBuilder::addState(AbstractState* state)
{
    if (state->stateMachine != NULL)
    {
        CLOG(WARNING, LOG_BUILDER) <<"can't add state: state is already part of another state machine";

        return;
    }

    states.append(state);
}

void StateMachineBuilder::addTransition(AbstractTransition* transition)
{
    if (transition->stateMachine != NULL)
    {
        CLOG(WARNING, LOG_BUILDER) <<"can't add transition: transition is already part of another state machine";

        return;
    }

    transitions.append(transition);
}

StateMachine* StateMachineBuilder::build()
{
    if (stateMachine == NULL)
    {
        CLOG(WARNING, LOG_BUILDER) <<"can't build state machine: no instance of StateMachine was provided";

        return NULL;
    }

    CLOG(INFO, LOG_BUILDER) <<"create state machine";

    //link states
    CLOG(INFO, LOG_BUILDER) <<"link states";
    for (int i = 0; i < states.size(); i++)
    {
        AbstractState* state = states[i];

        //find parent state
        AbstractState* parentState = getState(state->parentStateId);
        if (parentState == NULL)
        {
            CLOG(WARNING, LOG_BUILDER) <<"initialization failed: couldn't find parent state \"" <<state->parentStateId <<"\"";

            return NULL;
        }

        //link state
        CLOG(INFO, LOG_BUILDER) <<"link child state \"" <<state->getId() <<"\" with parent state \"" <<parentState->getId() <<"\"";
        state->stateMachine = stateMachine;
        state->setParent(parentState);
        state->getDelegate()->setParent(parentState->getDelegate());
    }

    //initialize state machine
    CLOG(INFO, LOG_BUILDER) <<"initialize state machine";
    stateMachine->stateMachine = stateMachine;
    if (!stateMachine->initialize())
    {
        CLOG(WARNING, LOG_BUILDER) <<"initialization failed: initialization of state machine failed";

        return NULL;
    }

    //initialize states
    CLOG(INFO, LOG_BUILDER) <<"initialize " <<states.size() <<" states";
    for (int i = 0; i < states.size(); i++)
    {
        AbstractState* state = states[i];

        //initialize state
        CLOG(INFO, LOG_BUILDER) <<"[" <<i <<"] initialize state \"" <<state->getId() <<"\"";
        if (!state->initialize())
        {
            CLOG(WARNING, LOG_BUILDER) <<"initialization failed: initialization of states failed";

            return NULL;
        }
    }

    //initialize transitions
    CLOG(INFO, LOG_BUILDER) <<"initialize " <<transitions.size() <<" transitions";
    for (int i = 0; i < transitions.size(); i++)
    {
        AbstractTransition* transition = transitions[i];

        //find source state
        AbstractState* sourceState = getState(transition->sourceStateId);
        if (sourceState == NULL)
        {
            CLOG(WARNING, LOG_BUILDER) <<"initialization failed: couldn't find transition source state \"" <<transition->sourceStateId <<"\"";

            return NULL;
        }

        //find target state
        AbstractState* targetState = getState(transition->targetStateId);
        if (targetState == NULL)
        {
            CLOG(WARNING, LOG_BUILDER) <<"initialization failed: couldn't find transition source state \"" <<transition->targetStateId <<"\"";

            return NULL;
        }

        sourceState->transitions.append(transition);
        transition->stateMachine = stateMachine;
        transition->sourceState = sourceState;
        transition->targetState = targetState;

        CLOG(INFO, LOG_BUILDER) <<"[" <<i <<"] initialize transition \"" <<transition->getId() <<"\"";
        if (!transition->initialize())
        {
            CLOG(WARNING, LOG_BUILDER) <<"initialization failed: couldn't initialize all transitions";

            return NULL;
        }
    }

    CLOG(INFO, LOG_BUILDER) <<"created state machine successfully";

    return stateMachine;
}

StateMachineBuilder& StateMachineBuilder::operator<<(StateMachine* stateMachine)
{
    addState(stateMachine);

    return *this;
}

StateMachineBuilder& StateMachineBuilder::operator<<(AbstractState* state)
{
    addState(state);

    return *this;
}

StateMachineBuilder& StateMachineBuilder::operator<<(AbstractTransition* transition)
{
    addTransition(transition);

    return *this;
}

AbstractState* StateMachineBuilder::getState(const QString& stateId)
{
    if (stateId.isEmpty())
    {
        return stateMachine;
    }

    for (int i = 0; i < states.size(); i++)
    {
        if (states[i]->getId() == stateId)
        {
            return states[i];
        }
    }

    return NULL;
}
