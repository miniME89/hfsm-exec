#include <statemachine.h>
#include <QDebug>
#include <QTimer>

using namespace hfsmexec;

/*
 * AbstractState
 */
AbstractState::AbstractState(const QString& id, State* parent) :
        id(id),
        initialized(false)
{
    setParent(parent);
    setObjectName("AbstractState");
}

AbstractState::~AbstractState()
{
}

const QString& AbstractState::getId() const
{
    return id;
}

const bool& AbstractState::isInitialized() const
{
    return initialized;
}

State* AbstractState::getParentState() const
{
    return qobject_cast<State*>(parent());
}

const StateMachine* AbstractState::getStateMachine() const
{
    AbstractState* parent = getParentState();
    if (parent != NULL)
    {
        return parent->getStateMachine();
    }

    return NULL;
}

const AbstractState* AbstractState::findState(const QString& id) const
{
    if (this->id == id)
    {
        return this;
    }

    return NULL;
}

/*
 * FinalState
 */
FinalState::FinalState(const QString& id, State* parent) :
        AbstractState(id, parent)
{
    delegate = new QFinalState((parent != NULL) ? parent->getDelegate() : NULL);
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
    qDebug() <<toString() <<"initialize";

    return true;
}

QString FinalState::toString() const
{
    return "Final [id: " + id + "]";
}

/*
 * State
 */
State::State(const QString &id, State *parent) :
    AbstractState(id, parent)
{
    delegate = new QState((parent != NULL) ? parent->getDelegate() : NULL);
}

State::~State()
{

}

void State::addTransition(AbstractTransition* transition)
{
    transitions.push_back(transition);
}

void State::removeTransition(AbstractTransition* transition)
{
    //TODO
}

const AbstractState* State::findState(const QString& id) const
{
    if (this->id == id)
    {
        return this;
    }

    //find state recursively
    QList<AbstractState*> children = getChildStates();
    for (int i = 0; i < children.size(); i++)
    {
        const AbstractState* element = children[i]->findState(id);
        if (element != NULL)
        {
            return element;
        }
    }

    return NULL;
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

QState *State::getDelegate() const
{
    return delegate;
}

bool State::initialize()
{
    if (initialized)
    {
        qWarning() <<toString() <<"already initialized";

        return true;
    }

    qDebug() <<toString() <<"initialize";

    //initialize children
    QList<AbstractState*> children = getChildStates();
    for (int i = 0; i < children.size(); i++)
    {
        if (!children[i]->initialize())
        {
            qWarning() <<toString() <<"initialization failed: couldn't initialize all child components";

            return false;
        }
    }

    //get the state machine
    const StateMachine* stateMachine = getStateMachine();
    if (stateMachine == NULL)
    {
        qWarning() <<toString() <<"initialization failed: couldn't find state machine";

        return false;
    }

    //initialize transitions
    for (unsigned int i = 0; i < transitions.size(); i++)
    {
        if (!transitions[i]->initialize(stateMachine))
        {
            qWarning() <<toString() <<"initialization failed: couldn't initialize all transitions";

            return false;
        }
    }

    //connect signals
    connect(getDelegate(), SIGNAL(entered()), this, SLOT(eventEntered()));
    connect(getDelegate(), SIGNAL(exited()), this, SLOT(eventExited()));
    connect(getDelegate(), SIGNAL(finished()), this, SLOT(eventFinished()));

    return true;
}

QString State::toString() const
{
    return "State [id: " + id + "]";
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
CompositeState::CompositeState(const QString& id, State* parent) :
        State(id, parent)
{
    delegate->setChildMode(QState::ExclusiveStates);
}

CompositeState::~CompositeState()
{
}

const QString& CompositeState::getInitial() const
{
    return initial;
}

void CompositeState::setInitial(const QString& initial)
{
    this->initial = initial;
}

bool CompositeState::initialize()
{
    if (initialized)
    {
        qWarning() <<toString() <<"already initialized";

        return true;
    }

    //initialize state
    if (State::initialize())
    {
        qWarning() <<toString() <<"initialization failed: base class initialization failed";

        return false;
    }

    //set initial state
    const AbstractState* initialState = findState(initial);
    if (initialState == NULL)
    {
        qWarning() <<toString() <<"initialization failed: couldn't find initial state \"" <<initial <<"\"";

        return false;
    }

    delegate->setInitialState(initialState->getDelegate());

    initialized = true;

    return true;
}

QString CompositeState::toString() const
{
    return "CompositeState [id: " + id + "]";
}

/*
 * ParallelState
 */
ParallelState::ParallelState(const QString& id, State* parent) :
        State(id, parent)
{
    delegate->setChildMode(QState::ParallelStates);
}

ParallelState::~ParallelState()
{
}

bool ParallelState::initialize()
{
    if (initialized)
    {
        qWarning() <<toString() <<"already initialized";

        return true;
    }

    //initialize state
    if (State::initialize())
    {
        qWarning() <<toString() <<"initialization failed: base class initialization failed";

        return false;
    }

    initialized = true;

    return true;
}

QString ParallelState::toString() const
{
    return "Parallel [id: " + id + "]";
}

/*
 * StateMachine
 */
StateMachine::StateMachine(const QString& id, State* parent) :
    State(id, parent)
{
    delete State::delegate;
    delegate = new QStateMachine((parent != NULL) ? parent->getDelegate() : NULL);
}

StateMachine::~StateMachine()
{
}

const QString& StateMachine::getInitial() const
{
    return initial;
}

void StateMachine::setInitial(const QString& initial)
{
    this->initial = initial;
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

const StateMachine* StateMachine::getStateMachine() const
{
    return this;
}

QStateMachine* StateMachine::getDelegate() const
{
    return delegate;
}

bool StateMachine::initialize()
{
    if (initialized)
    {
        qWarning() <<toString() <<"already initialized";

        return true;
    }

    //initialize state
    if (State::initialize())
    {
        qWarning() <<toString() <<"initialization failed: base class initialization failed";

        return false;
    }

    //set initial state
    const AbstractState* initialState = findState(initial);
    if (initialState == NULL)
    {
        qWarning() <<toString() <<"initialization failed: couldn't find initial state \"" <<initial <<"\"";

        return false;
    }

    delegate->setInitialState(initialState->getDelegate());

    //connect signals
    connect(getDelegate(), SIGNAL(started()), this, SLOT(eventStarted()));
    connect(getDelegate(), SIGNAL(stopped()), this, SLOT(eventStopped()));

    initialized = true;

    return true;
}

QString StateMachine::toString() const
{
    return "StateMachine [id: " + id + "]";
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
 * StateMachineTest
 */
StateMachineTest::StateMachineTest()
{
    qDebug() <<"test";

    sm = new StateMachine("stateMachine1");
    sm->setInitial("p1");

    ParallelState* p = new ParallelState("p1", sm);

    CompositeState* s1 = new CompositeState("s1", p);
    CompositeState* s1_1 = new CompositeState("s1_1", s1);
    FinalState* f1_1 = new FinalState("fin1", s1);

    s1->setInitial("s1_1");
    s1_1->addTransition(new AbstractTransition(s1_1->getId(), f1_1->getId(), QString("f1")));

    CompositeState* s2 = new CompositeState("s2", p);
    CompositeState* s2_1 = new CompositeState("s2_1", s2);
    FinalState* f2_1 = new FinalState("fin2", s2);

    s2->setInitial("s2_1");
    s2_1->addTransition(new AbstractTransition(s2_1->getId(), f2_1->getId(), QString("f2")));


    sm->initialize();
    sm->start();

    QTimer::singleShot(0, this, SLOT(triggerEvents()));
}

void StateMachineTest::triggerEvents()
{
    sm->postDelayedEvent(new StringEvent("f1"), 5000);
    sm->postDelayedEvent(new StringEvent("f2"), 10000);
}
