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

#include <plugin_dot.h>

using namespace hfsmexec;

/*
 * Exporter
 */
Exporter::Exporter() :
    ExporterPlugin("DOT") {

}

Exporter::~Exporter() {

}

QString Exporter::exportStateMachine(StateMachine* stateMachine) {
    QString dot;
    dot.append("digraph {\n");
    dot.append("    graph[compound=true];\n");
    decode(dot, stateMachine, 1);
    dot.append("}\n");

    return dot;
}

void Exporter::decode(QString& dot, AbstractState* state, int level, const QString& prefix) {
    QString nodeId = prefix + state->getId();
    dot.append(QString("%1subgraph cluster_%2 {\n").arg(QString("    ").repeated(level)).arg(nodeId));
    dot.append(QString("%1label = %2; \n").arg(QString("    ").repeated(level + 1)).arg(state->getId()));
    dot.append(QString("%1cluster_%2_node [shape=point style=invis]; \n").arg(QString("    ").repeated(level + 1)).arg(nodeId));

    QList<AbstractState*> childs = state->getChildStates();
    for (int i = 0; i < childs.size(); i++) {
        decode(dot, childs[i], level + 1, prefix + state->getId() + "_");
    }

    dot.append(QString("%1}\n").arg(QString("    ").repeated(level)));

    QList<AbstractTransition*> transitions = state->getTransitions();
    for (int i = 0; i < transitions.size(); i++) {
        QString sourceNodeId = prefix + transitions[i]->getSourceState()->getId();
        QString targetNodeId = prefix + transitions[i]->getTargetState()->getId();
        dot.append(QString("%1cluster_%2_node -> cluster_%3_node [ltail=cluster_%2 lhead=cluster_%3, minlen=1.5];\n").arg(QString("    ").repeated(level)).arg(sourceNodeId).arg(targetNodeId));
    }
}



