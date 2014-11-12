#include <statemachine.h>
#include <transition.h>

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
