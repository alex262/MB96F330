#ifndef	_H_I2C
#define _H_I2C

/** E X T E R N A L   V A R I A B L E S ***************************/
void SetWorkChI2C(BYTE ch);

void HighDensByteWrite(WORD address, BYTE data);
void HighDensPageWrite(WORD address, BYTE *data, WORD numbytes);
void HighDensByteRead(WORD address, BYTE *data);
void HighDensSequentialRead(WORD address, BYTE *data, WORD numbytes);

//void I2CBus_write(BYTE* txBytes, BYTE len);
//void I2CBus_read(BYTE* rxBytes, BYTE len);

void I2C_Init(BYTE ch);
void I2C_Acknowlegde(void);
void I2C_Start(BYTE slave_address);
void I2C_Continue(BYTE slave_address);
void I2C_Stop(void);
void I2C_Write(BYTE value);
BYTE I2C_Read(void);
BYTE I2C_LastRead(void);

#endif