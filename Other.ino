void EnableSPI_RFID() {
  digitalWrite( ethSS, HIGH );
  digitalWrite( rfidSS, LOW );
}

void EnableSPI_Ethernet() {
  digitalWrite( ethSS, LOW );
  digitalWrite( rfidSS, HIGH );
}

void clearCard() {
  shutterLocked = true;
  previous_card_number = F( "" );
  previous_card_valid = false;
}
