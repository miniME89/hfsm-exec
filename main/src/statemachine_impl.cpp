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
#include <logger.h>
#include <application.h>

#include <easylogging++.h>

using namespace hfsmexec;

/*
 * NamedEvent
 */
const QEvent::Type NamedEvent::type = QEvent::Type(QEvent::User + 1);

NamedEvent::NamedEvent(const QString &name) :
    AbstractEvent(type),
    eventName(name)
{

}

NamedEvent::~NamedEvent()
{

}

const QString& NamedEvent::getEventName() const
{
    return eventName;
}

void NamedEvent::setEventName(const QString& name)
{
    this->eventName = name;
}
const QString& NamedEvent::getOrigin() const
{
    return origin;
}

void NamedEvent::setOrigin(const QString& origin)
{
    this->origin = origin;
}
const QString& NamedEvent::getMessage() const
{
    return message;
}

void NamedEvent::setMessage(const QString& message)
{
    this->message = message;
}

QString NamedEvent::toString() const
{
    return "[StringEvent: " + eventName + "]";
}

/*
 * NamedTransition
 */
NamedTransition::NamedTransition(const QString& transitionId, const QString& sourceStateId, const QString& targetStateId, const QString& eventName) :
    AbstractTransition(transitionId, sourceStateId, targetStateId)
{
    this->eventName = eventName;
}

QString NamedTransition::toString() const
{
    return "[StringTransition: " + transitionId + "]";
}

bool NamedTransition::eventTest(QEvent* e)
{
    if (e->type() != NamedEvent::type)
    {
        return false;
    }

    NamedEvent* namedEvent = static_cast<NamedEvent*>(e);

    return namedEvent->getEventName() == eventName;
}

/*
 * InternalEvent
 */
const QEvent::Type InternalEvent::type = QEvent::Type(QEvent::User + 2);

InternalEvent::InternalEvent(const QString& eventName) :
    QEvent(type),
    eventName(eventName)
{

}

const QString& InternalEvent::getEventName() const
{
    return eventName;
}

/*
 * InternalTransition
 */
InternalTransition::InternalTransition(const QString& eventName) :
    eventName(eventName)
{

}

bool InternalTransition::eventTest(QEvent* e)
{
    if (e->type() != InternalEvent::type)
    {
        return false;
    }

    InternalEvent* iternalEvent = static_cast<InternalEvent*>(e);

    return iternalEvent->getEventName() == eventName;
}

void InternalTransition::onTransition(QEvent* e)
{

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
    CLOG(INFO, LOG_STATEMACHINE) <<toString() <<" initialize";

    return true;
}

QString FinalState::toString() const
{
    return "[Final: " + stateId + "]";
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
    CLOG(INFO, LOG_STATEMACHINE) <<toString() <<" initialize";

    if (!initialStateId.isEmpty()) //TODO remove
    {
        //set initial state
        const AbstractState* initialState = getState(initialStateId);
        if (initialState == NULL)
        {
            CLOG(WARNING, LOG_STATEMACHINE) <<toString() <<" initialization failed: couldn't find initial state \"" <<initialStateId <<"\"";

            return false;
        }

        delegate->setInitialState(initialState->getDelegate());
    }

    return true;
}

QString CompositeState::toString() const
{
    return "[CompositeState: " + stateId + "]";
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
    CLOG(INFO, LOG_STATEMACHINE) <<toString() <<" initialize";

    return true;
}

QString ParallelState::toString() const
{
    return "[Parallel: " + stateId + "]";
}

/*
 * InvokeState
 */
InvokeState::InvokeState(const QString& stateId, const QString& type, const QString& parentStateId) :
    AbstractComplexState(stateId, parentStateId),
    type(type),
    communicationPlugin(Application::getInstance()->getCommunicationPluginLoader()->getPlugin(type))
{
    QState* stateInvoke = new QState(delegate);
    QFinalState* stateFinal = new QFinalState(delegate);
    InternalTransition* transitionFinal = new InternalTransition("done." + stateId);
    transitionFinal->setTargetState(stateFinal);
    stateInvoke->addTransition(transitionFinal);

    delegate->setInitialState(stateInvoke);
}

InvokeState::~InvokeState()
{

}

const ValueContainer& InvokeState::getEndpoint() const
{
    return endpoint;
}

void InvokeState::setEndpoint(const ValueContainer& value)
{
    endpoint = value;
}

const ValueContainer& InvokeState::getInputParameters() const
{
    return inputParameters;
}

void InvokeState::setInputParameters(const ValueContainer& value)
{
    inputParameters = value;
}

const ValueContainer& InvokeState::getOutputParameters() const
{
    return outputParameters;
}

void InvokeState::setOutputParameters(const ValueContainer& value)
{
    outputParameters = value;
}

CommunicationPlugin* InvokeState::getCommunicationPlugin() const
{
    return communicationPlugin;
}

void InvokeState::setCommunicationPlugin(CommunicationPlugin* value)
{
    communicationPlugin = value;
}

void InvokeState::done()
{
    InternalEvent* event = new InternalEvent("done." + stateId);
    stateMachine->postEvent(event);
}

bool InvokeState::initialize()
{
    CLOG(INFO, LOG_STATEMACHINE) <<toString() <<" initialize";

    return true;
}

void InvokeState::eventEntered()
{
    AbstractComplexState::eventEntered();

    if (communicationPlugin == NULL)
    {
        CLOG(WARNING, LOG_STATEMACHINE) <<toString() <<" invalid communication plugin. Skip invocation.";

        return;
    }

    QString json;
    inputParameters.toJson(json);
    CLOG(INFO, LOG_STATEMACHINE) <<json;

    communicationPlugin->invoke(endpoint, inputParameters, outputParameters);

    QString json2;
    outputParameters.toJson(json2);
    CLOG(INFO, LOG_STATEMACHINE) <<json2;

    done(); //TODO temporary
}

void InvokeState::eventExited()
{
    AbstractComplexState::eventExited();
}

void InvokeState::eventFinished()
{
    AbstractComplexState::eventFinished();
}

QString InvokeState::toString() const
{
    return "[Invoke: " + stateId + "]";
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
    if (delegate->isRunning())
    {
        CLOG(WARNING, LOG_STATEMACHINE) <<toString() <<" can't start state machine: state machine is already running";

        return;
    }

    CLOG(INFO, LOG_STATEMACHINE) <<toString() <<" start state machine";

    delegate->start();
}

void StateMachine::stop() const
{
    if (!delegate->isRunning())
    {
        CLOG(WARNING, LOG_STATEMACHINE) <<toString() <<" can't stop state machine: state machine is not running";

        return;
    }

    CLOG(INFO, LOG_STATEMACHINE) <<toString() <<" stop state machine";

    delegate->stop();
}

int StateMachine::postDelayedEvent(QEvent* event, int delay)
{
    if (!delegate->isRunning())
    {
        CLOG(WARNING, LOG_STATEMACHINE) <<toString() <<" can't post delayed event to state machine: state machine is not running";

        return -1;
    }

    CLOG(INFO, LOG_STATEMACHINE) <<toString() <<" post delayed event";

    return delegate->postDelayedEvent(event, delay);
}

void StateMachine::postEvent(QEvent* event, QStateMachine::EventPriority priority)
{
    if (!delegate->isRunning())
    {
        CLOG(WARNING, LOG_STATEMACHINE) <<toString() <<" can't post event to state machine: state machine is not running";

        return;
    }

    CLOG(INFO, LOG_STATEMACHINE) <<toString() <<" post event";

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
    CLOG(INFO, LOG_STATEMACHINE) <<toString() <<" initialize";

    //set initial state
    const AbstractState* initialState = getState(initialId);
    if (initialState == NULL)
    {
        CLOG(WARNING, LOG_STATEMACHINE) <<toString() <<" initialization failed: couldn't find initial state \"" <<initialId <<"\"";

        return false;
    }

    delegate->setInitialState(initialState->getDelegate());

    return true;
}

QString StateMachine::toString() const
{
    return "[StateMachine: " + stateId + "]";
}

void StateMachine::eventStarted()
{
    CLOG(INFO, LOG_STATEMACHINE) <<toString() <<" --> started state machine";
}

void StateMachine::eventStopped()
{
    CLOG(INFO, LOG_STATEMACHINE) <<toString() <<" --> stopped state machine";
}
