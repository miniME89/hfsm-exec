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

        public:
            AbstractState(const QString& id, State* parent = NULL);
            virtual ~AbstractState();

            const QString& getId() const;
            const bool& isInitialized() const;

            State* getParentState() const;
            virtual const StateMachine* getStateMachine() const;
            virtual const AbstractState* findState(const QString& id) const;

            virtual QAbstractState* getDelegate() const = 0;
            virtual bool initialize() = 0;
            virtual QString toString() const = 0;

        protected:
            QString id;
            bool initialized;
    };

    class FinalState : public AbstractState
    {
        Q_OBJECT

        public:
            FinalState(const QString& id, State* parent = NULL);
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
            State(const QString& id, State* parent = NULL);
            ~State();

            void addTransition(AbstractTransition* transition);
            void removeTransition(AbstractTransition* transition);

            virtual const AbstractState* findState(const QString& id) const;
            virtual const QList<AbstractState*> getChildStates() const;

            virtual QState* getDelegate() const;
            virtual bool initialize();
            virtual QString toString() const;

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
            CompositeState(const QString& id, State* parent = NULL);
            ~CompositeState();

            const QString& getInitial() const;
            void setInitial(const QString& initial);

            virtual bool initialize();
            virtual QString toString() const;

        private:
            QString initial;
    };

    class ParallelState : public State
    {
        Q_OBJECT

        public:
            ParallelState(const QString& id, State* parent = NULL);
            ~ParallelState();

            virtual bool initialize();
            virtual QString toString() const;
    };

    class StateMachine : public State
    {
        Q_OBJECT

        public:
            StateMachine(const QString& id, State* parent = NULL);
            ~StateMachine();

            const QString& getInitial() const;
            void setInitial(const QString& initial);

            void start() const;
            void stop() const;

            int postDelayedEvent(QEvent* event, int delay);
            void postEvent(QEvent* event, QStateMachine::EventPriority priority = QStateMachine::NormalPriority);

            virtual const StateMachine* getStateMachine() const;

            virtual QStateMachine* getDelegate() const;
            virtual bool initialize();
            virtual QString toString() const;

        protected slots:
            virtual void eventStarted();
            virtual void eventStopped();

        protected:
            QStateMachine* delegate;

        private:
            QString initial;
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
