#include <decoder_impl.h>

#include <QDebug>

using namespace hfsmexec;

/*
 * XmlDecoderFactory
 */
XmlDecoderBuilder::XmlDecoderBuilder() :
    AbstractDecoderBuilder("XML")
{

}

StateMachine* XmlDecoderBuilder::decode(const QString& data)
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

bool XmlDecoderBuilder::decodeChilds(QDomElement& node, StateMachineBuilder& builder, AbstractState* parentState)
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

bool XmlDecoderBuilder::decodeTransitions(QDomElement& node, StateMachineBuilder& builder, AbstractState* sourceState)
{
    qDebug() <<"decode transitions";

    QDomElement transitionElement = node.firstChildElement("transition");
    while (!transitionElement.isNull())
    {
        QString target = transitionElement.attribute("target");

        StringTransition* transition = new StringTransition("TODO", sourceState->getId(), target, "TODO");
        builder <<transition;

        transitionElement = transitionElement.nextSiblingElement("transition");
    }

    return true;
}

bool XmlDecoderBuilder::decodeStateMachine(QDomElement& node, StateMachineBuilder& builder)
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

bool XmlDecoderBuilder::decodeComposite(QDomElement& node, StateMachineBuilder& builder, AbstractState* parentState)
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

bool XmlDecoderBuilder::decodeParallel(QDomElement& node, StateMachineBuilder& builder, AbstractState* parentState)
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

bool XmlDecoderBuilder::decodeInvoke(QDomElement& node, StateMachineBuilder& builder, AbstractState* parentState)
{
    qDebug() <<"decode invoke";

    //TODO

    return true;
}

bool XmlDecoderBuilder::decodeFinal(QDomElement& node, StateMachineBuilder& builder, AbstractState* parentState)
{
    qDebug() <<"decode final";

    QString id = node.attribute("id");

    FinalState* composite = new FinalState(id, parentState->getId());
    builder <<composite;

    return true;
}

/*
 * XmlDecoderFactoryTest
 */
#include <QFile>
DecoderFactoryTest::DecoderFactoryTest()
{
    qDebug() <<"XmlDecoderFactoryTest";

    QFile file("/home/marcel/Programming/hfsm-exec/state_machine.xml");
    if (!file.open(QFile::ReadOnly | QFile::Text))
    {
        qWarning() <<"couldn't open file";
    }

    QTextStream stream(&file);
    QString data = stream.readAll();

    XmlDecoderBuilder f;
    f.decode(data);
}

/*
 * JsonDecoderBuilder
 */
JsonDecoderBuilder::JsonDecoderBuilder() :
    AbstractDecoderBuilder("JSON")
{

}

StateMachine* JsonDecoderBuilder::decode(const QString& data)
{

}

/*
 * YamlDecoderBuilder
 */
YamlDecoderBuilder::YamlDecoderBuilder() :
    AbstractDecoderBuilder("YAML")
{

}

StateMachine *YamlDecoderBuilder::decode(const QString &data)
{

}
