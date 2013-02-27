//NOTE! Please copy the files templ.htm and templrow.htm to the base dir of your SD Card to make this sketch work.
//These file are located in the directory where this example is stored.

#include <SPI.h>
#include <EthernetUdp.h>
#include <Dns.h>
#include <EthernetServer.h>
#include <Dhcp.h>
#include <Ethernet.h>
#include <EthernetClient.h>
#include <util.h>
#include <SD.h>
#include <WebServer.h>
#include <TemplateParser.h>

static const char progTemplateString1[] PROGMEM = "##TITLE##";
static const char progTemplateString2[] PROGMEM = "##HEAD##";
static const char progTemplateString3[] PROGMEM = "##ROW##";

static const char progTemplateRowString1[] PROGMEM = "##ROWNAME##";
static const char progTemplateRowString2[] PROGMEM = "##ROWVALUE##";

static const char* const templateStrings[] PROGMEM =
{ progTemplateString1, progTemplateString2, progTemplateString3};

static const char* const templateRowStrings[] PROGMEM =
{ progTemplateRowString1, progTemplateRowString2};

byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
IPAddress ip(192,168,1,222);

TemplateParser parser;
WebServer webServer("", 80);

void defaultCmd(WebServer &server, WebServer::ConnectionType type, char *, bool)
{
  File templateFile, templateRowFile;
  char* replacementStrings[2];
  char* title = "ASCII Table";
  char* heading = "ASCII Table";
  char rowName[4];
  char rowValue[2];
  int16_t matchIdx = 0;
  uint8_t i;
  
  if (type != WebServer::HEAD)
  {
    server.httpSuccess();
   
    templateFile = SD.open("templ.htm", FILE_READ);
    templateRowFile = SD.open("templrow.htm", FILE_READ);
    
    while ((matchIdx = parser.processTemplateUntilNextMatch(&templateFile, templateStrings, 3, &server)) >= 0)
    {
      switch (matchIdx)
      {
        case 0:
          server.print(title);
          break;
        case 1:
          server.print(heading);
          break;
        case 2:
          for (i=0; i < 128; i++)
          {
            itoa(i,rowName, 10);
            rowValue[0] = i;
            rowValue[1] = 0;
            replacementStrings[0] = rowName;
            replacementStrings[1] = rowValue;
            parser.processSingleTemplate(&templateRowFile, templateRowStrings, replacementStrings, 2, &server);
          }
          break;
        default:
        server.print("Something unexpected happened");
        break;
      }
    }
    templateFile.close();
    templateRowFile.close();
  }
}

void setup()
{
// Deselect SD device!
  pinMode(4, OUTPUT);
  digitalWrite(4, HIGH);

// Deselect Wiznet device on Ethernet Shield! 
  pinMode(10, OUTPUT);
  digitalWrite(10, HIGH);  
  
//Enable SD  
  SD.begin(4);
  
//Enable Ethernet  
  Ethernet.begin(mac, ip);

//Start Webserver  
  webServer.setDefaultCommand(&defaultCmd);
  webServer.begin();  
}

void loop()
{
// Handle pending connections
  webServer.processConnection();
}
