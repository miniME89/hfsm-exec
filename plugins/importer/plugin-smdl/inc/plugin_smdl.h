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

#ifndef PLUGIN_SMDL_H
#define PLUGIN_SMDL_H

#include <plugins.h>
#include <builder.h>
#include <pugixml.hpp>

class Importer : public QObject, public hfsmexec::ImporterPlugin {
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "hfsmexec.Plugins.ImporterPlugin")
    Q_INTERFACES(hfsmexec::ImporterPlugin)

  public:
    Importer();
    virtual ~Importer();

    hfsmexec::StateMachine* importStateMachine(const QString &data);

  private:
    bool decodeChilds(pugi::xml_node& node, hfsmexec::StateMachineBuilder& builder, hfsmexec::AbstractState* parentState);
    bool decodeTransitions(pugi::xml_node& node, hfsmexec::StateMachineBuilder& builder, hfsmexec::AbstractState* state);
    bool decodeInput(pugi::xml_node& node, hfsmexec::AbstractState* state);
    bool decodeOutput(pugi::xml_node& node, hfsmexec::AbstractState* state);
    bool decodeDataflows(pugi::xml_node& node, hfsmexec::StateMachineBuilder& builder, hfsmexec::AbstractState* state);
    hfsmexec::AbstractState* decodeStateMachine(pugi::xml_node& node, hfsmexec::StateMachineBuilder& builder, hfsmexec::AbstractState* parentState);
    hfsmexec::AbstractState* decodeComposite(pugi::xml_node& node, hfsmexec::StateMachineBuilder& builder, hfsmexec::AbstractState* parentState);
    hfsmexec::AbstractState* decodeParallel(pugi::xml_node& node, hfsmexec::StateMachineBuilder& builder, hfsmexec::AbstractState* parentState);
    hfsmexec::AbstractState* decodeInvoke(pugi::xml_node& node, hfsmexec::StateMachineBuilder& builder, hfsmexec::AbstractState* parentState);
    hfsmexec::AbstractState* decodeFinal(pugi::xml_node& node, hfsmexec::StateMachineBuilder& builder, hfsmexec::AbstractState* parentState);
};

#endif
