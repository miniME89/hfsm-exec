#ifndef DECODER_IMPL_H
#define DECODER_IMPL_H

#include <decoder.h>
#include <QDomDocument>

namespace hfsmexec
{
    class XmlDecoderBuilder : public AbstractDecoderBuilder
    {
        public:
            XmlDecoderBuilder();

            StateMachine* decode(const QString &data);

        private:
            bool decodeChilds(QDomElement& node, StateMachineBuilder& builder, AbstractState* parentState);
            bool decodeTransitions(QDomElement& node, StateMachineBuilder& builder, AbstractState* sourceState);
            bool decodeStateMachine(QDomElement& node, StateMachineBuilder& builder);
            bool decodeComposite(QDomElement& node, StateMachineBuilder& builder, AbstractState* parentState);
            bool decodeParallel(QDomElement& node, StateMachineBuilder& builder, AbstractState* parentState);
            bool decodeInvoke(QDomElement& node, StateMachineBuilder& builder, AbstractState* parentState);
            bool decodeFinal(QDomElement& node, StateMachineBuilder& builder, AbstractState* parentState);
    };

    class JsonDecoderBuilder : public AbstractDecoderBuilder
    {
        public:
            JsonDecoderBuilder();

            StateMachine* decode(const QString &data);

        private:

    };

    class YamlDecoderBuilder : public AbstractDecoderBuilder
    {
        public:
            YamlDecoderBuilder();

            StateMachine* decode(const QString &data);

        private:

    };

    class DecoderFactoryTest
    {
        public:
            DecoderFactoryTest();
    };
}

#endif
