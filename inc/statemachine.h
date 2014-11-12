#ifndef STATEMACHINE_H
#define STATEMACHINE_H

#include <transition.h>

#include <string>
#include <vector>

#include <QAbstractState>
#include <QFinalState>
#include <QState>
#include <QStateMachine>

namespace hfsmexec
{
    class State;
    class StateMachine;

    class AbstractState : public QObject
    {
        Q_OBJECT

        friend class StateMachineBuilder;

        public:
            AbstractState(const QString& stateId, const QString& parentStateId = "");
            virtual ~AbstractState();

            const QString& getId() const;

            State* getParentState() const;
            QList<AbstractTransition*> getTransitions() const;
            virtual AbstractState* getState(const QString& stateId);
            virtual StateMachine* getStateMachine();

            virtual QAbstractState* getDelegate() const = 0;
            virtual bool initialize() = 0;
            virtual QString toString() const = 0;

        protected:
            QString stateId;
            QString parentStateId;
            StateMachine* stateMachine;
            QList<AbstractTransition*> transitions;
    };

    class FinalState : public AbstractState
    {
        Q_OBJECT

        public:
            FinalState(const QString& stateId, const QString& parentStateId = "");
            ~FinalState();

            virtual QFinalState* getDelegate() const;
            virtual bool initialize();
            virtual QString toString() const;

        private:
            QFinalState* delegate;
    };

    class State : public AbstractState
    {
        Q_OBJECT

        public:
            State(const QString& stateId, const QString& parentStateId = "");
            ~State();

            const QList<AbstractState*> getChildStates() const;
            virtual AbstractState* getState(const QString& stateId);

            virtual QState* getDelegate() const;
            virtual bool initialize() = 0;
            virtual QString toString() const = 0;

        protected slots:
            virtual void eventEntered();
            virtual void eventExited();
            virtual void eventFinished();

        protected:
            std::vector<AbstractTransition*> transitions;
            QState* delegate;
    };

    class CompositeState : public State
    {
        Q_OBJECT

        public:
            CompositeState(const QString& stateId, const QString &initialStateId, const QString& parentStateId = "");
            ~CompositeState();

            virtual bool initialize();
            virtual QString toString() const;

        private:
            QString initialStateId;
    };

    class ParallelState : public State
    {
        Q_OBJECT

        public:
            ParallelState(const QString& stateId, const QString& parentStateId = "");
            ~ParallelState();

            virtual bool initialize();
            virtual QString toString() const;
    };

    class StateMachine : public State
    {
        Q_OBJECT

        friend class StateMachineBuilder;

        public:
            ~StateMachine();

            void start() const;
            void stop() const;

            int postDelayedEvent(QEvent* event, int delay);
            void postEvent(QEvent* event, QStateMachine::EventPriority priority = QStateMachine::NormalPriority);

            StateMachine* getStateMachine();

            virtual QStateMachine* getDelegate() const;
            virtual bool initialize();
            virtual QString toString() const;

        protected slots:
            virtual void eventStarted();
            virtual void eventStopped();

        protected:
            QStateMachine* delegate;

        private:
            QString initialId;

            StateMachine(const QString& initialId);
    };

    class StateMachineBuilder
    {
        public:
            StateMachineBuilder();
            ~StateMachineBuilder();

            void addState(AbstractState* state);
            void addTransition(AbstractTransition* transition);

            StateMachine* create(const QString& initialStateId);

            StateMachineBuilder& operator<<(AbstractState* state);
            StateMachineBuilder& operator<<(AbstractTransition* transition);

        private:
            QList<AbstractState*> states;
            QList<AbstractTransition*> transitions;

            AbstractState* getState(const QString& stateId);
    };

    class StateMachineTest : public QObject
    {
        Q_OBJECT

        private:
            StateMachine* sm;

        public:
            StateMachineTest();

        private slots:
            void triggerEvents();
    };

}

#endif
