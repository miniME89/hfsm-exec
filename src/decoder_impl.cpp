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
#include <parameter_container.h>

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
    QString errorMessage;
    int errorLine;
    int errorColumn;
    QDomDocument doc;
    if (!doc.setContent(data, true, &errorMessage, &errorLine, &errorColumn))
    {
        qDebug() <<"couldn't parse XML: " <<errorMessage <<"(Line: " <<errorLine <<", Column: " <<errorColumn <<")";

        return NULL;
    }

    //get root element
    QDomElement node = doc.documentElement();
    if (node.tagName() != "stateMachine")
    {
        qDebug() <<"invalid XML encoding: root element needs to be of type" <<QString("stateMachine");

        return NULL;
    }

    //decode state machine recursively
    StateMachineBuilder builder;
    decodeStateMachine(node, builder);

    return builder.build();
}

bool XmlDecoder::decodeChilds(QDomElement& node, StateMachineBuilder& builder, AbstractState* parentState)
{
    qDebug() <<"decode childs";

    QDomElement child = node.firstChild().toElement();
    while (!child.isNull())
    {
        QString tagName = child.tagName();
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

        child = child.nextSibling().toElement();
    }

    return true;
}

bool XmlDecoder::decodeTransitions(QDomElement& node, StateMachineBuilder& builder, AbstractState* sourceState)
{
    qDebug() <<"decode transitions";

    QDomElement transitionElement = node.firstChildElement("transition");
    while (!transitionElement.isNull())
    {
        QString target = transitionElement.attribute("target");

        NamedTransition* transition = new NamedTransition("TODO", sourceState->getId(), target, "TODO");
        builder <<transition;

        transitionElement = transitionElement.nextSiblingElement("transition");
    }

    return true;
}

bool XmlDecoder::decodeStateMachine(QDomElement& node, StateMachineBuilder& builder)
{
    qDebug() <<"decode stateMachine";

    QString initial = node.attribute("initial");
    QDomElement childs = node.firstChildElement("childs");

    StateMachine* stateMachine = new StateMachine(initial);
    builder <<stateMachine;

    if (!decodeChilds(childs, builder, stateMachine))
    {
        return false;
    }

    return true;
}

bool XmlDecoder::decodeComposite(QDomElement& node, StateMachineBuilder& builder, AbstractState* parentState)
{
    qDebug() <<"decode composite";

    QString id = node.attribute("id");
    QString initial = node.attribute("initial");
    QDomElement transitions = node.firstChildElement("transitions");
    QDomElement childs = node.firstChildElement("childs");

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

bool XmlDecoder::decodeParallel(QDomElement& node, StateMachineBuilder& builder, AbstractState* parentState)
{
    qDebug() <<"decode parallel";

    QString id = node.attribute("id");
    QString initial = node.attribute("initial");
    QDomElement transitions = node.firstChildElement("transitions");
    QDomElement childs = node.firstChildElement("childs");

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

bool XmlDecoder::decodeInvoke(QDomElement& node, StateMachineBuilder& builder, AbstractState* parentState)
{
    qDebug() <<"decode invoke";

    QString id = node.attribute("id");
    QDomElement endpoint = node.firstChildElement("endpoint");

    QString endPointStr;
    QTextStream endpointStream(&endPointStr);
    endpoint.save(endpointStream, 4);
    qDebug() <<endPointStr;

    ParameterContainer endpointParameter;
    endpointParameter.fromXml("/", endPointStr);
    QString test;
    qDebug() <<endpointParameter.toJson("/", test);
    qDebug() <<test;

    InvokeState* invoke = new InvokeState(id, parentState->getId());
    builder <<invoke;

    return true;
}

bool XmlDecoder::decodeFinal(QDomElement& node, StateMachineBuilder& builder, AbstractState* parentState)
{
    qDebug() <<"decode final";

    QString id = node.attribute("id");

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
