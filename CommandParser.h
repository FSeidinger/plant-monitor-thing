#pragma once

#include <Stream.h>

namespace Infrastructure {
    /*
        A command parser parsing a command token from an input stream
    */
    class CommandParser	{
        public:
            /*
                Creates a new command parser taking an input stream referenz
            */
            CommandParser(Stream & stream);

            /*
                The "loop" of the command parser.

                Consumes characters from the input stream until a command token is found
                or the character size limit is hit
            */
            void process();

        private:
            enum ParserState {
                reset,
                skippedWhiteSpace,
                addedCharacter,
                matchedCommand
            } parserState = reset;

            char command[32];
            int commandIndex = 0;

            // The input and output stream used to parse command tokens
            Stream & parserStream;

            void skipWhiteSpace();
            bool isWhiteSpace();
    };
};
