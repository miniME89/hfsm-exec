#include <builder.h>

using namespace hfsmexec;

/*
 * StateMachineBuilder
 */
const Logger* StateMachineBuilder::logger = Logger::getLogger(LOGGER_BUILDER);

StateMachineBuilder::StateMachineBuilder() :
    stateMachine(NULL)
{

}

StateMachineBuilder::~StateMachineBuilder()
{

}

void StateMachineBuilder::addState(StateMachine* stateMachine)
{
    if (this->stateMachine == NULL)
    {
        this->stateMachine = stateMachine;
    }
    else
    {
        states.append(stateMachine);
    }
}

void StateMachineBuilder::addState(AbstractState* state)
{
    states.append(state);
}

void StateMachineBuilder::addTransition(AbstractTransition* transition)
{
    if (transition->stateMachine != NULL)
    {
        logger->warning("can't add transition: transition is already part of another state machine");

        return;
    }

    transitions.append(transition);
}

void StateMachineBuilder::addDataflow(Dataflow* dataflow)
{
    dataflows.append(dataflow);
}

StateMachine* StateMachineBuilder::build()
{
    if (stateMachine == NULL)
    {
        logger->warning("can't build state machine: no instance of StateMachine was provided");

        return NULL;
    }

    logger->info("create state machine");

    //link states
    logger->info("link states");
    for (int i = 0; i < states.size(); i++)
    {
        AbstractState* state = states[i];

        //find parent state
        AbstractState* parentState = getState(state->parentStateId);
        if (parentState == NULL)
        {
            logger->warning(QString("initialization failed: couldn't find parent state \"%1\"").arg(state->parentStateId));

            return NULL;
        }

        //link state
        logger->info(QString("link child state \"%1\" with parent state \"%2\"").arg(state->getId()).arg(parentState->getId()));
        parentState->childStates.append(state);
        state->setParent(parentState);
        state->getDelegate()->setParent(parentState->getDelegate());

        //set state machine
        state->stateMachine = stateMachine;
    }

    //link dataflows
    for (int i = 0; i < dataflows.size(); i++)
    {
        Dataflow* dataflow = dataflows[i];

        //find source state
        AbstractState* sourceState = getState(dataflow->getSourceStateId());
        if (sourceState == NULL)
        {
            logger->warning(QString("initialization failed: couldn't find dataflow source state \"%1\"").arg(dataflow->getSourceStateId()));

            return NULL;
        }

        //find target state
        AbstractState* targetState = getState(dataflow->getTargetStateId());
        if (targetState == NULL)
        {
            logger->warning(QString("initialization failed: couldn't find dataflow target state \"%1\"").arg(dataflow->getTargetStateId()));

            return NULL;
        }

        Value sourceParameters;
        sourceParameters["input"] = &sourceState->getInput();
        sourceParameters["output"] = &sourceState->getOutput();

        Value targetParameters;
        targetParameters["input"] = &targetState->getInput();
        targetParameters["output"] = &targetState->getOutput();

        targetParameters.getValue(dataflow->getTo()) = &sourceParameters.getValue(dataflow->getFrom());

        sourceState->dataflows.append(dataflow);
    }

    //initialize state machine
    logger->info("initialize state machine");
    stateMachine->stateMachine = stateMachine;
    if (!stateMachine->initialize())
    {
        logger->warning("initialization failed: initialization of state machine failed");

        return NULL;
    }

    //initialize states
    logger->info(QString("initialize %1 states").arg(states.size()));
    for (int i = 0; i < states.size(); i++)
    {
        AbstractState* state = states[i];

        //initialize state
        logger->info(QString("[%1] initialize state \"%2\"").arg(i).arg(state->getId()));
        if (!state->initialize())
        {
            logger->warning("initialization failed: initialization of states failed");

            return NULL;
        }
    }

    //initialize transitions
    logger->info(QString("initialize %1 transitions").arg(transitions.size()));
    for (int i = 0; i < transitions.size(); i++)
    {
        AbstractTransition* transition = transitions[i];

        //find source state
        AbstractState* sourceState = getState(transition->sourceStateId);
        if (sourceState == NULL)
        {
            logger->warning(QString("initialization failed: couldn't find transition source state \"%1\"").arg(transition->sourceStateId));

            return NULL;
        }

        //find target state
        AbstractState* targetState = getState(transition->targetStateId);
        if (targetState == NULL)
        {
            logger->warning(QString("initialization failed: couldn't find transition target state \"%1\"").arg(transition->targetStateId));

            return NULL;
        }

        sourceState->transitions.append(transition);
        transition->stateMachine = stateMachine;
        transition->sourceState = sourceState;
        transition->targetState = targetState;

        logger->info(QString("[%1] initialize transition \"%2\"").arg(i).arg(transition->getId()));
        if (!transition->initialize())
        {
            logger->warning("initialization failed: couldn't initialize all transitions");

            return NULL;
        }
    }

    logger->info("successfully created state machine");

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

StateMachineBuilder& StateMachineBuilder::operator<<(Dataflow* dataflow)
{
    addDataflow(dataflow);

    return *this;
}

AbstractState* StateMachineBuilder::getState(const QString& stateId)
{
    if (stateMachine->getId() == stateId)
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
