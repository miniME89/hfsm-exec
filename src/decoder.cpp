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
