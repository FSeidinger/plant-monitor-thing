#include <ESP8266WiFi.h>
#include <ArduinoJson.h>
#include <FS.h>
#include "CommandParser.h"

Infrastructure::CommandParser commandParser(Serial);

String ssid = String();
String password = String();
bool isFSInitalized = false;

void setup() {
    Serial.begin(115200);

    Serial.println();
    Serial.println();
    Serial.println("Booting");

    commandParser.addCommandDefinition(String("connect"), connectCommandHandler);
    commandParser.addCommandDefinition(String("disconnect"), disconnectCommandHandler);
    commandParser.addCommandDefinition(String("format"), formatCommandHandler);
    commandParser.addCommandDefinition(String("status"), statusCommandHandler);
    commandParser.addCommandDefinition(String("ssid"), ssidCommandHandler);
    commandParser.addCommandDefinition(String("password"), passwordCommandHandler);
    Serial.println("Parser initialised");


    isFSInitalized = SPIFFS.begin();
    Serial.println("Ready");
}

void loop() {
    commandParser.process();
    yield();
}

void ssidCommandHandler(Stream & stream) {
    char ssidBuffer[64];
    
    parseString(stream, ssidBuffer, sizeof(ssidBuffer));

    if (strlen(ssidBuffer) > 0) {
        stream.println("SSID configured");
        ssid = String(ssidBuffer);
    } else {
        stream.println("SSId must not be empty");
    }
}

void passwordCommandHandler(Stream & stream) {
    char passwordBuffer[64];

    parseString(stream, passwordBuffer, sizeof(passwordBuffer));

    if (strlen(passwordBuffer) > 0) {
        stream.println("Password configured");
        password = String(passwordBuffer);
    } else {
        stream.println("Password must not be empty");
    }
}

void connectCommandHandler(Stream & stream) {
	if ((ssid.length() > 0) && (password.length() > 0)) {
        disconnectCommandHandler(stream);
		connectWiFi(stream, ssid, password);
	}
	else {
		stream.println("For a connection SSID and password must be configured");
	}
}

void disconnectCommandHandler(Stream & stream) {
    if (WiFi.status() == WL_CONNECTED) {
        WiFi.disconnect(true);
        stream.println("Disconnected");
    }
}

void formatCommandHandler(Stream & stream) {
    if (SPIFFS.format()) {
        stream.println("File system formatted");

        if (isFSInitalized == false) {
            isFSInitalized = SPIFFS.begin();
        }
    } else {
        stream.println("Formatting file system failed");
    }
}

void statusCommandHandler(Stream & stream) {
    if (WiFi.status() == WL_CONNECTED) {
        stream.printf("Connected to '%s' using ip address '%s'\n", WiFi.SSID().c_str(), WiFi.localIP().toString().c_str());
    } else {
        stream.println("Disconnected");
    }

    if (isFSInitalized) {
        stream.println("File system is formatted");
    } else {
        stream.println("File system is not formatted");
    }

    stream.printf("SSID configured to '%s'\n", ssid.c_str());

    if (password.length() > 0) {
        stream.println("Password is configured");
    } else {
        stream.println("Password is not configured");
    }

    stream.printf("Sesor reading is %d\n", getSensorReading());
}

int getSensorReading() {
    return analogRead(A0);
}

void connectWiFi(Stream & stream, String ssid, String password)
{
    stream.println("Connecting to " + quoteString(ssid) + String(" using mac address ") + WiFi.macAddress());

	WiFi.mode(WIFI_STA);
	WiFi.begin(ssid.c_str(), password.c_str());

	for (int i = 1; (i < 60 && WiFi.status() != WL_CONNECTED); i++) {
		delay(1000);
	}

	if (WiFi.status() == WL_CONNECTED) {
        stream.println(String("IP address is ") + String(WiFi.localIP().toString()));
        stream.println(String("Connection to ") + quoteString(ssid) + String(" was successful."));
    } else {
        stream.println(String("Connection to ") + quoteString(ssid) + String(" failed"));
    }
}

String quoteString(String string) {
	return String("\"") + string + String("\"");
}

void parseString(Stream & stream, char * buffer, size_t size) {
    int index = 0;
    bool stringStartFound = false;
    bool stringEndFound = false;

    while (!stringStartFound) {
        int nextChar = stream.read();

        switch (nextChar) {
            case '"':
                stringStartFound = true;
                break;

            default:
                yield();
                break;
        }
    }

    while (!stringEndFound) {
        int nextChar = stream.read();

        switch (nextChar) {
            case -1:
                yield();
                break;

            case '"':
                stringEndFound = true;
                break;

            default:
                if (index < size - 1) {
                    buffer[index] = nextChar;
                    buffer[index + 1] = 0;
                    index++;
                }
                break;
        }
    }
}