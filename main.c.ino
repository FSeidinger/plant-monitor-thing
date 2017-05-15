#include <ESP8266WiFi.h>
#include <ArduinoJson.h>
#include "CommandParser.h"

Infrastructure::CommandParser commandParser(Serial);

void setup() {
    Serial.begin(115200);
}

void loop() {
    commandParser.process();

    delay(1000);
}

/*
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
				// command is completely read
				commandStatus = CommandStatus_CommandFinished;
			}
			else {
				if (command.length() < 32) {
					// add next character to command
					command = command + String((char) stream.read());
				}
			}
			break;
	}

	return commandStatus == CommandStatus_CommandFinished;
}
*/

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
		WiFi.disconnect();
		stream.println("Plant Guard reset");
	}
	else if (command.compareTo("configure") == 0) {
		parseConfiguration(stream);
	}
	else if (command.compareTo("save") == 0) {
		stream.println("Plant Guard configuration saved");
	}
	else if (command.compareTo("read") == 0) {
		int plantSensorReading = analogRead(A0);
		Serial.println(String("Sesor reading: ") + String(plantSensorReading));
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
		stream.println("Plant Guard configured");
		connectWiFi(ssid, password);
	}
	else {
		stream.println("SSID or password for Plant Guard must not be empty");
		stream.println("");
		stream.println("Syntax:");
		stream.println("");
		stream.println("configure {");
		stream.println("  \"ssid\":\"network ssid\",");
		stream.println("  \"password\":\"network password\"");
		stream.println("}");
	}
}

void connectWiFi(String ssid, String password)
{
	String macAddress = WiFi.macAddress();
	macAddress.toLowerCase();

	Serial.println();
	Serial.println("Connecting to: " + ssid + String(" using mac address ") + macAddress);

	WiFi.mode(WIFI_STA);
	WiFi.begin(ssid.c_str(), password.c_str());

	Serial.print("Connecting ");

	for (int i = 1; (i < 60 && WiFi.status() != WL_CONNECTED); i++) {
		Serial.print(".");
		delay(1000);
	}

	Serial.println();

	if (WiFi.status() == WL_CONNECTED) {
		Serial.println(String("WiFi connection to ") + quoteString(ssid) + String(" was successful."));
		Serial.println(String("Using ip address ") + String(WiFi.localIP().toString()));
	}
	else {
		Serial.println(String("Connection to ") + quoteString(ssid) + String(" failed"));
	}
}

String quoteString(String string) {
	return String("\"") + string + String("\"");
}
