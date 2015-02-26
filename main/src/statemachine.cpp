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
#include <application.h>

#include <QScriptEngine>

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

AbstractState* AbstractTransition::getSourceState()
{
    return sourceState;
}

AbstractState *AbstractTransition::getTargetState()
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
 * Dataflow
 */
Dataflow::Dataflow(const QString& sourceStateId, const QString& targetStateId, const QString& from, const QString& to)
{
    this->sourceStateId = sourceStateId;
    this->targetStateId = targetStateId;
    this->from = from;
    this->to = to;
}

Dataflow::~Dataflow()
{

}

const QString& Dataflow::getSourceStateId() const
{
    return sourceStateId;
}

const QString& Dataflow::getTargetStateId() const
{
    return targetStateId;
}

const QString& Dataflow::getFrom() const
{
    return from;
}

const QString& Dataflow::getTo() const
{
    return to;
}

AbstractState* Dataflow::getSourceState()
{
    return sourceState;
}

AbstractState* Dataflow::getTargetState()
{
    return targetState;
}

Value& Dataflow::getFromParameter()
{
    return fromParameter;
}

Value& Dataflow::getToParameter()
{
    return toParameter;
}

QString Dataflow::toString() const
{
    return QString("[Dataflow: sourceId=%1, targetId=%1, from=%3, to%4]").arg(sourceStateId).arg(targetStateId).arg(from).arg(to);
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

    input = Value::Object();
    output = Value::Object();
}

AbstractState::~AbstractState()
{
}


const QString& AbstractState::getId() const
{
    return stateId;
}

void AbstractState::setId(const QString& stateId)
{
    this->stateId = stateId;
}

const QString& AbstractState::getParentStateId() const
{
    return parentStateId;
}

void AbstractState::setParentStateId(const QString& parentStateId)
{
    this->parentStateId = parentStateId;
}

const StateMachine* AbstractState::getStateMachine() const
{
    return stateMachine;
}

Value& AbstractState::getInput()
{
    return input;
}

void AbstractState::setInput(const Value& value)
{
    input = value;
}

Value& AbstractState::getOutput()
{
    return output;
}

void AbstractState::setOutput(const Value& value)
{
    output = value;
}

const QList<Dataflow*> &AbstractState::getDataflows() const
{
    return dataflows;
}

AbstractState* AbstractState::getParentState()
{
    return qobject_cast<AbstractState*>(parent());
}

const QList<AbstractState*>& AbstractState::getChildStates() const
{
    return childStates;
}

AbstractState* AbstractState::getChildState(const QString& stateId)
{
    for (int i = 0; i < childStates.size(); i++)
    {
        if (childStates[i]->getId() == stateId)
        {
            return childStates[i];
        }
    }

    return NULL;
}

const QList<AbstractTransition*>& AbstractState::getTransitions() const
{
    return transitions;
}

AbstractTransition* AbstractState::getTransition(const QString& transitionId)
{
    for (int i = 0; i < transitions.size(); i++)
    {
        if (transitions[i]->getId() == transitionId)
        {
            return transitions[i];
        }
    }

    return NULL;
}

AbstractState* AbstractState::findState(const QString& stateId)
{
    if (this->stateId == stateId)
    {
        return this;
    }

    //find state recursively
    for (int i = 0; i < childStates.size(); i++)
    {
        AbstractState* state = childStates[i]->findState(stateId);
        if (state != NULL)
        {
            return state;
        }
    }

    return NULL;
}

/*
 * AbstractComplexState
 */
AbstractComplexState::AbstractComplexState(const QString &stateId, const QString& parentStateId) :
    AbstractState(stateId, parentStateId),
    delegate(new QState()),
    active(false)
{
    //connect signals
    connect(delegate, SIGNAL(entered()), this, SLOT(eventEnter()));
    connect(delegate, SIGNAL(exited()), this, SLOT(eventExit()));
    connect(delegate, SIGNAL(finished()), this, SLOT(eventFinish()));
}

AbstractComplexState::~AbstractComplexState()
{

}

bool AbstractComplexState::isActive()
{
    return active;
}

QState* AbstractComplexState::getDelegate() const
{
    return delegate;
}

bool AbstractComplexState::initialize()
{
    logger->info(QString("%1 initialize").arg(toString()));

    QObject::connect(stateMachine->getDelegate(), SIGNAL(started()), this, SLOT(eventStart()));
    QObject::connect(stateMachine->getDelegate(), SIGNAL(stopped()), this, SLOT(eventStop()));
}

void AbstractComplexState::eventStart()
{

}

void AbstractComplexState::eventStop()
{
    active = false;
}

void AbstractComplexState::eventEnter()
{
    logger->info(QString("%1 --> entered").arg(toString()));

    active = true;

    Value value;
    value["action"] = "state";
    value["id"] = stateId;
    value["change"] = "enter";

    Application::getInstance()->getApi().pushState(value);
}

void AbstractComplexState::eventExit()
{
    logger->info(QString("%1 --> exited").arg(toString()));

    active = false;

    Value value;
    value["action"] = "state";
    value["id"] = stateId;
    value["change"] = "exit";

    Application::getInstance()->getApi().pushState(value);
}

void AbstractComplexState::eventFinish()
{
    logger->info(QString("%1 --> finished").arg(toString()));

    active = false;

    NamedEvent* event = new NamedEvent("finish." + stateId);
    stateMachine->postEvent(event);

    Value value;
    value["action"] = "state";
    value["id"] = stateId;
    value["change"] = "finish";

    Application::getInstance()->getApi().pushState(value);
}

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
    return QString("[NamedEvent: eventName=%1]").arg(eventName);
}

/*
 * ConditionalTransition
 */
ConditionalTransition::ConditionalTransition(const QString& transitionId, const QString& sourceStateId, const QString& targetStateId, const QString& eventName, QString condition) :
    AbstractTransition(transitionId, sourceStateId, targetStateId),
    eventName(eventName),
    condition(condition)
{

}

QString ConditionalTransition::toString() const
{
    return QString("[ConditionalTransition: id=%1, eventName=%2]").arg(transitionId).arg(eventName);
}

bool ConditionalTransition::eventTest(QEvent* e)
{
    if (e->type() != NamedEvent::type)
    {
        return false;
    }

    NamedEvent* namedEvent = static_cast<NamedEvent*>(e);

    if (namedEvent->getEventName() != eventName)
    {
        return false;
    }

    if (!condition.isEmpty())
    {
        QScriptEngine* scriptEngine = stateMachine->getScriptEngine();
        QScriptContext* context = scriptEngine->pushContext();

        context->activationObject().setProperty("input", ValueScriptBinding::create(scriptEngine, &sourceState->getInput())); //TODO performance?
        context->activationObject().setProperty("output", ValueScriptBinding::create(scriptEngine, &sourceState->getOutput())); //TODO performance?

        bool result = scriptEngine->evaluate(condition).toBool();

        scriptEngine->popContext();

        return result;
    }

    return true;
}

void ConditionalTransition::onTransition(QEvent* e)
{
    if (e->type() != NamedEvent::type)
    {
        return;
    }

    NamedEvent* namedEvent = static_cast<NamedEvent*>(e);

    logger->info(QString("%1 transition on event %2 from %3 to %4").arg(toString()).arg(namedEvent->toString()).arg(sourceStateId).arg(targetStateId));

    Value value;
    value["action"] = "transition";
    value["from"] = sourceStateId;
    value["to"] = targetStateId;
    value["event"] = namedEvent->getEventName();

    Application::getInstance()->getApi().pushState(value);
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
    return QString("[Final: id=%1]").arg(stateId);
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
    AbstractComplexState::initialize();

    //set initial state
    const AbstractState* initialState = findState(initialStateId);
    if (initialState == NULL)
    {
        logger->warning(QString("%1 initialization failed: couldn't find initial state \"%2\"").arg(toString()).arg(initialStateId));

        return false;
    }

    delegate->setInitialState(initialState->getDelegate());

    return true;
}

QString CompositeState::toString() const
{
    return QString("[CompositeState: id=%1]").arg(stateId);
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
    AbstractComplexState::initialize();

    return true;
}

QString ParallelState::toString() const
{
    return QString("[Parallel: id=%1]").arg(stateId);
}

/*
 * InvokeState
 */
InvokeState::InvokeState(const QString& stateId, const QString& binding, const QString& parentStateId) :
    AbstractComplexState(stateId, parentStateId),
    binding(binding),
    communicationPlugin(Application::getInstance()->getCommunicationPluginLoader().getCommunicationPlugin(binding)),
    invocationActive(false)
{
    if (communicationPlugin != NULL) {
        communicationPlugin->successCallback = std::bind(&InvokeState::success, this);
        communicationPlugin->errorCallback = std::bind(&InvokeState::error, this, std::placeholders::_1);
    }

    QState* stateInvoke = new QState(delegate);
    QFinalState* stateFinal = new QFinalState(delegate);
    InternalTransition* transitionFinal = new InternalTransition("done." + stateId);
    transitionFinal->setTargetState(stateFinal);
    stateInvoke->addTransition(transitionFinal);

    delegate->setInitialState(stateInvoke);

    endpoint = Value::Object();
}

InvokeState::~InvokeState()
{
    if (communicationPlugin != NULL)
    {
        delete communicationPlugin;
    }
}

void InvokeState::invoke()
{
    if (communicationPlugin == NULL)
    {
        logger->warning(QString("%1 can't invoke application: invalid communication plugin").arg(toString()));

        return;
    }

    invocationActive = true;

    communicationPlugin->endpoint = &endpoint;
    communicationPlugin->input = &input;
    communicationPlugin->output = &output;

    communicationPlugin->invoke();
}

void InvokeState::cancel()
{
    if (!invocationActive)
    {
        return;
    }

    if (communicationPlugin == NULL)
    {
        logger->warning(QString("%1 can't cancel invocation: invalid communication plugin").arg(toString()));

        return;
    }

    communicationPlugin->cancel();
}

const QString& InvokeState::getBinding() const
{
    return binding;
}

CommunicationPlugin* InvokeState::getCommunicationPlugin()
{
    return communicationPlugin;
}

Value& InvokeState::getEndpoint()
{
    return endpoint;
}

void InvokeState::setEndpoint(Value& value)
{
    endpoint = value;
}

bool InvokeState::initialize()
{
    AbstractComplexState::initialize();

    return true;
}

void InvokeState::eventStop()
{
    AbstractComplexState::eventStop();

    cancel();
}

void InvokeState::eventEnter()
{
    AbstractComplexState::eventEnter();

    invoke();
}

void InvokeState::eventExit()
{
    AbstractComplexState::eventExit();

    cancel();
}

void InvokeState::eventFinish()
{
    AbstractComplexState::eventFinish();
}

void InvokeState::success()
{
    if (!active)
    {
        return;
    }

    if (!invocationActive)
    {
        return;
    }

    invocationActive = false;

    logger->info(QString("%1 invocation finished successfully").arg(toString()));

    NamedEvent* event = new NamedEvent("invoke.success." + stateId);
    stateMachine->postEvent(event);

    InternalEvent* internalEvent = new InternalEvent("done." + stateId);
    stateMachine->postEvent(internalEvent);
}

void InvokeState::error(QString message)
{
    if (!active)
    {
        return;
    }

    if (!invocationActive)
    {
        return;
    }

    invocationActive = false;

    logger->warning(QString("%1 invocation finished with an error: %2").arg(toString()).arg(message));

    NamedEvent* event = new NamedEvent("invoke.error." + stateId);
    stateMachine->postEvent(event);

    InternalEvent* internalEvent = new InternalEvent("done." + stateId);
    stateMachine->postEvent(internalEvent);
}

QString InvokeState::toString() const
{
    return QString("[Invoke: id=%1]").arg(stateId);
}

/*
 * StateMachine
 */
StateMachine::StateMachine(const QString& stateId, const QString &initialId, const QString& parentStateId) :
    AbstractComplexState(stateId, parentStateId),
    delegate(new QStateMachine()),
    scriptEngine(new QScriptEngine()),
    initialId(initialId)
{
    delete AbstractComplexState::delegate;

    //connect signals
    connect(delegate, SIGNAL(entered()), this, SLOT(eventEnter()));
    connect(delegate, SIGNAL(exited()), this, SLOT(eventExit()));
    connect(delegate, SIGNAL(finished()), this, SLOT(eventFinish()));
}

StateMachine::~StateMachine()
{
    delete delegate;
    delete scriptEngine;
}

bool StateMachine::isRoot()
{
    if (getParentState() == NULL)
    {
        return true;
    }

    return false;
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

QScriptEngine* StateMachine::getScriptEngine()
{
    return scriptEngine;
}

QStateMachine* StateMachine::getDelegate() const
{
    return delegate;
}

bool StateMachine::initialize()
{
    AbstractComplexState::initialize();

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
    return QString("[StateMachine: id=%1]").arg(stateId);
}

void StateMachine::eventStart()
{
    logger->info(QString("%1 --> started state machine").arg(toString()));

    Value value;
    value["action"] = "statemachine";
    value["id"] = stateId;
    value["change"] = "start";

    Application::getInstance()->getApi().pushState(value);
}

void StateMachine::eventStop()
{
    logger->info(QString("%1 --> stopped state machine").arg(toString()));

    Value value;
    value["action"] = "statemachine";
    value["id"] = stateId;
    value["change"] = "stop";

    Application::getInstance()->getApi().pushState(value);
}

void StateMachine::eventFinish()
{
    if (isRoot())
    {
        logger->info(QString("%1 --> finished").arg(toString()));

        Value value;
        value["action"] = "statemachine";
        value["id"] = stateId;
        value["change"] = "finish";

        Application::getInstance()->getApi().pushState(value);
    }
    else
    {
        AbstractComplexState::eventFinish();
    }
}
