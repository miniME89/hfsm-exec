#include <statemachine.h>
#include <QDebug>
#include <QTimer>

using namespace hfsmexec;

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

State* AbstractState::getParentState() const
{
    return qobject_cast<State*>(parent());
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
 * State
 */
State::State(const QString &stateId, const QString& parentStateId) :
    AbstractState(stateId, parentStateId),
    delegate(new QState())
{
    //connect signals
    connect(delegate, SIGNAL(entered()), this, SLOT(eventEntered()));
    connect(delegate, SIGNAL(exited()), this, SLOT(eventExited()));
    connect(delegate, SIGNAL(finished()), this, SLOT(eventFinished()));
}

State::~State()
{

}

const QList<AbstractState*> State::getChildStates() const
{
    QList<AbstractState*> childsCast;
    QObjectList childs = children();
    for (int i = 0; i < childs.size(); i++)
    {
        childsCast.append(qobject_cast<AbstractState*>(childs[i]));
    }

    return childsCast;
}

AbstractState* State::getState(const QString& stateId)
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

    qWarning() <<toString() <<"couldn't find state with state id" <<stateId;

    return NULL;
}

QState* State::getDelegate() const
{
    return delegate;
}

void State::eventEntered()
{
    qDebug() <<toString() <<"--> entered";
}

void State::eventExited()
{
    qDebug() <<toString() <<"--> exited";
}

void State::eventFinished()
{
    qDebug() <<toString() <<"--> finished";
}

/*
 * CompositeState
 */
CompositeState::CompositeState(const QString& stateId, const QString &initialId, const QString& parentStateId) :
        State(stateId, parentStateId),
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
            qWarning() <<toString() <<"initialization failed: couldn't find initial state " <<initialStateId;

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
        State(stateId, parentStateId)
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
 * StateMachine
 */
StateMachine::StateMachine(const QString &initialId) :
    State("SM"),
    delegate(new QStateMachine()),
    initialId(initialId)
{
    stateMachine = this;
    delete State::delegate;

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
    qDebug() <<"initialize state machine";

    //set initial state
    const AbstractState* initialState = getState(initialId);
    if (initialState == NULL)
    {
        qWarning() <<toString() <<"initialization failed: couldn't find initial state \"" <<initialId <<"\"";

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
 * StateMachineFactory
 */
StateMachineBuilder::StateMachineBuilder()
{

}

StateMachineBuilder::~StateMachineBuilder()
{

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

StateMachine* StateMachineBuilder::create(const QString& initialStateId)
{
    qDebug() <<"create state machine";

    StateMachine* stateMachine = new StateMachine(initialStateId);

    //link states
    qDebug() <<"link states";
    for (int i = 0; i < states.size(); i++)
    {
        AbstractState* state = states[i];

        //find parent state
        AbstractState* parentState = (state->parentStateId.isEmpty()) ? stateMachine : getState(state->parentStateId);
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

    stateMachine->initialize();

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
StateMachineTest::StateMachineTest()
{
    StateMachineBuilder factory;

    factory <<new ParallelState("p1");
    factory <<new FinalState("f1");

    factory <<new CompositeState("s1", "s1_1", "p1");
    factory <<new CompositeState("s1_1", "", "s1");
    factory <<new FinalState("f1_1", "s1");

    factory <<new CompositeState("s2", "s2_1", "p1");
    factory <<new CompositeState("s2_1", "", "s2");
    factory <<new FinalState("f2_1", "s2");

    factory <<new StringTransition("t1", "p1", "f1", "f");
    factory <<new StringTransition("t2", "s1_1", "f1_1", "f1");
    factory <<new StringTransition("t3", "s2_1", "f2_1", "f2");

    sm = factory.create("p1");
    sm->start();

    QTimer::singleShot(100, this, SLOT(triggerEvents()));
}

void StateMachineTest::triggerEvents()
{
    qDebug() <<"event";
    sm->postDelayedEvent(new StringEvent("f1"), 2000);
    sm->postDelayedEvent(new StringEvent("f2"), 4000);
    sm->postDelayedEvent(new StringEvent("f"), 4500);
}
