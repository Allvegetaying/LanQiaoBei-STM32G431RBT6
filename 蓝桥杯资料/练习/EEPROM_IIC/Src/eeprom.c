/* e2prom.c */
 
#include "eeprom.h"
 
/* E2PROM写操作，对应Figure 8. Btye Write */
void e2prom_write(unsigned char address, unsigned char info)
{
    I2CStart();              //1.I2C起始信号(START)
    I2CSendByte(0xa0);       //2.发送设备地址与“写”信号(DEVICE ADDRESS+WRITE)，将在下文解释
    I2CWaitAck();            //3.IC2等待确认信号(ACK)
 
    I2CSendByte(address);    //4.发送数据存储地址(WORD ADDRESS)（可以为0~255，对应256个）
    I2CWaitAck();            //5.I2C等待确认信号(ACK)
    I2CSendByte(info);       //6.发送数据(DATA)
    I2CWaitAck();            //7.I2C等待确认信号(ACK)
    I2CStop();               //8.I2C结束信号(STOP)
}
 
/* E2PROM读操作，对应Figure 11. Random Read */

unsigned char e2prom_read(unsigned char address)
{
    unsigned char val;
 
    I2CStart();              //1.I2C起始信号(START)
    I2CSendByte(0xa0);       //2.发送设备地址与“写”信号(DEVICE ADDRESS+WR-TE)，将在下文解释
    I2CWaitAck();            //3.IC2等待确认信号(ACK)
 
    I2CSendByte(address);    //4.发送数据存储地址(WORD ADDRESS)（可以为0~255，对应256个）
    I2CWaitAck();            //5.I2C等待确认信号(ACK)
    I2CStop();               //下一次发送起始信号前先停止
 
    I2CStart();              //6.I2C起始信号(START)
    I2CSendByte(0xa1);       //7.发送设备地址与“读”信号(DEVICE ADDRESS+READ)，将在下文解释
    I2CWaitAck();            //8.I2C等待确认信号(ACK)
    val = I2CReceiveByte();  //9.接收数据(DATA)
    I2CSendNotAck();         //10.I2C发送非确认信号(NO ACK)
    I2CStop();               //11.I2C结束信号(STOP)
 
    return (val);
}
