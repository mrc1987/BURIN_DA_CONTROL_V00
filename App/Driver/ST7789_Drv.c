#define LCD_REST_LOW()   HAL_GPIO_WritePin(LCD_RST_GPIO_Port, LCD_RST_Pin, GPIO_PIN_RESET);
#define LCD_REST_HIGH()  HAL_GPIO_WritePin(LCD_RST_GPIO_Port, LCD_RST_Pin, GPIO_PIN_SET);

#define LCD_DC_LOW()     HAL_GPIO_WritePin(LCD_DC_GPIO_Port, LCD_DC_Pin, GPIO_PIN_RESET);
#define LCD_DC_HIGH()    HAL_GPIO_WritePin(LCD_DC_GPIO_Port, LCD_DC_Pin, GPIO_PIN_SET);

#define LCD_CS_LOW()     HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, GPIO_PIN_RESET);
#define LCD_CS_HIGH()    HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, GPIO_PIN_SET);

#define LCD_WR_RS(n)     (n?HAL_GPIO_WritePin(LCD_DC_GPIO_Port,LCD_DC_Pin,GPIO_PIN_SET):HAL_GPIO_WritePin(LCD_DC_GPIO_Port,LCD_DC_Pin,GPIO_PIN_RESET))
 
 
 //常用颜色定义，为RGB565格式,其他颜色可以自己改变配比

/************** 颜色(RGB 5,6,5) **************/
#define LCD_DISP_RED                    0xF800       //先高字节，后低字节      
#define LCD_DISP_GREEN                  0x07E0
#define LCD_DISP_BLUE                   0x001F
#define LCD_DISP_WRITE                  0xFFFF
#define LCD_DISP_BLACK                  0x0000
#define LCD_DISP_GRAY                   0xEF5D
#define LCD_DISP_GRAY75                 0x39E7
#define LCD_DISP_GRAY50                 0x7BEF
#define LCD_DISP_GRAY25                 0xADB5
 
 /**
 * @brief  写16位的数据（两个字节）到LCD
 * @param   dat —— 需要发送的16bit数据
 * @return  none
 */
void LCD_Write_2Bytes(const uint16_t dat)
{
    uint8_t data[2] = {0};

    data[0] = dat >> 8;
    LCD_data_write(data[0]);
    data[1] = dat;
    LCD_data_write(data[1]);

    // LCD_SPI_Send(data, 2);
}

void LCD_command_write(uint8_t command)
{ // 写命令
    LCD_DC_LOW();
    LCD_CS_LOW();
    lcd_SendBytes(command);
    LCD_CS_HIGH();
}
void LCD_data_write(uint8_t data)
{ //写数据
    LCD_DC_HIGH();
    LCD_CS_LOW();
    lcd_SendBytes(data);
    LCD_CS_HIGH();
}

void delay_lcd(unsigned int time)
{
    while (time)
    {
        time--;
        time--;
        time--;
        time--;
    }
}

void LCD_Reset(void)
{
    LCD_REST_HIGH();
    delay_lcd(1000);
    LCD_REST_LOW();
    delay_lcd(10000);
    LCD_REST_HIGH();
    delay_lcd(10000);
    LCD_CS_HIGH();
}
//LCD行列设置

//========================================================
//CASET (2Ah): Column Address Set
//RASET (2Bh): Row Address Set
//x1,y1:起始地址,x2,y2:终点地址
//========================================================
void lcd_set_address(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2)
{
    LCD_command_write(0x2a);
    LCD_data_write(x1 >> 8);
    LCD_data_write(x1);
    LCD_data_write(x2 >> 8);
    LCD_data_write(x2);

    LCD_command_write(0x2b);
    LCD_data_write(y1 >> 8);
    LCD_data_write(y1);
    LCD_data_write(y2 >> 8);
    LCD_data_write(y2);

    LCD_command_write(0x2C); // write_ram
}

//LCD初始化代码，这部分主要是参考网上已经完成的，相关配置可以在手册具体查看

void lcd_init(void)
{

    LCD_CS_HIGH();
    LCD_Reset();

    /************* display Setting *************/
    LCD_command_write(0x01);
    delay_lcd(10000);
    LCD_command_write(LCD_CMD_SLPOUT);
    delay_lcd(10000);

  //修改此处，可以改变屏幕的显示方向，横屏，竖屏等
    LCD_command_write(LCD_CMD_MADCTL); // 0x36  ///< Memory Data Access Control
LCD_data_write(0x00);              //50//40//C0

LCD_command_write(LCD_CMD_COLMOD); //  0x3A     ///< Interface Pixel Format
    LCD_data_write(0x05);              /// 16bit/pixel

    /*********** Frame Rate Setting ***********/
    LCD_command_write(LCD_CMD_PORCTRL); //0xB2     ///< Porch Setting
    LCD_data_write(0x0c);               ///< Back porch in normal mode
    LCD_data_write(0x0c);               ///< Front porch in normal mode
    LCD_data_write(0x00);               ///< Disable separate porch control
    LCD_data_write(0x33);               ///< Back and front porch in idle mode
    LCD_data_write(0x33);               ///< Back and front porch in partial mode

    LCD_command_write(LCD_CMD_GCTRL); //  0xB7     ///< Gate Control
    LCD_data_write(0x72);             ///< VGH:13.26, VGL:-10.43

    LCD_command_write(LCD_CMD_VCOMS); // 0xBB     ///< VCOM Setting
    LCD_data_write(0x3d);             ///< VCOM: 1.425

    LCD_command_write(LCD_CMD_LCMCTRL); ///0xC0     ///< LCM Control
    LCD_data_write(0x2c);

    LCD_command_write(LCD_CMD_VDVVRHEN); //0xC2     ///< VDV and VRH Command Enable
    LCD_data_write(0x01);
    LCD_data_write(0xFF);

    /* VRH Set */
    LCD_command_write(LCD_CMD_VRHS); //0xC3
    LCD_data_write(0x19);

    /* VDV Set */
    LCD_command_write(LCD_CMD_VDVS); //0xC4
    LCD_data_write(0x20);

    LCD_command_write(LCD_CMD_FRCTRL2); //  0xC6     ///< Frame Rate Control in Normal Mode
    LCD_data_write(0x0f);               ///< 60Hz

    LCD_command_write(LCD_CMD_PWCTRL1); // 0xD0     ///< Power Control 1
    LCD_data_write(0xa4);
    LCD_data_write(0xa1); ///< AVDD:6.8V, AVCL:-4.8V, VDDS:2.3V

    /* Positive Voltage Gamma Control */
    LCD_command_write(0xE0);
    LCD_data_write(0xD0);
    LCD_data_write(0x04);
    LCD_data_write(0x0D);
    LCD_data_write(0x11);
    LCD_data_write(0x13);
    LCD_data_write(0x2B);
    LCD_data_write(0x3F);
    LCD_data_write(0x54);
    LCD_data_write(0x4C);
    LCD_data_write(0x18);
    LCD_data_write(0x0D);
    LCD_data_write(0x0B);
    LCD_data_write(0x1F);
    LCD_data_write(0x23);

    /* Negative Voltage Gamma Control */
    LCD_command_write(0xE1);
    LCD_data_write(0xD0);
    LCD_data_write(0x04);
    LCD_data_write(0x0C);
    LCD_data_write(0x11);
    LCD_data_write(0x13);
    LCD_data_write(0x2C);
    LCD_data_write(0x3F);
    LCD_data_write(0x44);
    LCD_data_write(0x51);
    LCD_data_write(0x2F);
    LCD_data_write(0x1F);
    LCD_data_write(0x1F);
    LCD_data_write(0x20);
    LCD_data_write(0x23);

    // LCD_command_write( 0x21);     ///< Display Inversion On
    //LCD_command_write( 0x13);
    //LCD_InitSequence();
    //Display_logo();
    // lcd_clear(LCD_DISP_RED);
    lcd_clear(LCD_DISP_WRITE);
    LCD_command_write(LCD_CMD_DISPON); ///< Display on
}



//LCD清屏，可以填充指定的颜色

//========================================================
//LCD清屏  color:要写入的颜色
//先关显示，等所有的数据写入到RAM后，再打开显示
//========================================================
void lcd_clear(uint16_t color)
{

    uint16_t i, j;
    LCD_command_write(0x2A);
    LCD_data_write(0);
    LCD_data_write(0);
    LCD_data_write(0);
    LCD_data_write(240);
    LCD_command_write(0X2B);
    LCD_data_write(0);
    LCD_data_write(0);
    LCD_data_write(0X01);
    LCD_data_write(0X40);

    LCD_command_write(0X2C);

    for (i = 0; i < 240; i++)
{
for (j = 0; j < 320; j++)
        {
            LCD_data_write(color >> 8);
            LCD_data_write(color);
        }
    }
    lcd_display_on(); /* 开LCD显示 */
}
 