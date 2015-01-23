#ifndef BUILDER_H
#define BUILDER_H

#define LOGGER_BUILDER "builder"

#include <logger.h>
#include <statemachine.h>

namespace hfsmexec
{
    class StateMachineBuilder
    {
        public:
            StateMachineBuilder();
            ~StateMachineBuilder();

            void addState(StateMachine* stateMachine);
            void addState(AbstractState* state);
            void addTransition(AbstractTransition* transition);
            void addDataflow(Dataflow* dataflow);

            StateMachine* build();

            StateMachineBuilder& operator<<(StateMachine* stateMachine);
            StateMachineBuilder& operator<<(AbstractState* state);
            StateMachineBuilder& operator<<(AbstractTransition* transition);
            StateMachineBuilder& operator<<(Dataflow* dataflow);

        private:
            static const Logger* logger;
            StateMachine* stateMachine;
            QList<AbstractState*> states;
            QList<AbstractTransition*> transitions;
            QList<Dataflow*> dataflows;

            AbstractState* getState(const QString& stateId);
    };
}

#endif
