#ifndef _USMART_H_
#define _USMART_H_


#include "stm32f4xx_hal.h"
#include "usart.h"



//----------------------------------------------------------------------------
//
//																	�û�
//
//----------------------------------------------------------------------------
#define MAX_FNAME_LEN 		30	//��������󳤶ȣ�Ӧ������Ϊ��С����������ĳ��ȡ�											   
#define MAX_PARM 					10	//���Ϊ10������ ,�޸Ĵ˲���,�����޸�usmart_exe��֮��Ӧ.
#define PARM_LEN 					200	//���в���֮�͵ĳ��Ȳ�����PARM_LEN���ֽ�,ע�⴮�ڽ��ղ���Ҫ��֮��Ӧ(��С��PARM_LEN)


#define USMART_ENTIMX_SCAN 	1	//ʹ��TIM�Ķ�ʱ�ж���ɨ��SCAN����,�������Ϊ0,��Ҫ�Լ�ʵ�ָ�һ��ʱ��ɨ��һ��scan����.
								//ע��:���Ҫ��runtimeͳ�ƹ���,��������USMART_ENTIMX_SCANΪ1!!!!
								
#define USMART_USE_HELP		1	//ʹ�ð�������ֵ��Ϊ0�����Խ�ʡ��700���ֽڣ����ǽ������޷���ʾ������Ϣ��
#define USMART_USE_WRFUNS	1	//ʹ�ö�д����,ʹ������,���Զ�ȡ�κε�ַ��ֵ,������д�Ĵ�����ֵ.
///////////////////////////////////////////////END///////////////////////////////////////////////////////////

#define USMART_OK 				0  //�޴���
#define USMART_FUNCERR 		1  //��������
#define USMART_PARMERR 		2  //��������
#define USMART_PARMOVER 	3  //�������
#define USMART_NOFUNCFIND 4  //δ�ҵ�ƥ�亯��

#define SP_TYPE_DEC      	0  //10���Ʋ�����ʾ
#define SP_TYPE_HEX       1  //16���Ʋ�����ʾ


//�������б�	 
struct _m_usmart_nametab
{
	void* func;				//����ָ��
	const uint8_t* name;		//������(���Ҵ�)	 
};



//usmart���ƹ�����
struct _m_usmart_dev
{
	struct _m_usmart_nametab *funs;	//������ָ��

	void (*init)(uint8_t);				//��ʼ��
	uint8_t (*cmd_rec)(uint8_t*str);			//ʶ������������
	void (*exe)(void); 				//ִ�� 
	void (*scan)(void);             //ɨ��
	uint8_t fnum; 				  		//��������
	uint8_t pnum;                        //��������
	uint8_t id;							//����id
	uint8_t sptype;						//������ʾ����(���ַ�������):0,10����;1,16����;
	uint16_t parmtype;					//����������
	uint8_t  plentbl[MAX_PARM];  		//ÿ�������ĳ����ݴ��
	uint8_t  parm[PARM_LEN];  			//�����Ĳ���
	uint8_t runtimeflag;					//0,��ͳ�ƺ���ִ��ʱ��;1,ͳ�ƺ���ִ��ʱ��,ע��:�˹��ܱ�����USMART_ENTIMX_SCANʹ�ܵ�ʱ��,������
	uint32_t runtime;					//����ʱ��,��λ:0.1ms,�����ʱʱ��Ϊ��ʱ��CNTֵ��2��*0.1ms
};

 void usmart_init(uint8_t sysclk);			//��ʼ��
 uint8_t usmart_cmd_rec(uint8_t*str);				//ʶ��
 void usmart_exe(void);						//ִ��
 void usmart_scan(void);     			//ɨ��
 uint32_t read_addr(uint32_t addr);					//��ȡָ����ַ��ֵ
 void write_addr(uint32_t addr,uint32_t val);//��ָ����ַд��ָ����ֵ
 uint32_t usmart_get_runtime(void);			//��ȡ����ʱ��
 void usmart_reset_runtime(void);	//��λ����ʱ��




extern struct _m_usmart_nametab usmart_nametab[];	//��usmart_config.c���涨��
extern struct _m_usmart_dev usmart_dev;				//��usmart_config.c���涨��






#define USART_REC_LEN  			200  	//�����������ֽ��� 200
#define EN_USART1_RX 			1		//ʹ�ܣ�1��/��ֹ��0������1����
	  	
extern uint8_t  USART_RX_BUF[USART_REC_LEN]; //���ջ���,���USART_REC_LEN���ֽ�.ĩ�ֽ�Ϊ���з� 
extern uint16_t USART_RX_STA;         		//����״̬���	
extern UART_HandleTypeDef UART1_Handler; //UART���

#define RXBUFFERSIZE   1 //�����С
extern uint8_t aRxBuffer[RXBUFFERSIZE];//HAL��USART����Buffer
void Usmart_Rev_Pro(void);


#define CONFIG_HARDWARE_VERSION "Mini_MultiCopter_v01"
#define CONFIG_SOFTWARE_VERSION "Mini_MultiCopter_v01"

void print_Info ( void );
//----------------------------------------------------------------------------
//
//																	End
//
//----------------------------------------------------------------------------







#endif

