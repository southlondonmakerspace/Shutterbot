void up() {
  cardTimer.reset();

  if ( previous_direction != UP_DIR && directionTimer.check() == 0 )
    return stop();

  Serial.println( "UP" );

  digitalWrite( dirRelay, LOW);  // Set direction
  delay( 50 );
  digitalWrite( pwrRelay, HIGH );  // Power to shutter
  
  previous_direction = UP_DIR;
  directionTimer.reset();
}

void down() {
  cardTimer.reset();

  if ( previous_direction != DOWN_DIR && directionTimer.check() == 0 )
    return stop();

  Serial.println( "DOWN" );

  digitalWrite( dirRelay, HIGH ); // Set direction
  delay( 50 );
  digitalWrite( pwrRelay, HIGH );  // Power to shutter
  
  previous_direction = DOWN_DIR;
  directionTimer.reset();
}

void stop() {
  Serial.println( "STOP" );
  digitalWrite( dirRelay, LOW); // Set direction
  delay( 50 );
  digitalWrite( pwrRelay, LOW );  // Power to shutter
  previous_direction = STOPPED;
}
