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

#ifndef DECODER_IMPL_H
#define DECODER_IMPL_H

#include <decoder.h>
#include <pugixml.hpp>

namespace hfsmexec
{
    class XmlDecoder : public AbstractDecoder
    {
        public:
            XmlDecoder();

            StateMachine* decode(const QString &data);

        private:
            bool decodeChilds(pugi::xml_node& node, StateMachineBuilder& builder, AbstractState* parentState);
            bool decodeTransitions(pugi::xml_node& node, StateMachineBuilder& builder, AbstractState* sourceState);
            bool decodeStateMachine(pugi::xml_node& node, StateMachineBuilder& builder);
            bool decodeComposite(pugi::xml_node& node, StateMachineBuilder& builder, AbstractState* parentState);
            bool decodeParallel(pugi::xml_node& node, StateMachineBuilder& builder, AbstractState* parentState);
            bool decodeInvoke(pugi::xml_node& node, StateMachineBuilder& builder, AbstractState* parentState);
            bool decodeFinal(pugi::xml_node& node, StateMachineBuilder& builder, AbstractState* parentState);
    };

    class JsonDecoder : public AbstractDecoder
    {
        public:
            JsonDecoder();

            StateMachine* decode(const QString &data);

        private:

    };

    class YamlDecoder : public AbstractDecoder
    {
        public:
            YamlDecoder();

            StateMachine* decode(const QString &data);

        private:

    };

    class DecoderTest
    {
        public:
            DecoderTest();
    };
}

#endif
