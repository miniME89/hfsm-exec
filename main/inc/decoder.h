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

#ifndef DECODER_H
#define DECODER_H

#include <QString>
#include <statemachine.h>
#include <statemachine_impl.h>

namespace hfsmexec
{
    class AbstractDecoder
    {
        public:
            AbstractDecoder(const QString& encoding);
            virtual ~AbstractDecoder();

            const QString& getEncoding() const;

            virtual StateMachine* decode(const QString& data) = 0;

        private:
            QString encoding;
    };

    class DecoderProvider
    {
        public:
            DecoderProvider();
            ~DecoderProvider();

            AbstractDecoder* getDecoder(const QString& encoding);
            void addDecoder(AbstractDecoder* decoder);
            void removeDecoder(AbstractDecoder* decoder);

        private:
            QMap<QString, AbstractDecoder*> decoders;
    };
}

#endif
