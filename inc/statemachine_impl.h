#ifndef STATEMACHINE_IMPL_H
#define STATEMACHINE_IMPL_H

#include <statemachine.h>

#include <QFinalState>
#include <QState>
#include <QStateMachine>

namespace hfsmexec
{
    class StringEvent : public AbstractEvent
    {
        public:
            static const QEvent::Type typeId;

            StringEvent(const QString &value);
            ~StringEvent();

            virtual QString toString() const;

            QString value;
    };

    class StringTransition : public AbstractTransition
    {
        public:
            StringTransition(const QString transitionId, const QString sourceStateId, const QString targetStateId, const QString &value);

            virtual QString toString() const;

        protected:
            virtual bool eventTest(QEvent* e);

        private:
            QString value;
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


    class CompositeState : public AbstractComplexState
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

    class ParallelState : public AbstractComplexState
    {
        Q_OBJECT

        public:
            ParallelState(const QString& stateId, const QString& parentStateId = "");
            ~ParallelState();

            virtual bool initialize();
            virtual QString toString() const;
    };

    class StateMachine : public AbstractComplexState
    {
        Q_OBJECT

        friend class StateMachineBuilder;

        public:
            StateMachine(const QString& initialId);
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
    };

    class StateMachineBuilder
    {
        public:
            StateMachineBuilder();
            ~StateMachineBuilder();

            void addState(StateMachine* stateMachine);
            void addState(AbstractState* state);
            void addTransition(AbstractTransition* transition);

            StateMachine* build();

            StateMachineBuilder& operator<<(StateMachine* stateMachine);
            StateMachineBuilder& operator<<(AbstractState* state);
            StateMachineBuilder& operator<<(AbstractTransition* transition);

        private:
            StateMachine* stateMachine;
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
