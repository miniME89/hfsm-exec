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
#include <logger.h>

#include <easylogging++.h>

using namespace hfsmexec;

/*
 * AbstractDecoder
 */
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
        CLOG(WARNING, LOG_DECODER) <<"couldn't decode data: no decoder for data encoding " <<encoding <<" is available";

        return NULL;
    }

    StateMachine* stateMachine = decoder->decode(data);
    if (stateMachine == NULL)
    {
        CLOG(WARNING, LOG_DECODER) <<"couldn't decode data: decoding failed";

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
    CLOG(INFO, LOG_DECODER) <<"add decoder for " <<decoder->getEncoding() <<" encoding";

    decoders[decoder->getEncoding()] = decoder;
}

void DecoderProvider::removeDecoder(AbstractDecoder* decoder)
{
    CLOG(INFO, LOG_DECODER) <<"remove decoder for " <<decoder->getEncoding() <<" encoding";

    //TODO
}
