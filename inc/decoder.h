#ifndef DECODER_H
#define DECODER_H

#include <QString>
#include <statemachine.h>
#include <statemachine_impl.h>

namespace hfsmexec
{
    class AbstractDecoderBuilder
    {
        public:
            AbstractDecoderBuilder(const QString& encoding);

            const QString& getEncoding() const;

            virtual StateMachine* decode(const QString& data) = 0;

        private:
            QString encoding;
    };

    class DecoderBuilderProvider
    {
        public:
            static DecoderBuilderProvider* instance;

            static DecoderBuilderProvider* getInstance();

            AbstractDecoderBuilder* getDecoderFactory(const QString& encoding);
            void addDecoderFactory(AbstractDecoderBuilder* factory);
            void removeDecoderFactory(AbstractDecoderBuilder* factory);

        private:
            QList<AbstractDecoderBuilder*> factories;

            DecoderBuilderProvider();
    };
}

#endif
