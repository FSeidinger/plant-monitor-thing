#include "CommandParser.h"

using namespace Infrastructure;

CommandDefinition::CommandDefinition() {
    undefined = true;
}

CommandDefinition::CommandDefinition(String & commandRef, CommandHandlerFunction & commandHandlerRef) {
    command = commandRef;
    commandHandler = commandHandlerRef;
    undefined = false;
}

String CommandDefinition::getCommand() {
    return command;
}

bool CommandDefinition::isUndefined() {
    return undefined;
}

void CommandDefinition::apply(Stream & stream) {
    commandHandler(stream);
}

CommandParser::CommandParser(Stream & stream) : parserStream(stream) {
}

void CommandParser::addCommandDefinition(String command, CommandHandlerFunction commandHandler) {
    commandDefinitions[commandCount] = CommandDefinition(command, commandHandler);
    commandCount++;
}

void CommandParser::process() {
    while (parserStream.available() > 0) {
        switch (parserState) {
            case reset: {
                    skipWhiteSpace();

                    if (parserStream.available() > 0) {
                        commandIndex = 0;
                        parserState = skippedWhiteSpace;
                    }

                }
                break;

            case skippedWhiteSpace: {
                    if (parserStream.available() > 0) {
                        parserState = addedCharacter;
                    }
                }
                break;

            case addedCharacter: {
                    if (parserStream.available() > 0) {
                        if (!isWhiteSpace()) {
                            char nextChar = parserStream.read();

                            int lastIndex = sizeof(command) - 1;
                            if (commandIndex < lastIndex) {
                                command[commandIndex] = nextChar;
                                command[commandIndex + 1] = 0;
                                commandIndex++;
                            }
                        } else {
                            parserState = foundCommand;
                        }
                    }
                }
                break;

            case foundCommand: {
                    CommandDefinition definition = findCommandDefinition();

                    if (!definition.isUndefined()) {
                        parserState = matchedCommand;
                    } else {
                        parserStream.printf("Unknown command '%s'\n", command);
                        parserState = reset;
                    }
                }
                break;

            case matchedCommand: {
                    CommandDefinition definition = findCommandDefinition();
                    definition.apply(parserStream);
                    parserState = reset;
                }
                break;
        }
    }
}

void CommandParser::skipWhiteSpace() {
    while (isWhiteSpace()) {
        parserStream.read();
    }
}

bool CommandParser::isWhiteSpace() {
    if (parserStream.available() > 0) {
        char whiteSpaceCharacters[] = "\n\t ";
        char nextChar = parserStream.peek();

        for (int whiteSpaceCharIndex = 0; whiteSpaceCharIndex < 3; whiteSpaceCharIndex++) {
            if (nextChar == whiteSpaceCharacters[whiteSpaceCharIndex]) {
                return true;
            }
        }
    }

    return false;
}

CommandDefinition CommandParser::findCommandDefinition() {
    for (int i = 0; i < commandCount; i++) {
        CommandDefinition definition = commandDefinitions[i];

        if (definition.getCommand().compareTo(String(command)) == 0) {
            return definition;
        }
    }

    return unknwonDefinition;
}
