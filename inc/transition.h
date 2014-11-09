#ifndef TRANSITION_H
#define TRANSITION_H

#include <QAbstractTransition>
#include <QEvent>

namespace hfsmexec
{
    class AbstractState;
    class StateMachine;

    class StringEvent : public QEvent
    {
        public:
            StringEvent(const QString &value);
            ~StringEvent();

            QString value;
    };

    class AbstractTransition : public QAbstractTransition
    {
        public:
            AbstractTransition(const QString sourceId, const QString targetId, const QString &value);
            AbstractTransition(const QString sourceId = "", const QString targetId = "");
            ~AbstractTransition();

            const bool& isInitialized() const;

            const QString getSourceId() const;
            const QString getTargetId() const;
            const AbstractState* getSourceState() const;
            const AbstractState* getTargetState() const;

            bool initialize(const StateMachine* stateMachine);

        protected:
            virtual bool eventTest(QEvent* e);
            virtual void onTransition(QEvent* e);

        private:
            bool initialized;
            QString sourceId;
            QString targetId;
            const AbstractState* sourceState;
            const AbstractState* targetState;

            QString value;
    };
}

#endif
