#include <decoder.h>

using namespace hfsmexec;

/*
 * AbstractDecoderFactory
 */
AbstractDecoderBuilder::AbstractDecoderBuilder(const QString &encoding) :
    encoding(encoding)
{

}

const QString& AbstractDecoderBuilder::getEncoding() const
{
    return encoding;
}

/*
 * DecoderFactoryProvider
 */
DecoderBuilderProvider* DecoderBuilderProvider::instance = NULL;

DecoderBuilderProvider* DecoderBuilderProvider::getInstance()
{
    if (instance == NULL)
    {
        instance = new DecoderBuilderProvider();
    }

    return instance;
}

DecoderBuilderProvider::DecoderBuilderProvider()
{

}

AbstractDecoderBuilder* DecoderBuilderProvider::getDecoderFactory(const QString &encoding)
{
    for (int i = 0; i < factories.size(); i++)
    {
        if (factories[i]->getEncoding() == encoding)
        {
            return factories[i];
        }
    }

    return NULL;
}

void DecoderBuilderProvider::addDecoderFactory(AbstractDecoderBuilder* factory)
{
    factories.append(factory);
}

void DecoderBuilderProvider::removeDecoderFactory(AbstractDecoderBuilder* factory)
{
    //TODO
}
