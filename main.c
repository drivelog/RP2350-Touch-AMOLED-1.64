#include "DEV_Config.h"
#include "AMOLED_1in64.h"
#include "qspi_pio.h"
#include "QMI8658.h"
#include "FT3168.h"
//#include "image.h"
#include "GUI_Paint.h"

unsigned char const *PIC;
int flag=0;
uint8_t i2c_lock = 0;
#define I2C_LOCK() i2c_lock = 1
#define I2C_UNLOCK() i2c_lock = 0

void Touch_INT_callback(uint gpio, uint32_t events);

bool timer_callback(struct repeating_timer *t) {
    if(i2c_lock) return true; 
    if(FT3168.mode != FT3168_Point_Mode)
    {
        uint8_t gesture = FT3168_Get_Gesture();
            
        if (gesture == FT3168_Gesture_Double_Click)
        {
            flag = 1;
        }
    }
    else
    {
        static int y_point_old,x_point_old;
        FT3168_Get_Point();
        if(FT3168.x_point != x_point_old || FT3168.y_point != y_point_old)
        {
            flag = 1;
        }
        y_point_old = FT3168.y_point;
        x_point_old = FT3168.x_point;
    }
    return true; 
}

int main() 
{
    if(DEV_Module_Init()!=0){
        return -1;
    }
    
    /*QSPI PIO Init*/
    QSPI_GPIO_Init(qspi);
    QSPI_PIO_Init(qspi);
    QSPI_4Wire_Mode(&qspi);

    AMOLED_1IN64_Init();
    AMOLED_1IN64_SetBrightness(100);
    AMOLED_1IN64_Clear(WHITE);
    
    UDOUBLE Imagesize = AMOLED_1IN64_HEIGHT*AMOLED_1IN64_WIDTH*2;
    UWORD *BlackImage;
    if((BlackImage = (UWORD *)malloc(Imagesize)) == NULL) {
        printf("Failed to apply for black memory...\r\n");
        exit(0);
    }

    /*1.Create a new image cache named IMAGE_RGB and fill it with white*/
    Paint_NewImage((UBYTE *)BlackImage, AMOLED_1IN64.WIDTH, AMOLED_1IN64.HEIGHT, 0, WHITE);
    Paint_SetScale(65);
    Paint_SetRotate(ROTATE_0);
    Paint_Clear(WHITE);
    AMOLED_1IN64_Display(BlackImage);

    /* GUI */
    printf("drawing...\r\n");
    /*2.Drawing on the image*/


#if 1
    /*5.Display Pi with Font_XL*/
    Paint_Clear(BLACK);
    Paint_SetRotate(ROTATE_270);
    Paint_DrawString_XL(10, 10, "3.141593", CYAN);
    AMOLED_1IN64_Display(BlackImage);
    DEV_Delay_ms(2000);
#endif



 /* Module Exit */
     free(BlackImage);
     BlackImage = NULL;
     
     DEV_Module_Exit();
}
