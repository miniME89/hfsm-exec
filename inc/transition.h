#ifndef TRANSITION_H
#define TRANSITION_H

#include <QAbstractTransition>
#include <QEvent>

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

    class StringEvent : public AbstractEvent
    {
        public:
            static const QEvent::Type typeId;

            StringEvent(const QString &value);
            ~StringEvent();

            virtual QString toString() const;

            QString value;
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
}

#endif
