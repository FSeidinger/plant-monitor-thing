#include <ArduinoJson.h>

typedef enum CommandStatus {
	CommandStatus_CommandInitalized,
	CommandStatus_CommandStarted,
	CommandStatus_CommandFinished
};

String command;
CommandStatus commandStatus;

void setup() {
	command = "";
	commandStatus = CommandStatus_CommandInitalized;

	Serial.begin(115200);
}

void loop() {
	while (Serial.available() > 0) {
		if (isCommandRead(Serial)) {
			parseCommand(command, Serial);
			commandStatus = CommandStatus_CommandInitalized;
			command = "";
		}
	}

	int plantSensorReading = analogRead(A0);
	Serial.println(String("Sesor reading: ") + String(plantSensorReading));

	delay(1000);
}

boolean isCommandRead(Stream & stream) {
	switch (commandStatus) {
		case CommandStatus_CommandInitalized:
			// discard whitespace, if any
			if (isWhiteSpace(stream.peek())) {
				stream.read();
			}
			else {
				commandStatus = CommandStatus_CommandStarted;
			}
			break;

		case CommandStatus_CommandStarted:
			if (isWhiteSpace(stream.peek())) {
				commandStatus = CommandStatus_CommandFinished;
			}
			break;
	}

	return commandStatus == CommandStatus_CommandFinished;
}

boolean isWhiteSpace(char c) {
	if (c == ' ' || c == '\n') {
		return true;
	}
	else {
		return false;
	}
}

void parseCommand(String command, Stream & stream) {
	if (command.compareTo("reset") == 0) {
		stream.println("Plant guard reset");
	}
	else if (command.compareTo("configure") == 0) {
		parseConfiguration(stream);
	}
	else if (command.compareTo("save") == 0) {
		stream.println("Plant guard configuration saved");
	}
	else {
		stream.println(String("Received unknown command \"") + String(command) + String("\""));
	}
}

void parseConfiguration(Stream & stream) {
	StaticJsonBuffer<512> jsonBuffer;

	JsonObject& root = jsonBuffer.parse(stream);

	String ssid = root["ssid"];
	String password = root["password"];

	if ((ssid.length() > 0) && (password.length() > 0)) {
		stream.println(String("Plant guard configured with ssid ") + String(ssid) + String(" and password ") + String(password));
	}
	else {
		stream.println("SSID or password for Plant guard must not be empty");
		stream.println("");
		stream.println("Syntax:");
		stream.println("");
		stream.println("configure {");
		stream.println("  \"ssid\":\"network ssid\",");
		stream.println("  \"password\":\"network password\"");
		stream.println("}");
	}
}
