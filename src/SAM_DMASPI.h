#ifndef SAM_DMASPI_H
#define SAM_DMASPI_H
#include "SPIMemory.h"
class DMASAM {
public:
  DMASAM(void);
  void     SPIDmaRX(uint8_t* dst, uint16_t count);
  void     SPIDmaRX(char* dst, uint16_t count);
  void     SPIDmaTX(const uint8_t* src, uint16_t count);
  void     SPIDmaCharTX(const char* src, uint16_t count);
  void     SPIBegin(void);
  void     SPIInit(uint8_t dueSckDivisor);
  uint8_t  SPITransfer(uint8_t b);
  uint8_t  SPIRecByte(void);
  uint8_t  SPIRecByte(uint8_t* buf, size_t len);
  int8_t   SPIRecChar(void);
  int8_t   SPIRecChar(char* buf, size_t len);
  void     SPISendByte(uint8_t b);
  void     SPISendByte(const uint8_t* buf, size_t len);
  void     SPISendChar(char b);
  void     SPISendChar(const char* buf, size_t len);
private:
  void     _dmac_disable(void);
  void     _dmac_enable(void);
  void     _dmac_channel_disable(uint32_t ul_num);
  void     _dmac_channel_enable(uint32_t ul_num);
  bool     _dmac_channel_transfer_done(uint32_t ul_num);
};
#endif
