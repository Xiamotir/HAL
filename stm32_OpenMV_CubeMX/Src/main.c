/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "bsp_mcu.h"
#include "bsp_usart.h"
//#include "bsp_st7735r.h"
#include "bsp_st7789vw.h"

#include "ugui.h"
#include "SDFatfs.h"

#include "framebuffer.h"
#include "sensor.h"
#include "imlib.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
UG_GUI  GUI_1;

image_t img;
rectangle_t roi;
array_t *blobs;
list_t out;
list_t thresholds;
color_thresholds_list_lnk_data_t lnk_data;

rectangle_t *r;

uint8_t u8statue = 0;
int8_t ret = 0;
uint32_t u32time1 = 0, u32time2 = 0, u32temp = 0;

extern PCD_HandleTypeDef hpcd_USB_OTG_FS;


/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/**
  * @brief  Main program
  * @param  None
  * @retval None
  */
int main(void)
{

    /* MCU Configuration----------------------------------------------------------*/
    
    /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
    HAL_Init();
    SystemClock_Config();
    NVIC_Configuration();
    GPIO_Configuration();
    systick_init();
    USART3_Configuration(UART3_BAUD);
    DEBUG("OpenMV BUILD:%04d %s %s\r\n" ,BUILD_NUMBER , __DATE__, __TIME__ );
    
    ST7789VW_Init();
    UG_Init( &GUI_1, (void*)ST7789VW_DrawPoint, (UG_S16)ST7789VW_WIDTH, (UG_S16)ST7789VW_HEIGHT );
    UG_SelectGUI(&GUI_1);
    UG_FontSelect(&FONT_12X16);
    UG_SetForecolor(C_BLACK  );
    UG_SetBackcolor(C_DARK_GRAY );
    UG_PutString(0,170,"Hello OpenMV4!!");
    
    SDCard_Init();
    
    USB_DEVICE_Init();
    delay_ms(100);
    USB_printf("USB_DEVICE_Init OK!!\r\n");

    fb_alloc_init0();
    u32temp = fb_avail();
    DEBUG("fb_avail：%d",u32temp);
    ret = sensor_init();
    if (ret < 0) {
        DEBUG("sensor_init return %d\n", ret);
        while(1);
    }

    ret = sensor_reset();
    if (ret < 0) {
        DEBUG("sensor_reset return %d\n", ret);
        while(1);
    }
    
    ret = sensor_set_pixformat(PIXFORMAT_RGB565);
    if (ret < 0) {
        DEBUG("sensor_set_pixformat return %d\n", ret);
        while(1);
    }
    
    ret = sensor_set_framesize(FRAMESIZE_HQVGA);
    if (ret < 0) {
        DEBUG("sensor_set_framesize return %d\n", ret);
        while(1);
    }

    DEBUG("Running example_find_blobs\n");
    
    list_init(&thresholds, sizeof(color_thresholds_list_lnk_data_t));
    // 红色的LAB上限、下限
    lnk_data.LMin = 25;lnk_data.LMax = 50;
    lnk_data.AMin = 40;lnk_data.AMax = 70;
    lnk_data.BMin = 20;lnk_data.BMax = 60;
    list_push_back(&thresholds, &lnk_data);
    
    while (1)
    {
        Loop_Begin(100)
        {
            //LED_Power_Ctrl(LED_BLUE, 2);
             
            ret = sensor_snapshot_simple(&img);
            if (ret != 0) 
            {
                DEBUG("sensor_snapshot_simple failed!\n");
                while(1);
            }
            roi.x = 0;roi.y = 0;
            roi.w = img.w;roi.h = img.h;

            imlib_find_blobs(&out, &img, &roi, 2, 1, &thresholds, 0, 10, 10,0, 0,NULL, NULL,NULL, NULL,NULL,NULL);
            for (size_t i = 0; list_size(&out); i++) 
            {
                find_blobs_list_lnk_data_t tmp_lnk_data;
                list_pop_front(&out, &tmp_lnk_data);
                imlib_draw_rectangle(&img,tmp_lnk_data.rect.x, tmp_lnk_data.rect.y, tmp_lnk_data.rect.w,tmp_lnk_data.rect.h, 0xffff,2,0);
            }
            ST7789VW_DrawBitLine16BPP(&img);  
        }Loop_End()
    }
}

void DMA1_Stream0_IRQHandler(void)
{
    HAL_DMA_IRQHandler(&hdma1_stream0);
}

void SPI2_IRQHandler(void)
{
    HAL_SPI_IRQHandler(&hspi2);    
}

void DCMI_IRQHandler(void) 
{
    HAL_DCMI_IRQHandler(&hdcmi);
}

void DMA2_Stream1_IRQHandler(void) 
{
    HAL_DMA_IRQHandler(&hdma2_stream1);
}

void OTG_FS_IRQHandler(void)
{
    HAL_PCD_IRQHandler(&hpcd_USB_OTG_FS);
}


/**
  * @brief  This function is executed in case of error occurrence.
  * @param  file: The file name as string.
  * @param  line: The line in file as a number.
  * @retval None
  */
void _Error_Handler(char *file, int line)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  while(1) 
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{ 
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
    ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
