#include "usmart.h"
#include "stdio.h"




//------------------------------------------------------------------------------
//
//																HAL�⺯��˵��	
//
//------------------------------------------------------------------------------

//    ���ڵķ��ͽ��պ�����
//    HAL_UART_Transmit();������ѯģʽ����,ʹ�ó�ʱ������ơ�
//    HAL_UART_Receive();������ѯģʽ����,ʹ�ó�ʱ������ơ�
//    HAL_UART_Transmit_IT();�����ж�ģʽ���ͣ�
//    HAL_UART_Receive_IT();�����ж�ģʽ����
//    HAL_UART_Transmit_DMA();����DMAģʽ����
//    HAL_UART_Receive_DMA();����DMAģʽ����
      
//    ������ص��жϺ���:
//    HAL_UART_TxHalfCpltCallback():һ������(half transfer)������ɺ�ͨ���жϴ��������á�
//    HAL_UART_TxCpltCallback():������ɺ�ͨ���жϴ��������á�
//    HAL_UART_RxHalfCpltCallback():һ������(half transfer)������ɺ�ͨ���жϴ��������á�
//    HAL_UART_RxCpltCallback():������ɺ�ͨ���жϴ��������á�
//    HAL_UART_ErrorCallback()����������г��ִ���ʱ��ͨ���жϴ��������á�


//------------------------------------------------------------------------------
//
//																	������ʼ��Ϣ
//
//------------------------------------------------------------------------------
void Get_ChipSerialNum ( void )
{
    uint32_t IntDeviceSerial[3];

    IntDeviceSerial[0] = * ( __IO uint32_t* ) ( 0x1FFF7A10 );
    IntDeviceSerial[1] = * ( __IO uint32_t* ) ( 0x1FFF7A14 );
    IntDeviceSerial[2] = * ( __IO uint32_t* ) ( 0x1FFF7A18 );

    printf ( "SerialNum   : %08x %08x %08x\r\n",
                IntDeviceSerial[0],
                IntDeviceSerial[1],
                IntDeviceSerial[2]
              );
}

void print_Info ( void )
{

    printf ( "\r\n-------------------------\r\n" );
    printf ( "Hardware ver: %s\r\n", CONFIG_HARDWARE_VERSION ); 
    printf ( "Software ver: %s\r\n", CONFIG_SOFTWARE_VERSION );
  
    Get_ChipSerialNum();

}



//------------------------------------------------------------------------------
//
//																	�ض���	
//
//------------------------------------------------------------------------------

#ifdef __GNUC__

	#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)/* With GCC/RAISONANCE, small printf (option LD Linker->Libraries->Small printf set to 'Yes') calls __io_putchar() */

#else

 #define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)

#endif /* __GNUC__ */

 
/**

 * @brief  Retargets the C library printf function to the USART.

 * @param  None

 * @retval None

 */

PUTCHAR_PROTOTYPE
{

 /* Place your implementation of fputc here */

 /* e.g. write a character to the EVAL_COM1 and Loop until the end of transmission */

 HAL_UART_Transmit(&huart1, (uint8_t *)&ch, 1, 0xFFFF);


 return ch;

}

//------------------------------------------------------------------------------
//
//																  ��ֲ��
//
//------------------------------------------------------------------------------




//------------------------------------------------------------------------------
//
//																���տ��ƶ���	
//
//------------------------------------------------------------------------------

TIM_HandleTypeDef TIM4_Handler;//��ʱ����� 

uint16_t USART_RX_STA=0;       //����״̬���	
uint8_t USART_RX_BUF[USART_REC_LEN];     //���ջ���,���USART_REC_LEN���ֽ�.
//ϵͳ����
uint8_t *sys_cmd_tab[]=
{
	"?",
	"help",
	"list",
	"id",
	"hex",
	"dec",
	"runtime",	   
};	



//------------------------------------------------------------------------------
//
//																���տ���	
//
//------------------------------------------------------------------------------



uint8_t usmart_sys_cmd_exe(uint8_t *str)
{
	uint8_t i;
	uint8_t sfname[MAX_FNAME_LEN];//��ű��غ�����
	uint8_t pnum;
	uint8_t rval;
	uint32_t res;  
	res=usmart_get_cmdname(str,sfname,&i,MAX_FNAME_LEN);//�õ�ָ�ָ���
	if(res)return USMART_FUNCERR;//�����ָ�� 
	str+=i;	 	 			    
	for(i=0;i<sizeof(sys_cmd_tab)/4;i++)//֧�ֵ�ϵͳָ��
	{
		if(usmart_strcmp(sfname,sys_cmd_tab[i])==0)break;
	}
	switch(i)
	{					   
		case 0:
		case 1://����ָ��
			printf("\r\n");
#if USMART_USE_HELP 
			printf("------------------------USMART V3.3------------------------ \r\n");
			printf("USMART��7��ϵͳ����(����Сд):\r\n");
			printf("?:      ��ȡ������Ϣ\r\n");
			printf("help:   ��ȡ������Ϣ\r\n");
			printf("list:   ���õĺ����б�\r\n\n");
			printf("id:     ���ú�����ID�б�\r\n\n");
			printf("hex:    ����16������ʾ,����ո�+���ּ�ִ�н���ת��\r\n\n");
			printf("dec:    ����10������ʾ,����ո�+���ּ�ִ�н���ת��\r\n\n");
			printf("runtime:1,�����������м�ʱ;0,�رպ������м�ʱ;\r\n\n");
			printf("�밴�ճ����д��ʽ���뺯�������������Իس�������.\r\n");    
			printf("----------------------------------------------------------- \r\n");
#else
			printf("ָ��ʧЧ\r\n");
#endif
			break;
		case 2://��ѯָ��
			printf("\r\n");
			printf("-------------------------�����嵥--------------------------- \r\n");
			for(i=0;i<usmart_dev.fnum;i++)printf("%s\r\n",usmart_dev.funs[i].name);
			printf("\r\n");
			break;	 
		case 3://��ѯID
			printf("\r\n");
			printf("-------------------------���� ID --------------------------- \r\n");
			for(i=0;i<usmart_dev.fnum;i++)
			{
				usmart_get_fname((uint8_t*)usmart_dev.funs[i].name,sfname,&pnum,&rval);//�õ����غ����� 
				printf("%s id is:\r\n0X%08X\r\n",sfname,usmart_dev.funs[i].func); //��ʾID
			}
			printf("\r\n");
			break;
		case 4://hexָ��
			printf("\r\n");
			usmart_get_aparm(str,sfname,&i);
			if(i==0)//��������
			{
				i=usmart_str2num(sfname,&res);	   	//��¼�ò���	
				if(i==0)						  	//����ת������
				{
					printf("HEX:0X%X\r\n",res);	   	//תΪ16����
				}else if(i!=4)return USMART_PARMERR;//��������.
				else 				   				//������ʾ�趨����
				{
					printf("16���Ʋ�����ʾ!\r\n");
					usmart_dev.sptype=SP_TYPE_HEX;  
				}

			}else return USMART_PARMERR;			//��������.
			printf("\r\n"); 
			break;
		case 5://decָ��
			printf("\r\n");
			usmart_get_aparm(str,sfname,&i);
			if(i==0)//��������
			{
				i=usmart_str2num(sfname,&res);	   	//��¼�ò���	
				if(i==0)						   	//����ת������
				{
					printf("DEC:%lu\r\n",res);	   	//תΪ10����
				}else if(i!=4)return USMART_PARMERR;//��������.
				else 				   				//������ʾ�趨����
				{
					printf("10���Ʋ�����ʾ!\r\n");
					usmart_dev.sptype=SP_TYPE_DEC;  
				}

			}else return USMART_PARMERR;			//��������. 
			printf("\r\n"); 
			break;	 
		case 6://runtimeָ��,�����Ƿ���ʾ����ִ��ʱ��
			printf("\r\n");
			usmart_get_aparm(str,sfname,&i);
			if(i==0)//��������
			{
				i=usmart_str2num(sfname,&res);	   		//��¼�ò���	
				if(i==0)						   		//��ȡָ����ַ���ݹ���
				{
					if(USMART_ENTIMX_SCAN==0)printf("\r\nError! \r\nTo EN RunTime function,Please set USMART_ENTIMX_SCAN = 1 first!\r\n");//����
					else
					{
						usmart_dev.runtimeflag=res;
						if(usmart_dev.runtimeflag)printf("Run Time Calculation ON\r\n");
						else printf("Run Time Calculation OFF\r\n"); 
					}
				}else return USMART_PARMERR;   			//δ������,���߲�������	 
 			}else return USMART_PARMERR;				//��������. 
			printf("\r\n"); 
			break;	    
		default://�Ƿ�ָ��
			return USMART_FUNCERR;
	}
	return 0;
}



////////////////////////////////////////////////////////////////////////////////////////
//��ֲע��:��������stm32Ϊ��,���Ҫ��ֲ������mcu,������Ӧ�޸�.
//usmart_reset_runtime,�����������ʱ��,��ͬ��ʱ���ļ����Ĵ����Լ���־λһ������.��������װ��ֵΪ���,������޶ȵ��ӳ���ʱʱ��.
//usmart_get_runtime,��ȡ��������ʱ��,ͨ����ȡCNTֵ��ȡ,����usmart��ͨ���жϵ��õĺ���,���Զ�ʱ���жϲ�����Ч,��ʱ����޶�
//ֻ��ͳ��2��CNT��ֵ,Ҳ���������+���һ��,���������2��,û������,���������ʱ,������:2*������CNT*0.1ms.��STM32��˵,��:13.1s����
//������:TIM4_IRQHandler��Timer4_Init,��Ҫ����MCU�ص������޸�.ȷ������������Ƶ��Ϊ:10Khz����.����,��ʱ����Ҫ�����Զ���װ�ع���!!




#if USMART_ENTIMX_SCAN==1
//��λruntime
//��Ҫ��������ֲ����MCU�Ķ�ʱ�����������޸�
void usmart_reset_runtime(void)
{
    __HAL_TIM_CLEAR_FLAG(&TIM4_Handler,TIM_FLAG_UPDATE);//����жϱ�־λ 
    __HAL_TIM_SET_AUTORELOAD(&TIM4_Handler,0XFFFF);     //����װ��ֵ���õ����
    __HAL_TIM_SET_COUNTER(&TIM4_Handler,0);             //��ն�ʱ����CNT
	usmart_dev.runtime=0;	
}






//���runtimeʱ��
//����ֵ:ִ��ʱ��,��λ:0.1ms,�����ʱʱ��Ϊ��ʱ��CNTֵ��2��*0.1ms
//��Ҫ��������ֲ����MCU�Ķ�ʱ�����������޸�
uint32_t usmart_get_runtime(void)
{
	if(__HAL_TIM_GET_FLAG(&TIM4_Handler,TIM_FLAG_UPDATE)==SET)//�������ڼ�,�����˶�ʱ�����
	{
		usmart_dev.runtime+=0XFFFF;
	}
	usmart_dev.runtime+=__HAL_TIM_GET_COUNTER(&TIM4_Handler);
	return usmart_dev.runtime;		//���ؼ���ֵ
}  




//��������������,��USMART����,�ŵ�����,����������ֲ. 
//��ʱ��4�жϷ������	 
void TIM4_IRQHandler(void)
{ 		    		  			       
    if(__HAL_TIM_GET_IT_SOURCE(&TIM4_Handler,TIM_IT_UPDATE)==SET)//����ж�
    {
        usmart_dev.scan();	//ִ��usmartɨ��
        __HAL_TIM_SET_COUNTER(&TIM4_Handler,0);    //��ն�ʱ����CNT
        __HAL_TIM_SET_AUTORELOAD(&TIM4_Handler,100);//�ָ�ԭ��������
    }
    __HAL_TIM_CLEAR_IT(&TIM4_Handler, TIM_IT_UPDATE);//����жϱ�־λ
}

//ʹ�ܶ�ʱ��4,ʹ���ж�.
void Timer4_Init(uint16_t arr,uint16_t psc)
{ 
    //��ʱ��4
    __HAL_RCC_TIM4_CLK_ENABLE();
    HAL_NVIC_SetPriority(TIM4_IRQn,3,3);    //�����ж����ȼ�����ռ���ȼ�3�������ȼ�3
    HAL_NVIC_EnableIRQ(TIM4_IRQn);          //����ITM4�ж�    
    
    TIM4_Handler.Instance=TIM4;                          //ͨ�ö�ʱ��4
    TIM4_Handler.Init.Prescaler=psc;                     //��Ƶ
    TIM4_Handler.Init.CounterMode=TIM_COUNTERMODE_UP;    //���ϼ�����
    TIM4_Handler.Init.Period=arr;                        //�Զ�װ��ֵ
    TIM4_Handler.Init.ClockDivision=TIM_CLOCKDIVISION_DIV1;
    HAL_TIM_Base_Init(&TIM4_Handler);
    HAL_TIM_Base_Start_IT(&TIM4_Handler); //ʹ�ܶ�ʱ��4�Ͷ�ʱ��4�ж� 					 
}
 




#endif
////////////////////////////////////////////////////////////////////////////////////////
//��ʼ�����ڿ�����
//sysclk:ϵͳʱ�ӣ�Mhz��
void usmart_init(uint8_t sysclk)
{
	__HAL_UART_ENABLE_IT(&huart1, UART_IT_RXNE); 
	
	
#if USMART_ENTIMX_SCAN==1
	Timer4_Init(1000,(uint32_t)sysclk*100-1);//��Ƶ,ʱ��Ϊ10K ,100ms�ж�һ��,ע��,����Ƶ�ʱ���Ϊ10Khz,�Ժ�runtime��λ(0.1ms)ͬ��.
#endif
	usmart_dev.sptype=1;	//ʮ��������ʾ����
	
		print_Info();
}		
//��str�л�ȡ������,id,��������Ϣ
//*str:�ַ���ָ��.
//����ֵ:0,ʶ��ɹ�;����,�������.
uint8_t usmart_cmd_rec(uint8_t*str) 
{
	uint8_t sta,i,rval;//״̬	 
	uint8_t rpnum,spnum;
	uint8_t rfname[MAX_FNAME_LEN];//�ݴ�ռ�,���ڴ�Ž��յ��ĺ�����  
	uint8_t sfname[MAX_FNAME_LEN];//��ű��غ�����
	sta=usmart_get_fname(str,rfname,&rpnum,&rval);//�õ����յ������ݵĺ���������������	  
	if(sta)return sta;//����
	for(i=0;i<usmart_dev.fnum;i++)
	{
		sta=usmart_get_fname((uint8_t*)usmart_dev.funs[i].name,sfname,&spnum,&rval);//�õ����غ���������������
		if(sta)return sta;//���ؽ�������	  
		if(usmart_strcmp(sfname,rfname)==0)//���
		{
			if(spnum>rpnum)return USMART_PARMERR;//��������(���������Դ����������)
			usmart_dev.id=i;//��¼����ID.
			break;//����.
		}	
	}
	if(i==usmart_dev.fnum)return USMART_NOFUNCFIND;	//δ�ҵ�ƥ��ĺ���
 	sta=usmart_get_fparam(str,&i);					//�õ�������������	
	if(sta)return sta;								//���ش���
	usmart_dev.pnum=i;								//����������¼
    return USMART_OK;
}
//usamrtִ�к���
//�ú�����������ִ�дӴ����յ�����Ч����.
//���֧��10�������ĺ���,����Ĳ���֧��Ҳ������ʵ��.�����õĺ���.һ��5�����ҵĲ����ĺ����Ѿ����ټ���.
//�ú������ڴ��ڴ�ӡִ�����.��:"������(����1������2...����N)=����ֵ".����ʽ��ӡ.
//����ִ�еĺ���û�з���ֵ��ʱ��,����ӡ�ķ���ֵ��һ�������������.
void usmart_exe(void)
{
	uint8_t id,i;
	uint32_t res;		   
	uint32_t temp[MAX_PARM];//����ת��,ʹ֧֮�����ַ��� 
	uint8_t sfname[MAX_FNAME_LEN];//��ű��غ�����
	uint8_t pnum,rval;
	id=usmart_dev.id;
	if(id>=usmart_dev.fnum)return;//��ִ��.
	usmart_get_fname((uint8_t*)usmart_dev.funs[id].name,sfname,&pnum,&rval);//�õ����غ�����,���������� 
	printf("\r\n%s(",sfname);//�����Ҫִ�еĺ�����
	for(i=0;i<pnum;i++)//�������
	{
		if(usmart_dev.parmtype&(1<<i))//�������ַ���
		{
			printf("%c",'"');			 
			printf("%s",usmart_dev.parm+usmart_get_parmpos(i));
			printf("%c",'"');
			temp[i]=(uint32_t)&(usmart_dev.parm[usmart_get_parmpos(i)]);
		}else						  //����������
		{
			temp[i]=*(uint32_t*)(usmart_dev.parm+usmart_get_parmpos(i));
			if(usmart_dev.sptype==SP_TYPE_DEC)printf("%ld",temp[i]);//10���Ʋ�����ʾ
			else printf("0X%X",temp[i]);//16���Ʋ�����ʾ 	   
		}
		if(i!=pnum-1)printf(",");
	}
	printf(")");
#if USMART_ENTIMX_SCAN==1
	usmart_reset_runtime();	//��ʱ������,��ʼ��ʱ
#endif
	switch(usmart_dev.pnum)
	{
		case 0://�޲���(void����)											  
			res=(*(uint32_t(*)())usmart_dev.funs[id].func)();
			break;
	    case 1://��1������
			res=(*(uint32_t(*)())usmart_dev.funs[id].func)(temp[0]);
			break;
	    case 2://��2������
			res=(*(uint32_t(*)())usmart_dev.funs[id].func)(temp[0],temp[1]);
			break;
	    case 3://��3������
			res=(*(uint32_t(*)())usmart_dev.funs[id].func)(temp[0],temp[1],temp[2]);
			break;
	    case 4://��4������
			res=(*(uint32_t(*)())usmart_dev.funs[id].func)(temp[0],temp[1],temp[2],temp[3]);
			break;
	    case 5://��5������
			res=(*(uint32_t(*)())usmart_dev.funs[id].func)(temp[0],temp[1],temp[2],temp[3],temp[4]);
			break;
	    case 6://��6������
			res=(*(uint32_t(*)())usmart_dev.funs[id].func)(temp[0],temp[1],temp[2],temp[3],temp[4],\
			temp[5]);
			break;
	    case 7://��7������
			res=(*(uint32_t(*)())usmart_dev.funs[id].func)(temp[0],temp[1],temp[2],temp[3],temp[4],\
			temp[5],temp[6]);
			break;
	    case 8://��8������
			res=(*(uint32_t(*)())usmart_dev.funs[id].func)(temp[0],temp[1],temp[2],temp[3],temp[4],\
			temp[5],temp[6],temp[7]);
			break;
	    case 9://��9������
			res=(*(uint32_t(*)())usmart_dev.funs[id].func)(temp[0],temp[1],temp[2],temp[3],temp[4],\
			temp[5],temp[6],temp[7],temp[8]);
			break;
	    case 10://��10������
			res=(*(uint32_t(*)())usmart_dev.funs[id].func)(temp[0],temp[1],temp[2],temp[3],temp[4],\
			temp[5],temp[6],temp[7],temp[8],temp[9]);
			break;
	}
#if USMART_ENTIMX_SCAN==1
	usmart_get_runtime();//��ȡ����ִ��ʱ��
#endif
	if(rval==1)//��Ҫ����ֵ.
	{
		if(usmart_dev.sptype==SP_TYPE_DEC)printf("=%lu;\r\n",res);//���ִ�н��(10���Ʋ�����ʾ)
		else printf("=0X%X;\r\n",res);//���ִ�н��(16���Ʋ�����ʾ)	   
	}else printf(";\r\n");		//����Ҫ����ֵ,ֱ���������
	if(usmart_dev.runtimeflag)	//��Ҫ��ʾ����ִ��ʱ��
	{ 
		printf("Function Run Time:%d.%1dms\r\n",usmart_dev.runtime/10,usmart_dev.runtime%10);//��ӡ����ִ��ʱ�� 
	}	
}
//usmartɨ�躯��
//ͨ�����øú���,ʵ��usmart�ĸ�������.�ú�����Ҫÿ��һ��ʱ�䱻����һ��
//�Լ�ʱִ�дӴ��ڷ������ĸ�������.
//�������������ж��������,�Ӷ�ʵ���Զ�����.
//�����ALIENTEK�û�,��USART_RX_STA��USART_RX_BUF[]��Ҫ�û��Լ�ʵ��
void usmart_scan(void)
{
	uint8_t sta,len;  
	if(USART_RX_STA&0x8000)//���ڽ�����ɣ�
	{					   
		len=USART_RX_STA&0x3fff;	//�õ��˴ν��յ������ݳ���
		USART_RX_BUF[len]='\0';	//��ĩβ���������. 
		sta=usmart_dev.cmd_rec(USART_RX_BUF);//�õ�����������Ϣ
		if(sta==0)usmart_dev.exe();	//ִ�к��� 
		else 
		{  
			len=usmart_sys_cmd_exe(USART_RX_BUF);
			if(len!=USMART_FUNCERR)sta=len;
			if(sta)
			{   
				switch(sta)
				{
					case USMART_FUNCERR:
						printf("��������!\r\n");
                      				
						break;	
					case USMART_PARMERR:
						printf("��������!\r\n");   			
						break;				
					case USMART_PARMOVER:
						printf("����̫��!\r\n");   			
						break;		
					case USMART_NOFUNCFIND:
						printf("δ�ҵ�ƥ��ĺ���!\r\n");   			
						break;		
				}
			}
		}
		USART_RX_STA=0;//״̬�Ĵ������	    
	}
}






#if USMART_USE_WRFUNS==1 	//���ʹ���˶�д���� 
//��ȡָ����ַ��ֵ		 
uint32_t read_addr(uint32_t addr)
{
	return *(uint32_t*)addr;//	
}
//��ָ����ַд��ָ����ֵ		 
void write_addr(uint32_t addr,uint32_t val)
{
	*(uint32_t*)addr=val; 	
}
#endif




void Usmart_Rev_Pro(void)
{
	uint8_t Res;
	if((__HAL_UART_GET_FLAG(&huart1,UART_FLAG_RXNE)!=RESET))  //�����ж�(���յ������ݱ�����0x0d 0x0a��β)
	{
        HAL_UART_Receive(&huart1,&Res,1,1000); 
		if((USART_RX_STA&0x8000)==0)//����δ���
		{
			if(USART_RX_STA&0x4000)//���յ���0x0d
			{
				if(Res!=0x0a)USART_RX_STA=0;//���մ���,���¿�ʼ
				else USART_RX_STA|=0x8000;	//��������� 
			}
			else //��û�յ�0X0D
			{	
				if(Res==0x0d)USART_RX_STA|=0x4000;
				else
				{
					USART_RX_BUF[USART_RX_STA&0X3FFF]=Res ;
					USART_RX_STA++;
					if(USART_RX_STA>(USART_REC_LEN-1))USART_RX_STA=0;//�������ݴ���,���¿�ʼ����	  
				}		 
			}
		}   		 
	}	
}

























