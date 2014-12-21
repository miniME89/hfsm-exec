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

#ifndef STATEMACHINE_H
#define STATEMACHINE_H

#define LOGGER_STATEMACHINE "statemachine"

#include <logger.h>
#include <value.h>

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

        protected:
            static const Logger* logger;
    };

    class AbstractTransition : public QAbstractTransition
    {
        friend class StateMachineBuilder;

        public:
            AbstractTransition(const QString transitionId, const QString sourceStateId, const QString targetStateId);
            virtual ~AbstractTransition();

            const QString& getId() const;
            AbstractState* getSourceState() const;
            AbstractState* getTargetState() const;
            StateMachine* getStateMachine();

            bool initialize();

            virtual QString toString() const = 0;

        protected:
            static const Logger* logger;
            QString transitionId;
            QString sourceStateId;
            QString targetStateId;
            AbstractState* sourceState;
            AbstractState* targetState;
            StateMachine* stateMachine;

            virtual bool eventTest(QEvent* e) = 0;
            virtual void onTransition(QEvent* e) = 0;
    };

    class AbstractState : public QObject
    {
        Q_OBJECT

        friend class StateMachineBuilder;

        public:
            AbstractState(const QString& stateId, const QString& parentStateId = "");
            virtual ~AbstractState();

            const QString& getId() const;
            const QString& getParentStateId() const;
            const StateMachine* getStateMachine() const;

            Value& getParameters();
            Value& getInputParameters();
            Value& getOutputParameters();

            AbstractState* getParentState() const;

            const QList<AbstractState*>& getChildStates() const;
            AbstractState* getChildState(const QString& stateId);

            const QList<AbstractTransition*>& getTransitions() const;
            AbstractTransition* getTransition(const QString& transitionId);

            AbstractState* findState(const QString& stateId);

            virtual QAbstractState* getDelegate() const = 0;
            virtual bool initialize() = 0;
            virtual QString toString() const = 0;

        protected:
            static const Logger* logger;
            QString stateId;
            QString parentStateId;
            StateMachine* stateMachine;
            Value parameters;
            QList<AbstractTransition*> transitions;
            QList<AbstractState*> childStates;
    };

    class AbstractComplexState : public AbstractState
    {
        Q_OBJECT

        public:
            AbstractComplexState(const QString& stateId, const QString& parentStateId = "");
            virtual ~AbstractComplexState();

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
