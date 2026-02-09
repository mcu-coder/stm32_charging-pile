#include "bsp_i2c_gpio.h"
#include "main.h"


static void i2c_CfgGpio(void);


/*
*********************************************************************************************************
*	�� �� ��: i2c_Delay
*	����˵��: I2C����λ�ӳ٣����400KHz
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void i2c_Delay(void)
{
	uint8_t i;

	/*��
	 	�����ʱ����ͨ���߼������ǲ��Եõ��ġ�
    ����������CPU��Ƶ72MHz ��MDK���뻷����1���Ż�
  
		ѭ������Ϊ10ʱ��SCLƵ�� = 205KHz 
		ѭ������Ϊ7ʱ��SCLƵ�� = 347KHz�� SCL�ߵ�ƽʱ��1.5us��SCL�͵�ƽʱ��2.87us 
	 	ѭ������Ϊ5ʱ��SCLƵ�� = 421KHz�� SCL�ߵ�ƽʱ��1.25us��SCL�͵�ƽʱ��2.375us 
	*/
	for (i = 0; i < 20; i++);
}

static void i2c_gpio_output(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = SOFT_I2C_SDA_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(SOFT_I2C_SDA_PORT, &GPIO_InitStruct);
}

static void i2c_gpio_input(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = SOFT_I2C_SDA_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(SOFT_I2C_SDA_PORT, &GPIO_InitStruct);
}

/*
*********************************************************************************************************
*	�� �� ��: i2c_Start
*	����˵��: CPU����I2C���������ź�
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void i2c_Start(void)
{
	/* ��SCL�ߵ�ƽʱ��SDA����һ�������ر�ʾI2C���������ź� */
    i2c_gpio_output();
	SOFT_I2C_SDA_1();
	SOFT_I2C_SCL_1();
	i2c_Delay();
	SOFT_I2C_SDA_0();
	i2c_Delay();
	SOFT_I2C_SCL_0();
	i2c_Delay();
}

/*
*********************************************************************************************************
*	�� �� ��: i2c_StOP
*	����˵��: CPU����I2C����ֹͣ�ź�
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void i2c_Stop(void)
{
	/* ��SCL�ߵ�ƽʱ��SDA����һ�������ر�ʾI2C����ֹͣ�ź� */
    i2c_gpio_output();
	SOFT_I2C_SDA_0();
	SOFT_I2C_SCL_1();
	i2c_Delay();
	SOFT_I2C_SDA_1();
}

/*
*********************************************************************************************************
*	�� �� ��: i2c_SendByte
*	����˵��: CPU��I2C�����豸����8bit����
*	��    �Σ�_ucByte �� �ȴ����͵��ֽ�
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void i2c_SendByte(uint8_t _ucByte)
{
	uint8_t i;
    i2c_gpio_output();

	/* �ȷ����ֽڵĸ�λbit7 */
	for (i = 0; i < 8; i++)
	{		
		if (_ucByte & 0x80)
		{
			SOFT_I2C_SDA_1();
		}
		else
		{
			SOFT_I2C_SDA_0();
		}
		i2c_Delay();
		SOFT_I2C_SCL_1();
		i2c_Delay();	
		SOFT_I2C_SCL_0();
		if (i == 7)
		{
			 SOFT_I2C_SDA_1(); // �ͷ�����
		}
		_ucByte <<= 1;	/* ����һ��bit */
		i2c_Delay();
	}
}

/*
*********************************************************************************************************
*	�� �� ��: i2c_ReadByte
*	����˵��: CPU��I2C�����豸��ȡ8bit����
*	��    �Σ���
*	�� �� ֵ: ����������
*********************************************************************************************************
*/
uint8_t i2c_ReadByte(void)
{
	uint8_t i;
	uint8_t value;
    i2c_gpio_input();

	/* ������1��bitΪ���ݵ�bit7 */
	value = 0;
	for (i = 0; i < 8; i++)
	{
		value <<= 1;
		SOFT_I2C_SCL_1();
		i2c_Delay();
		if (SOFT_I2C_SDA_READ())
		{
			value++;
		}
		SOFT_I2C_SCL_0();
		i2c_Delay();
	}
	return value;
}

/*
*********************************************************************************************************
*	�� �� ��: i2c_WaitAck
*	����˵��: CPU����һ��ʱ�ӣ�����ȡ������ACKӦ���ź�
*	��    �Σ���
*	�� �� ֵ: ����0��ʾ��ȷӦ��1��ʾ��������Ӧ
*********************************************************************************************************
*/
uint8_t i2c_WaitAck(void)
{
	uint8_t re;

    i2c_gpio_output();
	SOFT_I2C_SDA_1();	/* CPU�ͷ�SDA���� */
    i2c_gpio_input();
	i2c_Delay();
	SOFT_I2C_SCL_1();	/* CPU����SCL = 1, ��ʱ�����᷵��ACKӦ�� */
	i2c_Delay();
	if (SOFT_I2C_SDA_READ())	/* CPU��ȡSDA����״̬ */
	{
		re = 1;
	}
	else
	{
		re = 0;
	}
	SOFT_I2C_SCL_0();
	i2c_Delay();
	return re;
}

/*
*********************************************************************************************************
*	�� �� ��: i2c_Ack
*	����˵��: CPU����һ��ACK�ź�
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void i2c_Ack(void)
{
    i2c_gpio_output();
	SOFT_I2C_SDA_0();	/* CPU����SDA = 0 */
	i2c_Delay();
	SOFT_I2C_SCL_1();	/* CPU����1��ʱ�� */
	i2c_Delay();
	SOFT_I2C_SCL_0();
	i2c_Delay();
	SOFT_I2C_SDA_1();	/* CPU�ͷ�SDA���� */
}

/*
*********************************************************************************************************
*	�� �� ��: i2c_NAck
*	����˵��: CPU����1��NACK�ź�
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void i2c_NAck(void)
{
	SOFT_I2C_SDA_1();	/* CPU����SDA = 1 */
	i2c_Delay();
	SOFT_I2C_SCL_1();	/* CPU����1��ʱ�� */
	i2c_Delay();
	SOFT_I2C_SCL_0();
	i2c_Delay();	
}

/*
*********************************************************************************************************
*	�� �� ��: i2c_CfgGpio
*	����˵��: ����I2C���ߵ�GPIO������ģ��IO�ķ�ʽʵ��
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void i2c_CfgGpio(void)
{
    //MX_GPIO_Init();

	/* ��һ��ֹͣ�ź�, ��λI2C�����ϵ������豸������ģʽ */
	i2c_Stop();
}

/*
*********************************************************************************************************
*	�� �� ��: i2c_CheckDevice
*	����˵��: ���I2C�����豸��CPU�����豸��ַ��Ȼ���ȡ�豸Ӧ�����жϸ��豸�Ƿ����
*	��    �Σ�_Address���豸��I2C���ߵ�ַ
*	�� �� ֵ: ����ֵ 0 ��ʾ��ȷ�� ����1��ʾδ̽�⵽
*********************************************************************************************************
*/
uint8_t i2c_CheckDevice(uint8_t _Address)
{
	uint8_t ucAck;

	i2c_CfgGpio();		/* ����GPIO */

	
	i2c_Start();		/* ���������ź� */

	/* �����豸��ַ+��д����bit��0 = w�� 1 = r) bit7 �ȴ� */
	i2c_SendByte(_Address | SOFT_I2C_WR);
	ucAck = i2c_WaitAck();	/* ����豸��ACKӦ�� */

	i2c_Stop();			/* ����ֹͣ�ź� */

	return ucAck;
}

uint8_t i2c_transmit(uint8_t DevAddress, uint8_t *pData, uint16_t Size, uint32_t Timeout)
{
	uint16_t i,m;

    /*���ڣ�������ֹͣ�źţ������ڲ�д������*/
    i2c_Stop();
    
    /* ͨ���������Ӧ��ķ�ʽ���ж��ڲ�д�����Ƿ����, һ��С�� 10ms 			
        CLKƵ��Ϊ200KHzʱ����ѯ����Ϊ30������
    */
    for (m = 0; m < 1000; m++)
    {				
        /* ��1��������I2C���������ź� */
        i2c_Start();
        
        /* ��2������������ֽڣ���7bit�ǵ�ַ��bit0�Ƕ�д����λ��0��ʾд��1��ʾ�� */
        i2c_SendByte(DevAddress | SOFT_I2C_WR);	/* �˴���дָ�� */
        
        /* ��3��������һ��ʱ�ӣ��ж������Ƿ���ȷӦ�� */
        if (i2c_WaitAck() == 0)
        {
            break;
        }
    }
    if (m  == 1000)
    {
        goto cmd_fail;	/* EEPROM����д��ʱ */
    }
    
    for (i = 0; i < Size; i++) {
        /* ��6������ʼд������ */
        i2c_SendByte(pData[i]);

        /* ��7��������ACK */
        if (i2c_WaitAck() != 0)
        {
            goto cmd_fail;	/* EEPROM������Ӧ�� */
        }
    }
	
	/* ����ִ�гɹ�������I2C����ֹͣ�ź� */
	i2c_Stop();
	return 0;

cmd_fail: /* ����ִ��ʧ�ܺ��мǷ���ֹͣ�źţ�����Ӱ��I2C�����������豸 */
	/* ����I2C����ֹͣ�ź� */
	i2c_Stop();
	return 1;
}

uint8_t i2c_receive(uint16_t DevAddress, uint8_t *pData, uint16_t Size, uint32_t Timeout)
{
	uint16_t i;
	
	/* ��1��������I2C���������ź� */
	i2c_Start();
	
	/* ��2������������ֽڣ���7bit�ǵ�ַ��bit0�Ƕ�д����λ��0��ʾд��1��ʾ�� */
	i2c_SendByte(DevAddress | SOFT_I2C_RD);	/* �˴���дָ�� */
	 
	/* ��3�����ȴ�ACK */
	if (i2c_WaitAck() != 0)
	{
		goto cmd_fail;	/* EEPROM������Ӧ�� */
	}	
	
	/* ��9����ѭ����ȡ���� */
	for (i = 0; i < Size; i++)
	{
		pData[i] = i2c_ReadByte();	/* ��1���ֽ� */
		
		/* ÿ����1���ֽں���Ҫ����Ack�� ���һ���ֽڲ���ҪAck����Nack */
		if (i != Size - 1)
		{
			i2c_Ack();	/* �м��ֽڶ����CPU����ACK�ź�(����SDA = 0) */
		}
		else
		{
			i2c_NAck();	/* ���1���ֽڶ����CPU����NACK�ź�(����SDA = 1) */
		}
	}
	/* ����I2C����ֹͣ�ź� */
	i2c_Stop();
	return 0;	/* ִ�гɹ� */

cmd_fail: /* ����ִ��ʧ�ܺ��мǷ���ֹͣ�źţ�����Ӱ��I2C�����������豸 */
	/* ����I2C����ֹͣ�ź� */
	i2c_Stop();
	return 1;
}
