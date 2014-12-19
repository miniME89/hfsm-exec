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
 * AbstractDecoder
 */
const Logger* AbstractDecoder::logger = Logger::getLogger(LOGGER_DECODER);

AbstractDecoder::AbstractDecoder(const QString &encoding) :
    encoding(encoding)
{

}

AbstractDecoder::~AbstractDecoder()
{

}

const QString& AbstractDecoder::getEncoding() const
{
    return encoding;
}

/*
 * DecoderProvider
 */
const Logger* DecoderProvider::logger = Logger::getLogger(LOGGER_DECODER);

DecoderProvider::DecoderProvider()
{

}

DecoderProvider::~DecoderProvider()
{

}

StateMachine* DecoderProvider::decode(const QString& encoding, const QString& data)
{
    AbstractDecoder* decoder = getDecoder(encoding);
    if (decoder == NULL)
    {
        logger->warning(QString("couldn't decode data: no decoder for data encoding \"%1\" is available").arg(encoding));

        return NULL;
    }

    StateMachine* stateMachine = decoder->decode(data);
    if (stateMachine == NULL)
    {
        logger->warning("couldn't decode data: decoding failed");

        return NULL;
    }

    return stateMachine;
}

AbstractDecoder* DecoderProvider::getDecoder(const QString& encoding)
{
    if (!decoders.contains(encoding))
    {
        return NULL;
    }

    return decoders[encoding];
}

void DecoderProvider::addDecoder(AbstractDecoder* decoder)
{
    logger->info(QString("add decoder for \"%1\" encoding").arg(decoder->getEncoding()));

    decoders[decoder->getEncoding()] = decoder;
}

void DecoderProvider::removeDecoder(AbstractDecoder* decoder)
{
    logger->info(QString("remove decoder for \"%1\" encoding").arg(decoder->getEncoding()));

    //TODO
}
