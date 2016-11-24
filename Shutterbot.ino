/*
  ShutterBot
  For South London Makerspace
  
  To read cards serial numbers 
  Query remote server for access rights.
  Open close shutter.
*/

#include <SPI.h>
#include <Ethernet.h>
#include <Wire.h>
#include <inttypes.h>
#include <MFRC522.h>
#include <Metro.h>
#include <utility/w5100.h>

// Pin Mapping
// Ethernet shield attached to pins 10, 11, 12, 13
// SD card also uses pin 4
#define ethSS    10

#define upBtn    A4
#define dwnBtn   A3
#define inBtn    A1
#define pwrRelay 6
#define dirRelay 7
#define doorMon  A2

#define rfidRST  8
#define rfidSS   9
#define rfidMOSI 11
#define rfidMISO 12
#define rfidSCK  13

#define UP_DIR -1
#define STOPPED 0
#define DOWN_DIR 1

#define deviceID F( "shutter" )

// Ethernet configuration
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xEE };

// numeric IP for server node
IPAddress nodebotServer( 10, 0, 1, 2 );
uint16_t port = 1234;

// Initialize the Ethernet client library
EthernetClient client;
EthernetServer server = EthernetServer( 1234 );

// Initialsie RFID reader
MFRC522 mfrc522( rfidSS, rfidRST );	

boolean shutterLocked = true;       // Stores state sent to relay
String previous_card_number;        // Stores previously read card number for a time
boolean previous_card_valid;        // previously red card state valid true or false
int previous_direction = 0;

int attempts = 0;

Metro rfidReadTimer = Metro( 100 ); 
Metro cardTimer = Metro( 5000 );
Metro directionTimer = Metro( 1000 );
Metro netTimeout = Metro( 1000 );

char* masterCards[] = { "" };

void setup() {
  pinMode( ethSS, OUTPUT );
  pinMode( rfidSS, OUTPUT );
  pinMode( pwrRelay, OUTPUT );
  pinMode( dirRelay, OUTPUT );
  pinMode( inBtn, INPUT );
  pinMode( upBtn, INPUT );
  pinMode( dwnBtn, INPUT );
  pinMode( doorMon, INPUT );
  
  // Set default relay state  
  digitalWrite( pwrRelay, LOW );
  digitalWrite( dirRelay, LOW );

  Serial.begin( 115200 );
  Serial.println( F( "Shutterbot Started" ) );

  Serial.println( F( "RFID" ) );
  SPI.begin();
  EnableSPI_RFID();
  mfrc522.PCD_Init();
 
  Serial.println( F( "Ethernet" ) );
  EnableSPI_Ethernet();
  Ethernet.begin( mac );
  W5100.setRetransmissionTime( 0x07d0 ); // reduce ethernet connection timeout
  W5100.setRetransmissionCount( 3 );     // set ethernet connection retries
  
  Serial.println( Ethernet.localIP() );
}

void loop() { 
  if ( rfidReadTimer.check() ) rfidRead();
  if ( cardTimer.check() ) clearCard();

  // Move up if:
  // - inside button pressed
  // - up button pressed and shutter unlocked
  if ( digitalRead( inBtn ) || ( ! shutterLocked && digitalRead( upBtn ) && ! digitalRead( dwnBtn ) ) ) {
    up();

  // Move down if:
  // - down button pressed and shutter unlocked, door interlock is closed
  } else if ( ! shutterLocked && ! digitalRead( upBtn ) && digitalRead( dwnBtn ) && digitalRead( doorMon ) ) {
    down();

  // Stop otherwise
  } else {
    stop();
  }

  if ( Ethernet.maintain() > 0 )
    Serial.println( Ethernet.localIP() );
}
