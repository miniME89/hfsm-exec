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

            AbstractDecoderBuilder* getDecoderBuilder(const QString& encoding);
            void addDecoderBuilder(AbstractDecoderBuilder* builder);
            void removeDecoderBuilder(AbstractDecoderBuilder* builder);

        private:
            QMap<QString, AbstractDecoderBuilder*> builders;

            DecoderBuilderProvider();
    };
}

#endif
