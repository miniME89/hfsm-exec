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

#include <decoder_impl.h>
#include <logger.h>
#include <statemachine_impl.h>
#include <value_container.h>

#include <easylogging++.h>

#include <QTextStream>

#include <sstream>


using namespace hfsmexec;

/*
 * XmlDecoder
 */
XmlDecoder::XmlDecoder() :
    AbstractDecoder("XML")
{

}

StateMachine* XmlDecoder::decode(const QString& data)
{
    //parse XML
    pugi::xml_document doc;
    pugi::xml_parse_result result = doc.load_buffer(data.toStdString().c_str(), data.size());
    if (result.status != pugi::status_ok)
    {
        CLOG(WARNING, LOG_DECODER) <<"couldn't parse XML: " <<result.description();

        return NULL;
    }

    //get root element
    pugi::xml_node root = doc.first_child();
    if (std::string(root.name()) != "stateMachine")
    {
        CLOG(WARNING, LOG_DECODER) <<"invalid XML encoding: root element needs to be a \"stateMachine\" element";

        return NULL;
    }

    //decode state machine recursively
    StateMachineBuilder builder;
    decodeStateMachine(root, builder);

    return builder.build();
}

bool XmlDecoder::decodeChilds(pugi::xml_node& node, StateMachineBuilder& builder, AbstractState* parentState)
{
    CLOG(INFO, LOG_DECODER) <<"decode childs";

    for (pugi::xml_node child = node.first_child(); child; child = child.next_sibling())
    {
        QString tagName = child.name();
        if (tagName == "composite")
        {
           if (!decodeComposite(child, builder, parentState))
           {
               return false;
           }
        }
        else if (tagName == "parallel")
        {
           if (!decodeParallel(child, builder, parentState))
           {
               return false;
           }
        }
        else if (tagName == "invoke")
        {
           if (!decodeInvoke(child, builder, parentState))
           {
               return false;
           }
        }
        else if (tagName == "final")
        {
           if (!decodeFinal(child, builder, parentState))
           {
               return false;
           }
        }
    }

    return true;
}

bool XmlDecoder::decodeTransitions(pugi::xml_node& node, StateMachineBuilder& builder, AbstractState* sourceState)
{
    CLOG(INFO, LOG_DECODER) <<"decode transitions";

    for (pugi::xml_node transitionElement = node.child("transition"); transitionElement; transitionElement = transitionElement.next_sibling())
    {
        QString id = transitionElement.attribute("id").value();
        QString target = transitionElement.attribute("target").value();
        QString event = transitionElement.attribute("event").value();

        CLOG(INFO, LOG_DECODER) <<"create NamedTransition: id=" <<id <<", source=" <<sourceState->getId() <<", target=" <<target <<", event=" <<event;

        NamedTransition* transition = new NamedTransition(id, sourceState->getId(), target, event);
        builder <<transition;
    }

    return true;
}

bool XmlDecoder::decodeStateMachine(pugi::xml_node& node, StateMachineBuilder& builder)
{
    CLOG(INFO, LOG_DECODER) <<"decode stateMachine";

    QString initial = node.attribute("initial").value();
    pugi::xml_node childs = node.child("childs");

    CLOG(INFO, LOG_DECODER) <<"create StateMachine: initial=" <<initial;

    StateMachine* stateMachine = new StateMachine(initial);
    builder <<stateMachine;

    if (!decodeChilds(childs, builder, stateMachine))
    {
        return false;
    }

    return true;
}

bool XmlDecoder::decodeComposite(pugi::xml_node& node, StateMachineBuilder& builder, AbstractState* parentState)
{
    CLOG(INFO, LOG_DECODER) <<"decode composite";

    QString id = node.attribute("id").value();
    QString initial = node.attribute("initial").value();
    pugi::xml_node transitions = node.child("transitions");
    pugi::xml_node childs = node.child("childs");

    CLOG(INFO, LOG_DECODER) <<"create CompositeState: id=" <<id <<", initial=" <<initial <<", parent=" <<parentState->getId();

    CompositeState* composite = new CompositeState(id, initial, parentState->getId());
    builder <<composite;

    if (!decodeTransitions(transitions, builder, composite))
    {
        return false;
    }

    if (!decodeChilds(childs, builder, composite))
    {
        return false;
    }

    return true;
}

bool XmlDecoder::decodeParallel(pugi::xml_node& node, StateMachineBuilder& builder, AbstractState* parentState)
{
    CLOG(INFO, LOG_DECODER) <<"decode parallel";

    QString id = node.attribute("id").value();
    pugi::xml_node transitions = node.child("transitions");
    pugi::xml_node childs = node.child("childs");

    CLOG(INFO, LOG_DECODER) <<"create ParallelState: id=" <<id <<", parent=" <<parentState->getId();

    ParallelState* parallel = new ParallelState(id, parentState->getId());
    builder <<parallel;

    if (!decodeTransitions(transitions, builder, parallel))
    {
        return false;
    }

    if (!decodeChilds(childs, builder, parallel))
    {
        return false;
    }

    return true;
}

bool XmlDecoder::decodeInvoke(pugi::xml_node& node, StateMachineBuilder& builder, AbstractState* parentState)
{
    CLOG(INFO, LOG_DECODER) <<"decode invoke";

    QString id = node.attribute("id").value();
    QString type = node.attribute("type").value();
    pugi::xml_node endpoint = node.child("endpoint");
    pugi::xml_node transitions = node.child("transitions");

    std::ostringstream stream;
    endpoint.print(stream);
    QString endPointStr = stream.str().c_str();

    ValueContainer endpointParameter;
    endpointParameter.fromXml(endPointStr);

    CLOG(INFO, LOG_DECODER) <<"create InvokeState: id=" <<id <<", type=" <<type <<", parent=" <<parentState->getId();

    InvokeState* invoke = new InvokeState(id, type, parentState->getId());
    builder <<invoke;

    if (!decodeTransitions(transitions, builder, invoke))
    {
        return false;
    }

    return true;
}

bool XmlDecoder::decodeFinal(pugi::xml_node& node, StateMachineBuilder& builder, AbstractState* parentState)
{
    CLOG(INFO, LOG_DECODER) <<"decode final";

    QString id = node.attribute("id").value();

    CLOG(INFO, LOG_DECODER) <<"create FinalState: id=" <<id <<", parent=" <<parentState->getId();

    FinalState* composite = new FinalState(id, parentState->getId());
    builder <<composite;

    return true;
}

/*
 * JsonDecoder
 */
JsonDecoder::JsonDecoder() :
    AbstractDecoder("JSON")
{

}

StateMachine* JsonDecoder::decode(const QString& data)
{

}

/*
 * YamlDecoder
 */
YamlDecoder::YamlDecoder() :
    AbstractDecoder("YAML")
{

}

StateMachine* YamlDecoder::decode(const QString &data)
{

}
