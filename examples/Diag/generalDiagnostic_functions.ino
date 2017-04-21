
//************************************************************************************************//
//                                                                                                //
//                                    Non-board specific code                                     //
//                                                                                                //
//************************************************************************************************//

bool prevWritten() {
  uint8_t _state = flash.readByte(0x00);
  if (_state != 0xFF) {
    return true;
  }
  else {
    return false;
  }
}

void setWrittenStatus(void) {
  uint8_t _data;
  _data = PASS;
#if defined (__AVR_ATtiny85__)
  _data |= ATTINY85;
#endif
  addr = flash.getAddress(sizeof(_data));
  flash.writeByte(addr, _data);
}
