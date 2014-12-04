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
#include <value_container.h>
#include <sstream>

#include <QDebug>

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
        qDebug() <<"couldn't parse XML: " <<result.description();

        return NULL;
    }

    //get root element
    pugi::xml_node root = doc.first_child();
    if (std::string(root.name()) != "stateMachine")
    {
        qDebug() <<"invalid XML encoding: root element needs to be" <<QString("stateMachine");

        return NULL;
    }

    //decode state machine recursively
    StateMachineBuilder builder;
    decodeStateMachine(root, builder);

    return builder.build();
}

bool XmlDecoder::decodeChilds(pugi::xml_node& node, StateMachineBuilder& builder, AbstractState* parentState)
{
    qDebug() <<"decode childs";

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
    qDebug() <<"decode transitions";

    for (pugi::xml_node transitionElement = node.child("transition"); transitionElement; transitionElement = transitionElement.next_sibling())
    {
        QString target = transitionElement.attribute("target").value();

        NamedTransition* transition = new NamedTransition("TODO", sourceState->getId(), target, "TODO");
        builder <<transition;
    }

    return true;
}

bool XmlDecoder::decodeStateMachine(pugi::xml_node& node, StateMachineBuilder& builder)
{
    qDebug() <<"decode stateMachine";

    QString initial = node.attribute("initial").value();
    pugi::xml_node childs = node.child("childs");

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
    qDebug() <<"decode composite";

    QString id = node.attribute("id").value();
    QString initial = node.attribute("initial").value();
    pugi::xml_node transitions = node.child("transitions");
    pugi::xml_node childs = node.child("childs");

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
    qDebug() <<"decode parallel";

    QString id = node.attribute("id").value();
    pugi::xml_node transitions = node.child("transitions");
    pugi::xml_node childs = node.child("childs");

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
    qDebug() <<"decode invoke";

    QString id = node.attribute("id").value();
    pugi::xml_node endpoint = node.child("endpoint");

    std::ostringstream stream;
    endpoint.print(stream);
    QString endPointStr = stream.str().c_str();

    ValueContainer endpointParameter;
    endpointParameter.fromXml(endPointStr);

    QString test;
    qDebug() <<endpointParameter.toJson(test);
    qDebug() <<test;

    QString url;
    endpointParameter["url"].get(url);
    qDebug() <<url;

    InvokeState* invoke = new InvokeState(id, parentState->getId());
    builder <<invoke;

    return true;
}

bool XmlDecoder::decodeFinal(pugi::xml_node& node, StateMachineBuilder& builder, AbstractState* parentState)
{
    qDebug() <<"decode final";

    QString id = node.attribute("id").value();

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

/*
 * DecoderTest
 */
#include <QFile>
DecoderTest::DecoderTest()
{
    qDebug() <<"DecoderTest";

    QFile file("/home/marcel/Programming/hfsm-exec/state_machine.xml");
    if (!file.open(QFile::ReadOnly | QFile::Text))
    {
        qWarning() <<"couldn't open file";

        return;
    }

    QTextStream stream(&file);
    QString data = stream.readAll();

    XmlDecoder f;
    f.decode(data);
}
