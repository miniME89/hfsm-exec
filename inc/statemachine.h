#ifndef STATEMACHINE_H
#define STATEMACHINE_H

#include <QEvent>
#include <QAbstractTransition>
#include <QState>
#include <QAbstractState>

namespace hfsmexec
{
    class AbstractState;
    class StateMachine;

    class AbstractEvent : public QEvent
    {
        public:
            AbstractEvent(Type type);

            virtual QString toString() const = 0;
    };

    class AbstractTransition : public QAbstractTransition
    {
        friend class StateMachineBuilder;

        public:
            AbstractTransition(const QString transitionId, const QString sourceStateId, const QString targetStateId);
            ~AbstractTransition();

            const QString& getId() const;
            AbstractState* getSourceState() const;
            AbstractState* getTargetState() const;
            StateMachine* getStateMachine();

            bool initialize();

            virtual QString toString() const = 0;

        protected:
            QString transitionId;
            QString sourceStateId;
            QString targetStateId;
            AbstractState* sourceState;
            AbstractState* targetState;
            StateMachine* stateMachine;

            virtual bool eventTest(QEvent* e) = 0;
            void onTransition(QEvent* e);
    };

    class AbstractState : public QObject
    {
        Q_OBJECT

        friend class StateMachineBuilder;

        public:
            AbstractState(const QString& stateId, const QString& parentStateId = "");
            virtual ~AbstractState();

            const QString& getId() const;

            AbstractState* getParentState() const;
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

    class AbstractComplexState : public AbstractState
    {
        Q_OBJECT

        public:
            AbstractComplexState(const QString& stateId, const QString& parentStateId = "");
            ~AbstractComplexState();

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
            QState* delegate;
    };
}

#endif
