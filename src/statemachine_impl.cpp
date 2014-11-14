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

#include <statemachine_impl.h>

#include <QDebug>

using namespace hfsmexec;

/*
 * StringEvent
 */
const QEvent::Type StringEvent::typeId = QEvent::Type(QEvent::User + 1);

StringEvent::StringEvent(const QString &value) :
    AbstractEvent(typeId)
{
    this->value = value;
}

StringEvent::~StringEvent()
{

}

QString StringEvent::toString() const
{
    return "StringEvent [value: " + value + "]";
}

/*
 * StringTransition
 */
StringTransition::StringTransition(const QString transitionId, const QString sourceStateId, const QString targetStateId, const QString &value) :
        AbstractTransition(transitionId, sourceStateId, targetStateId)
{
    this->value = value;
}

QString StringTransition::toString() const
{
    return "StringTransition [transitionId: " + transitionId + "]";
}

bool StringTransition::eventTest(QEvent* e)
{
    if (e->type() != StringEvent::typeId)
    {
        return false;
    }

    StringEvent* event = static_cast<StringEvent*>(e);

    return event->value == value;
}

/*
 * FinalState
 */
FinalState::FinalState(const QString& stateId, const QString& parentStateId) :
    AbstractState(stateId, parentStateId),
    delegate(new QFinalState())
{

}

FinalState::~FinalState()
{
}

QFinalState* FinalState::getDelegate() const
{
    return delegate;
}

bool FinalState::initialize()
{
    return true;
}

QString FinalState::toString() const
{
    return "Final [stateId: " + stateId + "]";
}

/*
 * CompositeState
 */
CompositeState::CompositeState(const QString& stateId, const QString &initialId, const QString& parentStateId) :
        AbstractComplexState(stateId, parentStateId),
        initialStateId(initialId)
{
    delegate->setChildMode(QState::ExclusiveStates);
}

CompositeState::~CompositeState()
{
}

bool CompositeState::initialize()
{
    if (!initialStateId.isEmpty())
    {
        //set initial state
        const AbstractState* initialState = getState(initialStateId);
        if (initialState == NULL)
        {
            qWarning() <<toString() <<"initialization failed: couldn't find initial state" <<initialStateId;

            return false;
        }

        delegate->setInitialState(initialState->getDelegate());
    }

    return true;
}

QString CompositeState::toString() const
{
    return "CompositeState [stateId: " + stateId + "]";
}

/*
 * ParallelState
 */
ParallelState::ParallelState(const QString& stateId, const QString& parentStateId) :
        AbstractComplexState(stateId, parentStateId)
{
    delegate->setChildMode(QState::ParallelStates);
}

ParallelState::~ParallelState()
{
}

bool ParallelState::initialize()
{
    return true;
}

QString ParallelState::toString() const
{
    return "Parallel [stateId: " + stateId + "]";
}

/*
 * InvokeState
 */
InvokeState::InvokeState(const QString &stateId, const QString& type, const QString &parentStateId) :
    AbstractComplexState(stateId, parentStateId),
    type(type)
{

}

InvokeState::~InvokeState()
{

}

bool InvokeState::initialize()
{
    return true;
}

QString InvokeState::toString() const
{
    return "Invoke [stateId: " + stateId + "]";
}

/*
 * StateMachine
 */
StateMachine::StateMachine(const QString &initialId) :
    AbstractComplexState(""),
    delegate(new QStateMachine()),
    initialId(initialId)
{
    delete AbstractComplexState::delegate;

    //connect signals
    connect(delegate, SIGNAL(entered()), this, SLOT(eventEntered()));
    connect(delegate, SIGNAL(exited()), this, SLOT(eventExited()));
    connect(delegate, SIGNAL(finished()), this, SLOT(eventFinished()));
    connect(delegate, SIGNAL(started()), this, SLOT(eventStarted()));
    connect(delegate, SIGNAL(stopped()), this, SLOT(eventStopped()));
}

StateMachine::~StateMachine()
{
}

void StateMachine::start() const
{
    delegate->start();
}

void StateMachine::stop() const
{
    delegate->stop();
}

int StateMachine::postDelayedEvent(QEvent* event, int delay)
{
    return delegate->postDelayedEvent(event, delay);
}

void StateMachine::postEvent(QEvent* event, QStateMachine::EventPriority priority)
{
    delegate->postEvent(event, priority);
}

StateMachine* StateMachine::getStateMachine()
{
    return this;
}

QStateMachine* StateMachine::getDelegate() const
{
    return delegate;
}

bool StateMachine::initialize()
{
    //set initial state
    const AbstractState* initialState = getState(initialId);
    if (initialState == NULL)
    {
        qWarning() <<toString() <<"initialization failed: couldn't find initial state" <<initialId;

        return false;
    }

    delegate->setInitialState(initialState->getDelegate());

    return true;
}

QString StateMachine::toString() const
{
    return "StateMachine [stateId: " + stateId + "]";
}

void StateMachine::eventStarted()
{
    qDebug() <<toString() <<"--> started state machine";
}

void StateMachine::eventStopped()
{
    qDebug() <<toString() <<"--> stopped state machine";
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
        qWarning() <<"can't add state machine: another state machine was already provided";

        return;
    }

    if (stateMachine->stateMachine != NULL)
    {
        qWarning() <<"can't add state machine: state machine is already part of another state machine";

        return;
    }

    this->stateMachine = stateMachine;
}

void StateMachineBuilder::addState(AbstractState* state)
{
    if (state->stateMachine != NULL)
    {
        qWarning() <<"can't add state: state is already part of another state machine";

        return;
    }

    states.append(state);
}

void StateMachineBuilder::addTransition(AbstractTransition* transition)
{
    if (transition->stateMachine != NULL)
    {
        qWarning() <<"can't add transition: transition is already part of another state machine";

        return;
    }

    transitions.append(transition);
}

StateMachine* StateMachineBuilder::build()
{
    if (stateMachine == NULL)
    {
        qWarning() <<"can't build state machine: no instance of StateMachine was provided";

        return NULL;
    }

    qDebug() <<"create state machine";

    //link states
    qDebug() <<"link states";
    for (int i = 0; i < states.size(); i++)
    {
        AbstractState* state = states[i];

        //find parent state
        AbstractState* parentState = getState(state->parentStateId);
        if (parentState == NULL)
        {
            qWarning() <<"initialization failed: couldn't find parent state" <<state->parentStateId;

            return NULL;
        }

        //link state
        qDebug() <<"link child state" <<state->getId() <<"with parent state" <<parentState->getId();
        state->stateMachine = stateMachine;
        state->setParent(parentState);
        state->getDelegate()->setParent(parentState->getDelegate());
    }

    //initialize state machine
    qDebug() <<"initialize state machine";
    stateMachine->stateMachine = stateMachine;
    if (!stateMachine->initialize())
    {
        qWarning() <<"initialization failed: initialization of state machine failed";

        return NULL;
    }

    //initialize states
    qDebug() <<"initialize" <<states.size() <<"states";
    for (int i = 0; i < states.size(); i++)
    {
        AbstractState* state = states[i];

        //initialize state
        qDebug() <<"[" <<i <<"]" <<"initialize state" <<state->getId();
        if (!state->initialize())
        {
            qWarning() <<"initialization failed: initialization of states failed";

            return NULL;
        }
    }

    //initialize transitions
    qDebug() <<"initialize" <<transitions.size() <<"transitions";
    for (int i = 0; i < transitions.size(); i++)
    {
        AbstractTransition* transition = transitions[i];

        //find source state
        AbstractState* sourceState = getState(transition->sourceStateId);
        if (sourceState == NULL)
        {
            qWarning() <<"initialization failed: couldn't find transition source state" <<transition->sourceStateId;

            return NULL;
        }

        //find target state
        AbstractState* targetState = getState(transition->targetStateId);
        if (targetState == NULL)
        {
            qWarning() <<"initialization failed: couldn't find transition source state" <<transition->targetStateId;

            return NULL;
        }

        sourceState->transitions.append(transition);
        transition->stateMachine = stateMachine;
        transition->sourceState = sourceState;
        transition->targetState = targetState;

        qDebug() <<"[" <<i <<"]" <<"initialize transition" <<transition->getId();
        if (!transition->initialize())
        {
            qWarning() <<"initialization failed: couldn't initialize all transitions";

            return NULL;
        }
    }

    qDebug() <<"created state machine successfully";

    return stateMachine;
}

StateMachineBuilder &StateMachineBuilder::operator<<(StateMachine* stateMachine)
{
    addState(stateMachine);

    return *this;
}

StateMachineBuilder& StateMachineBuilder::operator<<(AbstractState* state)
{
    addState(state);

    return *this;
}

StateMachineBuilder &StateMachineBuilder::operator<<(AbstractTransition *transition)
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

/*
 * StateMachineTest
 */
#include <QTimer>
StateMachineTest::StateMachineTest()
{
    StateMachineBuilder builder;

    builder <<new StateMachine("p1");

    builder <<new ParallelState("p1");
    builder <<new FinalState("f1");

    builder <<new CompositeState("s1", "s1_1", "p1");
    builder <<new CompositeState("s1_1", "", "s1");
    builder <<new FinalState("f1_1", "s1");

    builder <<new CompositeState("s2", "s2_1", "p1");
    builder <<new CompositeState("s2_1", "", "s2");
    builder <<new FinalState("f2_1", "s2");

    builder <<new StringTransition("t1", "p1", "f1", "f");
    builder <<new StringTransition("t2", "s1_1", "f1_1", "f1");
    builder <<new StringTransition("t3", "s2_1", "f2_1", "f2");

    sm = builder.build();
    if (sm != NULL)
    {
        sm->start();
        QTimer::singleShot(100, this, SLOT(triggerEvents()));
    }
}

void StateMachineTest::triggerEvents()
{
    sm->postDelayedEvent(new StringEvent("f1"), 2000);
    sm->postDelayedEvent(new StringEvent("f2"), 4000);
    sm->postDelayedEvent(new StringEvent("f"), 4500);
}
