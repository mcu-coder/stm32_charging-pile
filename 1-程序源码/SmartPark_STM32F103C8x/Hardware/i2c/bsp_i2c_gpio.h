#ifndef _BSP_I2C_GPIO_H
#define _BSP_I2C_GPIO_H

#include <inttypes.h>

#define SOFT_I2C_WR	0		/* 写控制bit */
#define SOFT_I2C_RD	1		/* 读控制bit */


/* 定义I2C总线连接的GPIO端口, 用户只需要修改下面4行代码即可任意改变SCL和SDA的引脚 */
#define SOFT_I2C_SCL_PORT	    GPIOB
#define SOFT_I2C_SCL_PIN		GPIO_PIN_10			/* 连接到SCL时钟线的GPIO */
#define SOFT_I2C_SDA_PORT	    GPIOB
#define SOFT_I2C_SDA_PIN		GPIO_PIN_11			/* 连接到SDA数据线的GPIO */

#define SOFT_I2C_SCL_1()  HAL_GPIO_WritePin(SOFT_I2C_SCL_PORT, SOFT_I2C_SCL_PIN, GPIO_PIN_SET)		/* SCL = 1 */
#define SOFT_I2C_SCL_0()  HAL_GPIO_WritePin(SOFT_I2C_SCL_PORT, SOFT_I2C_SCL_PIN, GPIO_PIN_RESET)		/* SCL = 0 */

#define SOFT_I2C_SDA_1()  HAL_GPIO_WritePin(SOFT_I2C_SDA_PORT, SOFT_I2C_SDA_PIN, GPIO_PIN_SET)		/* SDA = 1 */
#define SOFT_I2C_SDA_0()  HAL_GPIO_WritePin(SOFT_I2C_SDA_PORT, SOFT_I2C_SDA_PIN, GPIO_PIN_RESET)		/* SDA = 0 */

#define SOFT_I2C_SDA_READ()  HAL_GPIO_ReadPin(SOFT_I2C_SDA_PORT, SOFT_I2C_SDA_PIN)	/* 读SDA口线状态 */

void i2c_Start(void);
void i2c_Stop(void);
void i2c_SendByte(uint8_t _ucByte);
uint8_t i2c_ReadByte(void);
uint8_t i2c_WaitAck(void);
void i2c_Ack(void);
void i2c_NAck(void);
uint8_t i2c_CheckDevice(uint8_t _Address);

uint8_t i2c_transmit(uint8_t DevAddress, uint8_t *pData, uint16_t Size, uint32_t Timeout);
uint8_t i2c_receive(uint16_t DevAddress, uint8_t *pData, uint16_t Size, uint32_t Timeout);

#endif

