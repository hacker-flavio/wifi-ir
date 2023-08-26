#include <SPI.h>
#include <WiFiNINA.h>
#include <IRremote.h>

#define led 9

char ssid[] = "test";
char pass[] = "password";
int receiver = 11; // Signal Pin of IR receiver to Arduino Digital Pin 11

WiFiServer server(80);
WiFiClient client;

/*-----( Declare objects )-----*/
IRrecv irrecv(receiver); // create instance of 'irrecv'
bool ledState;

IPAddress serverIP(10, 34, 71, 191); // Use localhost IP address
int serverPort = 8080;               // Use the port your Node.js server is listening on

/*-----( Function )-----*/
void translateIR() // takes action based on IR code received
// describing Remote IR codes
{

    switch (irrecv.decodedIRData.decodedRawData)
    {
    case 0xBA45FF00:
    {
        Serial.println("POWER");
        // Respond to POWER button
        ledState = !ledState;
        digitalWrite(LED_BUILTIN, ledState);
    }
    break;
    case 0xB847FF00:
        Serial.println("FUNC/STOP");
        break;
    case 0xB946FF00:
        Serial.println("VOL+");
        break;
    case 0xBB44FF00:
        Serial.println("FAST BACK");
        break;
    case 0xBF40FF00:
    {
        Serial.println("PAUSE");
        // Create a new WiFi client instance for sending the POST request
        WiFiClient client;
        // Connect to the server
        if (client.connect(serverIP, serverPort))
        {
            Serial.println("Connected to server for POST request");
            // Create the POST request
            String postData = "{\"buttonPress\":\"pause\"}"; // Customize the JSON data as needed
            client.println("POST /sendData HTTP/1.1");
            client.println("Host: " + String(serverIP));
            client.println("Content-Type: application/json");
            client.print("Content-Length: ");
            client.println(postData.length());
            client.println();
            client.println(postData); // Send the data to the server
            client.println();
            client.flush(); // Ensure all data is sent immediately
            // Close the connection
            client.stop();
            // Serial.println("POST request complete");
        }
        else
        {
            Serial.println("Failed to connect to server");
        }
    };
    break;
    case 0xBC43FF00:
        Serial.println("FAST FORWARD");
        break;
    case 0xF807FF00:
    {
        Serial.println("DOWN");
        // Create a new WiFi client instance for sending the POST request
        WiFiClient client;

        // Connect to the server
        if (client.connect(serverIP, serverPort))
        {
            Serial.println("Connected to server for POST request");

            // Create the POST request
            String postData = "{\"buttonPress\":\"down\"}"; // Customize the JSON data as needed
            client.println("POST /sendData HTTP/1.1");
            client.println("Host: " + String(serverIP));
            client.println("Content-Type: application/json");
            client.print("Content-Length: ");
            client.println(postData.length());
            client.println();
            client.println(postData); // Send the data to the server
            client.println();
            client.flush(); // Ensure all data is sent immediately
            // Close the connection
            client.stop();
        }
        else
        {
            Serial.println("Failed to connect to server");
        }
    };
    break;
    case 0xEA15FF00:
        Serial.println("VOL-");
        break;
    case 0xF609FF00:
    {
        Serial.println("UP");
        // Create a new WiFi client instance for sending the POST request
        WiFiClient client;
        // Connect to the server
        if (client.connect(serverIP, serverPort))
        {
            Serial.println("Connected to server for POST request");
            // Create the POST request
            String postData = "{\"buttonPress\":\"up\"}"; // Customize the JSON data as needed
            client.println("POST /sendData HTTP/1.1");
            client.println("Host: " + String(serverIP));
            client.println("Content-Type: application/json");
            client.print("Content-Length: ");
            client.println(postData.length());
            client.println();
            client.println(postData); // Send the data to the server
            client.println();
            client.flush(); // Ensure all data is sent immediately
            // Close the connection
            client.stop();
            // Serial.println("POST request complete");
        }
        else
        {
            Serial.println("Failed to connect to server");
        }
    };
    break;
    case 0xE619FF00:
        Serial.println("EQ");
        break;
    case 0xF20DFF00:
        Serial.println("ST/REPT");
        break;
    case 0xE916FF00:
        Serial.println("0");
        break;
    case 0xF30CFF00:
        Serial.println("1");
        break;
    case 0xE718FF00:
        Serial.println("2");
        break;
    case 0xA15EFF00:
        Serial.println("3");
        break;
    case 0xF708FF00:
        Serial.println("4");
        break;
    case 0xE31CFF00:
        Serial.println("5");
        break;
    case 0xA55AFF00:
        Serial.println("6");
        break;
    case 0xBD42FF00:
        Serial.println("7");
        break;
    case 0xAD52FF00:
        Serial.println("8");
        break;
    case 0xB54AFF00:
        Serial.println("9");
        break;
    default:
        Serial.println("Other button:");
        break;
    }

    // delay(100);
} // END translateIR

void setup()
{
    pinMode(led, OUTPUT);
    Serial.begin(9600);

    Serial.begin(9600);
    Serial.println("IR Receiver Button Decode");
    irrecv.enableIRIn(); // Start the receiver

    while (WiFi.begin(ssid, pass) != WL_CONNECTED)
    {
        Serial.println("Failed to connect to WiFi");
        delay(1000);
    }

    Serial.println("Connected to WiFi");

    // Set up LED
    ledState = false;
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, ledState);

    server.begin();
    Serial.print("Server started. IP address: ");
    Serial.println(WiFi.localIP());
}

void loop()
{

    if (irrecv.decode()) // have we received an IR signal?
    {
        translateIR();
        irrecv.resume(); // receive the next value
    }

    client = server.available();
    if (client)
    {
        Serial.println("New client connected.");

        // Read the request headers (not needed in this simplified version)
        while (client.connected())
        {
            if (client.available())
            {
                String line = client.readStringUntil('\r');
                if (line == "\n")
                {
                    break; // End of headers
                }
            }
        }

        // Read the request body (data sent from Node.js)
        if (client.available())
        {
            String data = client.readStringUntil('\r');
            Serial.println("Received data from Node.js: " + data);
            delay(1);
            // Process the data received from Node.js as needed
            if (data.indexOf("ledStatus\":\"on") != -1)
            {
                digitalWrite(led, HIGH); // Turn on the LED
                delay(1);
            }
            else if (data.indexOf("ledStatus\":\"off") != -1)
            {
                digitalWrite(led, LOW); // Turn off the LED
                delay(1);
            }

            // Process the data received from Node.js as needed

            // Send a response back to Node.js
            client.println("HTTP/1.1 200 OK");
            client.println("Content-Type: text/plain");
            client.println("Connection: close"); // Close the connection after sending the response
            client.println();                    // Empty line to indicate the end of headers
            client.println("success");           // You can send any data you want in the response
            client.println();                    // Empty line to indicate the end of the response
            client.flush();                      // Ensure all data is sent immediately

            // Close the client connection after sending the response
            delay(1);
            client.stop();
        }
        Serial.println("Client disconnected.");
    }
}
