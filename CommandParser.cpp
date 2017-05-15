#include "CommandParser.h"

using namespace Infrastructure;

CommandParser::CommandParser(Stream & stream) : parserStream(stream) {
    parserStream.println("Created command parser");
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
                            parserStream.printf("Found command '%s'\n", command);
                            parserState = matchedCommand;
                        }
                        
                    }
                }
                break;

            case matchedCommand: {
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

