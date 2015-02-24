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

#include <plugin_smdl.h>
#include <QUuid>
#include <sstream>

using namespace hfsmexec;

/*
 * Importer
 */
Importer::Importer() :
    ImporterPlugin("SMDL/XML")
{

}

Importer::~Importer()
{

}

StateMachine* Importer::importStateMachine(const QString& data)
{
    //parse XML
    pugi::xml_document doc;
    pugi::xml_parse_result result = doc.load_buffer(data.toStdString().c_str(), data.size());
    if (result.status != pugi::status_ok)
    {
        logger->warning(QString("couldn't parse XML: %1").arg(result.description()));

        return NULL;
    }

    //get root element
    pugi::xml_node root = doc.first_child();
    if (std::string(root.name()) != "statemachine")
    {
        logger->warning("invalid XML encoding: root element needs to be a \"statemachine\" element");

        return NULL;
    }

    //decode state machine recursively
    StateMachineBuilder builder;
    if (!decodeChilds(doc, builder, NULL))
    {
        logger->warning("couldn't decode state machine");

        return NULL;
    }

    return builder.build();
}

bool Importer::decodeChilds(pugi::xml_node& node, StateMachineBuilder& builder, AbstractState* parentState)
{
    logger->info("decode childs");

    for (pugi::xml_node child = node.first_child(); child; child = child.next_sibling())
    {
        AbstractState* state = NULL;
        QString tagName = child.name();
        if (tagName == "statemachine")
        {
            state = decodeStateMachine(child, builder, parentState);
        }
        else if (tagName == "composite")
        {
           state = decodeComposite(child, builder, parentState);
        }
        else if (tagName == "parallel")
        {
           state = decodeParallel(child, builder, parentState);
        }
        else if (tagName == "invoke")
        {
           state = decodeInvoke(child, builder, parentState);
        }
        else if (tagName == "final")
        {
           state = decodeFinal(child, builder, parentState);
        }

        if (state == NULL)
        {
            return false;
        }

        pugi::xml_node input = child.child("input");
        pugi::xml_node output = child.child("output");
        pugi::xml_node dataflows = child.child("dataflows");
        pugi::xml_node transitions = child.child("transitions");
        pugi::xml_node childs = child.child("childs");

        if (!decodeInput(input, state))
        {
            return false;
        }

        if (!decodeOutput(output, state))
        {
            return false;
        }

        if (!decodeDataflows(dataflows, builder, state))
        {
            return false;
        }

        if (!decodeTransitions(transitions, builder, state))
        {
            return false;
        }

        if (!decodeChilds(childs, builder, state))
        {
            return false;
        }
    }

    return true;
}

bool Importer::decodeTransitions(pugi::xml_node& node, StateMachineBuilder& builder, AbstractState* state)
{
    logger->info("decode transitions");

    for (pugi::xml_node transitionElement = node.child("transition"); transitionElement; transitionElement = transitionElement.next_sibling())
    {
        QString id = transitionElement.attribute("id").value();
        QString target = transitionElement.attribute("target").value();
        QString event = transitionElement.attribute("event").value();
        QString condition = transitionElement.attribute("condition").value();

        if (event == "finish" || event == "state.success" || event == "state.error")
        {
            event = event + "." + state->getId();
        }

        logger->info(QString("decode ConditionalTransition: id=%1, source=%2, target=%3, event=%4, condition=%5").arg(id).arg(state->getId()).arg(target).arg(event).arg(condition));

        ConditionalTransition* transition = new ConditionalTransition(id, state->getId(), target, event, condition);
        builder <<transition;
    }

    return true;
}

bool Importer::decodeInput(pugi::xml_node& node, AbstractState* state)
{
    logger->info("decode input");

    std::ostringstream stream;
    node.print(stream);
    QString nodeStr = stream.str().c_str();

    Value parameters;
    if (!parameters.fromXml(nodeStr))
    {
        return false;
    }

    state->setInput(parameters["input"]);

    return true;
}

bool Importer::decodeOutput(pugi::xml_node& node, AbstractState* state)
{
    logger->info("decode output");

    std::ostringstream stream;
    node.print(stream);
    QString nodeStr = stream.str().c_str();

    Value parameters;
    if (!parameters.fromXml(nodeStr))
    {
        return false;
    }

    state->setOutput(parameters["output"]);

    return true;
}

bool Importer::decodeDataflows(pugi::xml_node& node, StateMachineBuilder& builder, AbstractState* state)
{
    logger->info("decode dataflows");

    for (pugi::xml_node dataflowElement = node.child("dataflow"); dataflowElement; dataflowElement = dataflowElement.next_sibling())
    {
        QString source = dataflowElement.attribute("source").value();
        QString from = dataflowElement.attribute("from").value();
        QString to = dataflowElement.attribute("to").value();

        logger->info(QString("decode dataflow: sourceStateId=%1, targetStateId=%2, from=%3, to=%4").arg(source).arg(state->getId()).arg(from).arg(to));

        Dataflow* dataflow = new Dataflow(source, state->getId(), from, to);
        builder <<dataflow;
    }

    return true;
}

AbstractState* Importer::decodeStateMachine(pugi::xml_node& node, StateMachineBuilder& builder, AbstractState* parentState)
{
    QString id = node.attribute("id").value();
    QString initial = node.attribute("initial").value();

    if (parentState == NULL)
    {
        id = QUuid::createUuid().toString().remove(QChar('{')).remove(QChar('}'));
    }

    QString parentStateId = "";
    if (parentState != NULL)
    {
        parentStateId = parentState->getId();
    }

    logger->info(QString("decode StateMachine: id=%1, initial=%2, parent=%3").arg(id).arg(initial).arg(parentStateId));

    StateMachine* state = new StateMachine(id, initial, parentStateId);
    builder <<state;

    return state;
}

AbstractState* Importer::decodeComposite(pugi::xml_node& node, StateMachineBuilder& builder, AbstractState* parentState)
{
    QString id = node.attribute("id").value();
    QString initial = node.attribute("initial").value();

    logger->info(QString("decode CompositeState: id=%1, initial=%2, parent=%3").arg(id).arg(initial).arg(parentState->getId()));

    CompositeState* state = new CompositeState(id, initial, parentState->getId());
    builder <<state;

    return state;
}

AbstractState* Importer::decodeParallel(pugi::xml_node& node, StateMachineBuilder& builder, AbstractState* parentState)
{
    QString id = node.attribute("id").value();

    logger->info(QString("decode ParallelState: id=%1, parent=%2").arg(id).arg(parentState->getId()));

    ParallelState* state = new ParallelState(id, parentState->getId());
    builder <<state;

    return state;
}

AbstractState* Importer::decodeInvoke(pugi::xml_node& node, StateMachineBuilder& builder, AbstractState* parentState)
{
    QString id = node.attribute("id").value();
    pugi::xml_node endpoint = node.child("endpoint");
    QString binding = endpoint.attribute("binding").value();

    std::ostringstream stream;
    endpoint.print(stream);
    QString endPointStr = stream.str().c_str();

    Value endpointParameter;
    endpointParameter.fromXml(endPointStr);

    logger->info(QString("decode InvokeState: id=%1, binding=%2, parent=%3").arg(id).arg(binding).arg(parentState->getId()));

    InvokeState* state = new InvokeState(id, binding, parentState->getId());
    state->setEndpoint(endpointParameter["endpoint"]);
    builder <<state;

    return state;
}

AbstractState* Importer::decodeFinal(pugi::xml_node& node, StateMachineBuilder& builder, AbstractState* parentState)
{
    QString id = node.attribute("id").value();

    logger->info(QString("decode FinalState: id=%1, parent=%2").arg(id).arg(parentState->getId()));

    FinalState* state = new FinalState(id, parentState->getId());
    builder <<state;

    return state;
}
