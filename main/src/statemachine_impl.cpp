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
    return "[NamedEvent: " + eventName + "]";
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

void NamedTransition::onTransition(QEvent* e)
{
    if (e->type() != NamedEvent::type)
    {
        return;
    }

    NamedEvent* namedEvent = static_cast<NamedEvent*>(e);

    logger->info(QString("%1 triggered transition on event %2").arg(toString()).arg(namedEvent->toString()));
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
    logger->info(QString("%1 initialize").arg(toString()));

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
    logger->info(QString("%1 initialize").arg(toString()));

    if (!initialStateId.isEmpty()) //TODO remove
    {
        //set initial state
        const AbstractState* initialState = findState(initialStateId);
        if (initialState == NULL)
        {
            logger->warning(QString("%1 initialization failed: couldn't find initial state \"%2\"").arg(toString()).arg(initialStateId));

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
    logger->info(QString("%1 initialize").arg(toString()));

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

    endpoint = Object();
}

InvokeState::~InvokeState()
{

}

Value& InvokeState::getEndpoint()
{
    return endpoint;
}

void InvokeState::setEndpoint(Value& value)
{
    endpoint = value;
}

CommunicationPlugin* InvokeState::getCommunicationPlugin()
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
    logger->info(QString("%1 initialize").arg(toString()));

    return true;
}

void InvokeState::eventEntered()
{
    AbstractComplexState::eventEntered();

    if (communicationPlugin == NULL)
    {
        logger->warning(QString("%1 invalid communication plugin. Skip invocation.").arg(toString()));

        return;
    }

    Value all;
    all["input"] = &inputParameters;
    all["output"] = &outputParameters;
    all["endpoint"] = &endpoint;

    QString json;
    all.toJson(json);
    logger->info(json);

    communicationPlugin->invoke(endpoint, inputParameters, outputParameters);

    QString json2;
    all.toJson(json2);
    logger->info(json2);

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
StateMachine::StateMachine(const QString& stateId, const QString &initialId, const QString& parentStateId) :
    AbstractComplexState(stateId, parentStateId),
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
        logger->warning(QString("%1 can't start state machine: state machine is already running").arg(toString()));

        return;
    }

    logger->info(QString("%1 start state machine").arg(toString()));

    delegate->start();
}

void StateMachine::stop() const
{
    if (!delegate->isRunning())
    {
        logger->warning(QString("%1 can't stop state machine: state machine is not running").arg(toString()));

        return;
    }

    logger->info(QString("%1 stop state machine").arg(toString()));

    delegate->stop();
}

int StateMachine::postDelayedEvent(QEvent* event, int delay)
{
    if (!delegate->isRunning())
    {
        logger->warning(QString("%1 can't post delayed event to state machine: state machine is not running").arg(toString()));

        return -1;
    }

    logger->info(QString("%1 post delayed event").arg(toString()));

    return delegate->postDelayedEvent(event, delay);
}

void StateMachine::postEvent(QEvent* event, QStateMachine::EventPriority priority)
{
    if (!delegate->isRunning())
    {
        logger->warning(QString("%1 can't post event to state machine: state machine is not running").arg(toString()));

        return;
    }

    logger->info(QString("%1 post event").arg(toString()));

    delegate->postEvent(event, priority);
}

QStateMachine* StateMachine::getDelegate() const
{
    return delegate;
}

bool StateMachine::initialize()
{
    logger->info(QString("%1 initialize").arg(toString()));

    //set initial state
    const AbstractState* initialState = findState(initialId);
    if (initialState == NULL)
    {
        logger->warning(QString("%1 initialization failed: couldn't find initial state \"%2\"").arg(toString()).arg(initialId));

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
    logger->info(QString("%1 --> started state machine").arg(toString()));
}

void StateMachine::eventStopped()
{
    logger->info(QString("%1 --> stopped state machine").arg(toString()));
}
