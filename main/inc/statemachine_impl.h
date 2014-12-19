/*
 *  Copyright (C) 2014 Marcel Lehwald
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.

 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef STATEMACHINE_IMPL_H
#define STATEMACHINE_IMPL_H

#include <statemachine.h>
#include <plugins.h>

#include <QFinalState>
#include <QState>
#include <QStateMachine>
#include <QMutex>

namespace hfsmexec
{
    class NamedEvent : public AbstractEvent
    {
        public:
            static const QEvent::Type type;

            NamedEvent(const QString& eventName);
            ~NamedEvent();

            const QString& getEventName() const;
            void setEventName(const QString& eventName);

            const QString& getOrigin() const;
            void setOrigin(const QString& origin);

            const QString& getMessage() const;
            void setMessage(const QString& message);

            virtual QString toString() const;

        private:
            QString eventName;
            QString origin;
            QString message;
    };

    class NamedTransition : public AbstractTransition
    {
        public:
            NamedTransition(const QString& transitionId, const QString& sourceStateId, const QString& targetStateId, const QString& eventName);

            virtual QString toString() const;

        protected:
            virtual bool eventTest(QEvent* e);
            virtual void onTransition(QEvent* e);

        private:
            QString eventName;
    };

    class InternalEvent : public QEvent
    {
        public:
            static const QEvent::Type type;

            InternalEvent(const QString& eventName);

            const QString& getEventName() const;

        private:
            QString eventName;
    };

    class InternalTransition : public QAbstractTransition
    {
        public:
            InternalTransition(const QString& eventName);

        protected:
            virtual bool eventTest(QEvent* e);
            virtual void onTransition(QEvent* e);

        private:
            QString eventName;
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

    class InvokeState : public AbstractComplexState
    {
        Q_OBJECT

        public:
            InvokeState(const QString& stateId, const QString& type, const QString& parentStateId = "");
            virtual ~InvokeState();

            const Value& getEndpoint() const;
            void setEndpoint(const Value& value);

            const Value& getInputParameters() const;
            void setInputParameters(const Value& value);

            const Value& getOutputParameters() const;
            void setOutputParameters(const Value& value);

            CommunicationPlugin* getCommunicationPlugin() const;
            void setCommunicationPlugin(CommunicationPlugin* value);

            void done();

            virtual bool initialize();
            virtual QString toString() const;

        protected slots:
            virtual void eventEntered();
            virtual void eventExited();
            virtual void eventFinished();

        private:
            QString type;
            Value endpoint;
            Value inputParameters;
            Value outputParameters;
            CommunicationPlugin* communicationPlugin;
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
}

#endif
