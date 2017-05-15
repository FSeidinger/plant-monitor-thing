#pragma once

#include <String.h>
#include <Stream.h>

namespace Infrastructure {
    typedef void(*CommandHandlerFunction)(Stream &);

    class CommandDefinition {
        public:
        CommandDefinition();
        CommandDefinition(String & command, CommandHandlerFunction & commandHandler);

            String getCommand();
            void apply(Stream & stream);
            bool isUndefined();

        private:
            bool undefined;
            String command;
            CommandHandlerFunction commandHandler;

    };

    /*
        A command parser parsing a command token from an input stream
    */
    class CommandParser	{
        public:
            /*
                Creates a new command parser taking an input stream referenz
            */
            CommandParser(Stream & stream);

            void addCommandDefinition(String command, CommandHandlerFunction commandHandler);

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
                foundCommand,
                matchedCommand
            } parserState = reset;

            char command[32];
            int commandIndex = 0;

            int commandCount = 0;
            CommandDefinition commandDefinitions[8];
            CommandDefinition unknwonDefinition = CommandDefinition();

            // The input and output stream used to parse command tokens
            Stream & parserStream;

            void skipWhiteSpace();
            bool isWhiteSpace();

            CommandDefinition findCommandDefinition();
    };
};
