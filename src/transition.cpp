#include <statemachine.h>
#include <transition.h>

#include <QDebug>

using namespace hfsmexec;

/*
 * StringEvent
 */
StringEvent::StringEvent(const QString &value) :
        QEvent(QEvent::Type(QEvent::User + 1))
{
    this->value = value;
}

StringEvent::~StringEvent()
{

}

/*
 * AbstractTransition
 */
AbstractTransition::AbstractTransition(const QString sourceId, const QString targetId, const QString &value) :
        initialized(false),
        sourceId(sourceId),
        targetId(targetId),
        sourceState(NULL),
        targetState(NULL)
{
    this->value = value;
}

AbstractTransition::AbstractTransition(const QString sourceId, const QString targetId) :
        initialized(false),
        sourceId(sourceId),
        targetId(targetId),
        sourceState(NULL),
        targetState(NULL)
{
}

AbstractTransition::~AbstractTransition()
{

}

const bool& AbstractTransition::isInitialized() const
{
    return initialized;
}

const QString AbstractTransition::getSourceId() const
{
    return sourceId;
}

const QString AbstractTransition::getTargetId() const
{
    return targetId;
}

const AbstractState* AbstractTransition::getSourceState() const
{
    return sourceState;
}

const AbstractState* AbstractTransition::getTargetState() const
{
    return targetState;
}

bool AbstractTransition::initialize(const StateMachine* stateMachine)
{
    if (initialized)
    {
        qWarning() <<"transition already initialized";

        return true;
    }

    qDebug() <<"initialize transition";

    //find source state
    const AbstractState* source = stateMachine->findState(sourceId);
    if (source == NULL)
    {
        qWarning() <<"transition initialization failed: couldn't find source state \"" <<sourceId <<"\"";

        return false;
    }

    //find target state
    const AbstractState* target = stateMachine->findState(targetId);
    if (target == NULL)
    {
        qWarning() <<"transition initialization failed: couldn't find target state \"" <<targetId <<"\"";

        return false;
    }

    sourceState = source;
    targetState = target;

    //cast abstract state
    QState* state = dynamic_cast<QState*>(sourceState->getDelegate());
    if (state == NULL)
    {
        qWarning() <<"transition initialization failed: source delegate is not of type QState";

        return false;
    }

    qDebug() <<"add transition from " <<source->getId() <<" to " <<target->getId();

    setTargetState(targetState->getDelegate());
    state->addTransition(this);

    initialized = true;

    return true;
}

bool AbstractTransition::eventTest(QEvent* e)
{
    if (e->type() != QEvent::Type(QEvent::User + 1))
    {
        return false;
    }

    StringEvent* event = static_cast<StringEvent*>(e);

    return event->value == value;
}

void AbstractTransition::onTransition(QEvent* e)
{

}
