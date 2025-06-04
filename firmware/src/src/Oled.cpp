

#include "Oled.h"

Oled oled;

// 屏幕缓存定义  根据不同平台更改内存位置修饰符
static uint8_t Oled_GRAM[PAGE_SIZE][WIDTH];
static uint8_t Oled_roll_x[4] = {0, WIDTH, 0, PAGE_SIZE}; // 滚动显示边界设定，依次为左上角xy和右下角xy
static char chinese_buffer[4] = {0, 0, 0, 0};
static uint8_t chinese_buffer_count = 0;
/*************************************
以下两个函数请对应移植平台进行修改
***************************************/

void write_oled(uint8_t dat, uint8_t mode) // 根据不同平台更改IIC或SPI发送函数
{
  /* ***********IIC*************************/
  Wire.beginTransmission(Oled_SLAVE); // 开始IIC并写物理地址
  if (mode)                           // 判断是指令还是数据
  {
    Wire.write(0x40);
  }
  else
  {
    Wire.write(0x00);
  }
  Wire.write(dat);        // 写数据
  Wire.endTransmission(); // 结束
  /**************************************/

  /* ***********SPI*************************/
  //  OLED_CS_CLR;
  //  if(mode){OLED_DC_SET;}else{OLED_DC_CLR;}
  //  write_oled(dat);//写数据
  //  OLED_CS_SET;
  /**************************************/
}
void Oled::begin() // IIC/SPI初始化函数，根据平台自行修改
{
#if WRITE_MODE
  {
    // IIC初始化函数  根据平台自行修改
    Wire.begin(SDA_PIN, SCL_PIN);
    Wire.setClockStretchLimit(1000000);
    Wire.setClock(1000000);
    init();
  }
#else
  {
    // SPI初始化函数 根据平台自行修改
    //    SPI_begin();
    //    OLED_RES_CLR;
    //    delay(10);
    //    OLED_RES_SET;
  }
#endif
}

/*******************************************/

void Oled::init()
{
  uint8_t i, n;
  write_oled(0xAE, OLED_CMD); // 关闭显示
#ifdef SSD1306_128_64
  {
    /**************初始化SSD1306*****************/

    write_oled(0x00, OLED_CMD); //---set low column address
    write_oled(0x10, OLED_CMD); //---set high column address
    write_oled(0x40, OLED_CMD); //--set start line address
    write_oled(0xB0, OLED_CMD); //--set page address

    write_oled(0xA1, OLED_CMD); //--Set SEG/Column Mapping     0xa0左右反置 0xa1正常
    write_oled(0xC8, OLED_CMD); // Set COM/Row Scan Direction   0xc0上下反置 0xc8正常

    write_oled(0xA8, OLED_CMD); //--set multiplex ratio(1 to 64)
    write_oled(0x3F, OLED_CMD); //--1/32 duty
                                //    write_oled(0xC8,OLED_CMD);//Com scan direction
    write_oled(0xD3, OLED_CMD); //-set display offset
    write_oled(0x00, OLED_CMD); //

    write_oled(0xD5, OLED_CMD); // set osc division
    write_oled(0x80, OLED_CMD); //

    write_oled(0xD8, OLED_CMD); // set area color mode off
    write_oled(0x05, OLED_CMD); //

    write_oled(0xD9, OLED_CMD); // Set Pre-Charge Period
    write_oled(0xF1, OLED_CMD); //

    write_oled(0xDA, OLED_CMD); // set com pin configuartion
    write_oled(0x12, OLED_CMD); //

    write_oled(0xDB, OLED_CMD); // set Vcomh
    write_oled(0x30, OLED_CMD); //

    write_oled(0x8D, OLED_CMD); // set charge pump enable
    write_oled(0x14, OLED_CMD); //

    /*******************************************/
  }
#elif defined SH1106
  {

    /**************初始化CH1106***************/
    write_oled(0x02, OLED_CMD); //---set low column address
    write_oled(0x10, OLED_CMD); //---set high column address
    write_oled(0x40, OLED_CMD); //--set start line address  Set Mapping RAM Display Start Line (0x00~0x3F)
    write_oled(0xB0, OLED_CMD);
    write_oled(0xA1, OLED_CMD); //--Set SEG/Column Mapping     0xa0左右反置 0xa1正常
                                //    write_oled(0xC8,OLED_CMD);//Set COM/Row Scan Direction   0xc0上下反置 0xc8正常
    write_oled(0xA6, OLED_CMD); //--set normal display
    write_oled(0xA8, OLED_CMD); //--set multiplex ratio(1 to 64)
    write_oled(0x3f, OLED_CMD); //--1/64 duty
    write_oled(0xAD, OLED_CMD);
    write_oled(0x8B, OLED_CMD);
    write_oled(0x33, OLED_CMD);
    write_oled(0xC8, OLED_CMD);
    write_oled(0xD3, OLED_CMD); //-set display offset Shift Mapping RAM Counter (0x00~0x3F)
    write_oled(0x00, OLED_CMD); //-not offset
    write_oled(0xd5, OLED_CMD); //--set display clock divide ratio/oscillator frequency
    write_oled(0x80, OLED_CMD); //--set divide ratio, Set Clock as 100 Frames/Sec
    write_oled(0xD9, OLED_CMD); //--set pre-charge period
    write_oled(0x1F, OLED_CMD); // Set Pre-Charge as 15 Clocks & Discharge as 1 Clock
    write_oled(0xDA, OLED_CMD); //--set com pins hardware configuration
    write_oled(0x12, OLED_CMD);
    write_oled(0xDB, OLED_CMD); //--set vcomh
    write_oled(0x40, OLED_CMD); // Set VCOM Deselect Level
    write_oled(0xAF, OLED_CMD); //--turn on oled panel
                                /*******************************************/
  }
#elif defined SSD1306_128_32
  {
    write_oled(0x40, OLED_CMD); //---set low column address
    write_oled(0xB0, OLED_CMD); //---set high column address
    write_oled(0xC8, OLED_CMD); //-not offset
    write_oled(0xa1, OLED_CMD); // 段重定向设置
    write_oled(0xa6, OLED_CMD); //
    write_oled(0xa8, OLED_CMD); // 设置驱动路数
    write_oled(0x1f, OLED_CMD);
    write_oled(0xd3, OLED_CMD);
    write_oled(0x00, OLED_CMD);
    write_oled(0xd5, OLED_CMD);
    write_oled(0xf0, OLED_CMD);
    write_oled(0xd9, OLED_CMD);
    write_oled(0x22, OLED_CMD);
    write_oled(0xda, OLED_CMD);
    write_oled(0x02, OLED_CMD);
    write_oled(0xdb, OLED_CMD);
    write_oled(0x49, OLED_CMD);
    write_oled(0x8d, OLED_CMD);
    write_oled(0x14, OLED_CMD);
  }
#endif

  write_oled(0x81, OLED_CMD);       // contract control
  write_oled(Brightness, OLED_CMD); //--128
  write_oled(0xaf, OLED_CMD);

  for (i = 0; i < PAGE_SIZE; i++)
  {
    write_oled(0xb0 + i, OLED_CMD); // 设置页地址（0~7）
#ifdef SH1106
    write_oled(0x02, OLED_CMD); // 设置显示位置—列低地址
#else
    write_oled(0x00, OLED_CMD); // 设置显示位置—列低地址
#endif
    write_oled(0x10, OLED_CMD); // 设置显示位置—列高地址
    for (n = 0; n < WIDTH; n++)
      write_oled(0, OLED_DATA);
  } // 更新显示
}
/*******************************************/

void oled_set_pos(uint8_t x, uint8_t y)
{
#ifdef SH1106
  {
    x += 2;
  }
#endif
  write_oled(YLevel + y, OLED_CMD);
  write_oled((x >> 4) | XLevelH, OLED_CMD);
  write_oled((x & 0x0f), OLED_CMD);
}

void Oled::clr()
{
  uint16_t size = PAGE_SIZE * WIDTH;
  uint8_t *p = &Oled_GRAM[0][0];
  while (size--)
    *p++ = 0;
}

void Oled::choose_clr(uint8_t x, uint8_t y, uint8_t width, uint8_t height)
{
  uint8_t i;
  for (i = 0; i < height; i++)
  {
    memset(&Oled_GRAM[y + i][x], 0, width);
  }
}

static void write_char_1(int16_t x, int16_t y, char chr, uint8_t Size)
{
  uint8_t c, i = 1, j;
  int16_t buf;
  uint8_t k, l;
  uint8_t tmp;
  uint8_t flg;
  uint8_t y_buf = Oled_roll_x[2] >> 3;
  uint8_t csize;

  c = chr - ' '; // 得到偏移后的值

  if (Size == 16)
    csize = 8;

  buf = Oled_roll_x[2] - y;
  if (buf > 15)
    return;

  if (buf > 7 && buf < 16)
  {
    y += 8;
    i = 0;
  }
  l = y >> 3;
  if (y <= Oled_roll_x[2])
  {
    k = Oled_roll_x[2] - y;
    for (j = 0; j < csize; j++)
    {
      buf = x + j;
      if (buf >= Oled_roll_x[0] && buf < Oled_roll_x[1])
      {
        if (i)
        {
          tmp = F8X16[c * 16 + j];
        }
        else
        {
          tmp = F8X16[c * 16 + j + 8];
        }
        flg = tmp >> k;

        Oled_GRAM[y_buf][buf] |= flg;

        if (i && Size == 16)
        {
          tmp = F8X16[c * 16 + j + 8];
          flg = tmp << (8 - k);

          Oled_GRAM[y_buf][buf] |= flg;

          if (Oled_roll_x[3] > 1)
          {
            flg = tmp >> k;
            Oled_GRAM[y_buf + 1][buf] |= flg;
          }
        }
      }
    }
  }
  else
  {
    k = y % 8;
    flg = 2;
    for (i = 0; i < flg; i++)
    {
      if ((l + i) >= Oled_roll_x[3])
        return;

      for (j = 0; j < csize; j++)
      {
        buf = x + j;
        if (buf >= Oled_roll_x[0] && buf < Oled_roll_x[1])
        {

          tmp = F8X16[c * 16 + j + i * 8];
          Size = tmp << k;
          Oled_GRAM[l + i][buf] |= Size;

          if (k && (l + i) < (Oled_roll_x[3] + y_buf - 1))
          {
            Size = tmp >> (8 - k);
            Oled_GRAM[l + i + 1][buf] |= Size;
          }
        }
      }
    }
  }
}

static void write_char_0(int16_t x, int16_t y, char chr, uint8_t Size)
{
  uint8_t c, i = 1, j;
  int16_t buf;
  uint8_t k, l;
  uint8_t tmp;
  uint8_t flg;
  uint8_t y_buf = Oled_roll_x[2] >> 3;
  uint8_t csize;

  c = chr - ' '; // 得到偏移后的值

  if (Size == 16)
    csize = 8;

  buf = Oled_roll_x[2] - y;
  if (buf > 15)
    return;

  if (buf > 7 && buf < 16)
  {
    y += 8;
    i = 0;
  }
  l = y >> 3;
  if (y <= Oled_roll_x[2])
  {
    k = Oled_roll_x[2] - y;
    for (j = 0; j < csize; j++)
    {
      buf = x + j;
      if (buf >= Oled_roll_x[0] && buf < Oled_roll_x[1])
      {
        if (i)
        {
          tmp = F8X16[c * 16 + j];
        }
        else
        {
          tmp = F8X16[c * 16 + j + 8];
        }
        flg = tmp >> k;

        Oled_GRAM[y_buf][buf] &= ~flg;

        if (i && Size == 16)
        {
          tmp = F8X16[c * 16 + j + 8];
          flg = tmp << (8 - k);

          Oled_GRAM[y_buf][buf] &= ~flg;

          if (Oled_roll_x[3] > 1)
          {
            flg = tmp >> k;
            Oled_GRAM[y_buf + 1][buf] &= ~flg;
          }
        }
      }
    }
  }
  else
  {
    k = y % 8;
    flg = 2;
    for (i = 0; i < flg; i++)
    {
      if ((l + i) >= Oled_roll_x[3])
        return;

      for (j = 0; j < csize; j++)
      {
        buf = x + j;
        if (buf >= Oled_roll_x[0] && buf < Oled_roll_x[1])
        {

          tmp = F8X16[c * 16 + j + i * 8];
          Size = tmp << k;
          Oled_GRAM[l + i][buf] &= ~Size;

          if (k && (l + i) < (Oled_roll_x[3] + y_buf - 1))
          {
            Size = tmp >> (8 - k);
            Oled_GRAM[l + i + 1][buf] &= ~Size;
          }
        }
      }
    }
  }
}

void Oled::write_char(int16_t x, int16_t y, char chr, uint8_t Size, uint8_t mode)
{

  if (mode == 0)
    write_char_0(x, y, chr, Size);
  else
    write_char_1(x, y, chr, Size);
}

void Oled::str(int16_t x, int16_t y, const void *chr, uint8_t Size, uint8_t mode, uint8_t fill)
{
  uint8_t csize;
  const char *p = (char *)chr;

  if (Size == 16)
  {
    csize = 8;
  }
  else
  {
    return;
  }
  chinese_buffer_count = 0;
  while (*p)
  {
    if ((y / 8) > (Oled_roll_x[2] + Oled_roll_x[3] - 1))
      return;

    if (*p >= 0x80)
    {
      chinese_buffer[chinese_buffer_count++] = *p;
      if (chinese_buffer_count == 3)
      {
        chinese_buffer_count = 0;
        chinese(x, y, (const char *)chinese_buffer, Size, mode, fill);
        x += Size;
      }
    }
    else
    {
      chinese_buffer_count = 0;
      write_char(x, y, *p, Size, mode);
      x += csize;
    }
    p++;
    if (x > Oled_roll_x[1] - 8)
    {
      if (fill)
      {
        x = Oled_roll_x[0];
        y += Size;
      }
      else
      {
        if (x > Oled_roll_x[1])
          return;
      }
    }
  }
}

void Oled::printf(int16_t x, int16_t y, uint8_t Size, uint8_t mode, uint8_t fill, const void *fmt, ...)
{
  char buf[128];
  va_list ap;
  va_start(ap, fmt);
  vsnprintf(buf, sizeof(buf), (char *)fmt, ap);
  va_end(ap);
  str(x, y, buf, Size, mode, fill);
}

void Oled::refresh()
{
  uint8_t i;
  // uint8_t *p = &Oled_GRAM[0][0];

#if WRITE_MODE
  {
    for (i = 0; i < PAGE_SIZE; i++)
    {
      oled_set_pos(0, i);
      Wire.beginTransmission(Oled_SLAVE); // 开始IIC并写物理地址
      Wire.write(0x40);
      Wire.write(&Oled_GRAM[i][0], WIDTH);
      // for (n = 0; n < WIDTH; n++)
      // {
      //   Wire.write(*p++); // 写数据
      // }
      Wire.endTransmission(1); // 结束
    } // 更新显示
  }
#else
  {
    for (i = 0; i < PAGE_SIZE; i++)
    {
      oled_set_pos(0, i);
      for (n = 0; n < WIDTH; n++)
      {
        write_oled(*p++, OLED_DATA);
      }
    } // 更新显示
  }
#endif
}

void Oled::choose_refresh(uint8_t x, uint8_t y, uint8_t width, uint8_t height)
{
  uint8_t i;
  if ((height + y) > PAGE_SIZE)
    return;
#if WRITE_MODE
  {
    for (i = 0; i < height; i++)
    {
      oled_set_pos(x, y + i);
      Wire.beginTransmission(Oled_SLAVE); // 开始IIC并写物理地址
      Wire.write(0x40);
      Wire.write(&Oled_GRAM[y + i][x], width);
      // for (n = 0; n < width; n++)
      // {
      //   Wire.write(*p++); // 写数据
      // }
      Wire.endTransmission(1); // 结束
    } // 更新显示
  }
#else
  {
    for (i = 0; i < height; i++)
    {
      oled_set_pos(x, y + i);
      p = &Oled_GRAM[y + i][x];
      for (n = 0; n < width; n++)
      {
        write_oled(*p++, OLED_DATA);
      }
    } // 更新显示
  }
#endif
}

static uint32_t mypow(uint8_t m, uint8_t n)
{
  uint32_t result = 1;
  while (n--)
    result *= m;
  return result;
}

uint8_t Oled::num(int16_t x, int16_t y, long num, uint8_t len, uint8_t Size, uint8_t align, uint8_t mode)
{
  uint8_t t;
  uint8_t enshow = 0, csize;
  uint8_t count = 0;
  uint8_t low_zero = 0;
  uint8_t dat_buf[10];
  uint8_t tmp = 0;
  //  int16_t rem_x=x;
  if (Size == 16)
  {
    csize = 8;
  }
  else if (Size == 8)
  {
    csize = 6;
  }
  else
  {
    return 0;
  }

  if (num < 0)
  {
    low_zero = 1;
    num = -num;
  }

  for (t = 0; t < len; t++)
  {
    dat_buf[t] = (num / mypow(10, len - t - 1)) % 10;
    if (align == RIGHT && dat_buf[t] && !tmp)
      tmp = len - t;
  }
  if (!tmp)
    tmp = 1;

  if (align == RIGHT)
  {
    x -= tmp * csize;
    if (low_zero)
      x -= csize;
  }

  if (low_zero)
  {
    write_char(x, y, '-', Size, mode);
    x += csize;
    count++;
  }

  for (t = 0; t < len; t++)
  {
    if (enshow == 0 && t < (len - 1))
    {
      if (dat_buf[t] == 0)
      {
        continue;
      }
      else
      {
        enshow = 1;
      }
    }

    write_char(x, y, dat_buf[t] + '0', Size, mode);
    x += csize;
    count++;
  }
  return count;
}

static uint8_t OLED_ShowFont_0(int16_t x, int16_t y, const char *s, uint8_t Size)
{
  uint8_t i, j, k, tmp;
  uint8_t h_buf = Size / 8;
  uint16_t num;
  const char *font;
  uint8_t y_buf;
  uint8_t font_buf;

  if (Size == 16)
  {
    num = sizeof(cfont16) / sizeof(typFNT_GB16);
    font_buf = 36;
  }
  else
    return 0;

  if (y < Oled_roll_x[2])
  {
    if (y + Size < Oled_roll_x[2])
      return 0;
    else
    {
      i = (Oled_roll_x[2] - y) / 8;
      h_buf -= i;
      y += i << 3;
    }
  }
  if ((y / 8) > (Oled_roll_x[3] + Oled_roll_x[2] - 1))
    return 0;

  for (i = 0; i < num; i++)
  {
    font = cfont16[i].Index;
    if (*font == *s && *(font + 1) == *(s + 1) && *(font + 2) == *(s + 2))
    {
      font = cfont16[i].Msk;
      font += (Size / 8 - h_buf) * Size;

      if (y < Oled_roll_x[2])
      {
        y_buf = Oled_roll_x[2] - y;
        for (j = 0; j < Size; j++)
        {
          tmp = *font++;
          tmp >>= y_buf;
          if ((x + j) >= Oled_roll_x[0] && (x + j) < Oled_roll_x[1])
          {
            Oled_GRAM[Oled_roll_x[2]][x + j] &= ~tmp;
          }
        }
        y += 8;
        h_buf--;
      }

      y_buf = y % 8;
      y >>= 3;

      for (k = 0; k < h_buf; k++)
      {
        if ((y + k) > (Oled_roll_x[3] + Oled_roll_x[2] - 1))
          return 1;
        for (j = 0; j < Size; j++)
        {

          tmp = *font++;
          if ((x + j) >= Oled_roll_x[0] && (x + j) < Oled_roll_x[1])
          {
            font_buf = tmp << y_buf;
            Oled_GRAM[y + k][x + j] &= ~font_buf;
            if (y_buf && (y + k + 1) < (Oled_roll_x[3] + Oled_roll_x[2]))
            {
              font_buf = tmp >> (8 - y_buf);
              Oled_GRAM[y + k + 1][x + j] &= ~font_buf;
            }
          }
        }
      }
      return 1;
    }
    font += font_buf;
  }
  return 0;
}

static uint8_t OLED_ShowFont_1(int16_t x, int16_t y, const char *s, uint8_t Size)
{
  uint8_t i, j, k, tmp;
  uint8_t h_buf = Size / 8;
  uint16_t num;
  const char *font;
  uint8_t y_buf;
  uint8_t font_buf;

  if (Size == 16)
  {
    num = sizeof(cfont16) / sizeof(typFNT_GB16);
    font_buf = 36;
  }
  else
    return 0;

  if (y < Oled_roll_x[2])
  {
    if (y + Size < Oled_roll_x[2])
      return 0;
    else
    {
      i = (Oled_roll_x[2] - y) / 8;
      h_buf -= i;
      y += i << 3;
    }
  }
  if ((y / 8) > (Oled_roll_x[3] + Oled_roll_x[2] - 1))
    return 0;

  for (i = 0; i < num; i++)
  {
    font = cfont16[i].Index;
    if (*font == *s && *(font + 1) == *(s + 1) && *(font + 2) == *(s + 2))
    {
      font = cfont16[i].Msk;
      font += (Size / 8 - h_buf) * Size;

      if (y < Oled_roll_x[2])
      {
        y_buf = Oled_roll_x[2] - y;
        for (j = 0; j < Size; j++)
        {
          tmp = *font++;
          tmp >>= y_buf;
          if ((x + j) >= Oled_roll_x[0] && (x + j) < Oled_roll_x[1])
          {
            Oled_GRAM[Oled_roll_x[2]][x + j] |= tmp;
          }
        }
        y += 8;
        h_buf--;
      }

      y_buf = y % 8;
      y >>= 3;

      for (k = 0; k < h_buf; k++)
      {
        if ((y + k) > (Oled_roll_x[3] + Oled_roll_x[2] - 1))
          return 1;
        for (j = 0; j < Size; j++)
        {

          tmp = *font++;
          if ((x + j) >= Oled_roll_x[0] && (x + j) < Oled_roll_x[1])
          {
            font_buf = tmp << y_buf;
            Oled_GRAM[y + k][x + j] |= font_buf;
            if (y_buf && (y + k + 1) < (Oled_roll_x[3] + Oled_roll_x[2]))
            {
              font_buf = tmp >> (8 - y_buf);
              Oled_GRAM[y + k + 1][x + j] |= font_buf;
            }
          }
        }
      }
      return 1;
    }
    // font += font_buf;
  }
  return 0;
}

uint8_t OLED_ShowFont(int16_t x, int16_t y, const char *s, uint8_t Size, uint8_t mode)
{
  if (mode == 1)
    return OLED_ShowFont_1(x, y, s, Size);
  else
    return OLED_ShowFont_0(x, y, s, Size);
}

void Oled::chinese(int16_t x, int16_t y, const void *str, uint8_t hsize, uint8_t mode, uint8_t fill)
{
  const char *p = (char *)str;

  while (*p)
  {
    if (OLED_ShowFont(x, y, p, hsize, mode))
      x += hsize;
    if (x > (Oled_roll_x[1] - hsize))
    {
      if (fill)
      {
        x = Oled_roll_x[0];
        y += hsize;
        if (y > (WIDTH - 1))
          return;
      }
      else
      {
        if (x > Oled_roll_x[1])
          return;
      }
    }
    p += 3;
  }
}

void Oled::point(uint8_t x, uint8_t y, uint8_t mode) // x 0-127  y 0-63
{
  uint8_t i;
  if (x < 0 || x >= WIDTH || y < 0 || y >= HEIGHT)
    return;
  i = y % 8;
  y >>= 3;
  if (mode)
  {
    Oled_GRAM[y][x] |= 0x01 << i;
  }
  else
  {
    Oled_GRAM[y][x] &= ~(0x01 << i);
  }
}

void Oled::line(uint8_t x, uint8_t y, uint8_t width, uint8_t height, uint8_t mode)
{
  uint8_t i, j;
  if (x > WIDTH - 1)
    return;
  if (x + width > WIDTH)
    width = WIDTH - x;
  for (i = 0; i < height; i++)
  {
    for (j = 0; j < width; j++)
      point(x + j, y + i, mode);
  }
}

static void BMP_0(int16_t x, int16_t y, int16_t width, int16_t height, const void *p)
{
  uint8_t i, j;
  uint8_t k, l;
  uint8_t tmp;
  uint8_t num;
  int16_t buf;
  uint8_t y_buf = Oled_roll_x[2] / 8;
  const char *str = (const char *)p;

  if (y < (Oled_roll_x[2] - 8))
  {
    buf = (Oled_roll_x[2] - y) / 8;
    str += buf * width;
    height -= buf * 8;
    if (height < 1)
      return;
    y += buf * 8;
  }

  if (y < Oled_roll_x[2])
  {
    k = Oled_roll_x[2] - y;
    for (j = 0; j < width; j++)
    {
      buf = x + j;
      if (buf >= Oled_roll_x[0] && buf < Oled_roll_x[1])
      {
        tmp = ~(*str >> k);
        Oled_GRAM[y_buf][buf] &= tmp;
      }
      str++;
    }
    y += 8;
    height -= 8;
    if (height < 1)
      return;
  }
  k = y % 8;
  l = y >> 3;
  num = ((height - 1) >> 3) + 1;
  for (i = 0; i < num; i++)
  {
    if ((l + i) > (Oled_roll_x[3]) + y_buf - 1)
      return;

    for (j = 0; j < width; j++)
    {
      buf = x + j;
      if (buf >= Oled_roll_x[0] && buf < Oled_roll_x[1])
      {
        tmp = ~(*str << k);
        Oled_GRAM[l + i][buf] &= tmp;

        if (k && (l + i) < ((Oled_roll_x[3]) + y_buf - 1))
        {
          tmp = ~(*str >> (8 - k));
          Oled_GRAM[l + i + 1][buf] &= tmp;
        }
      }
      str++;
    }
  }
}

static void BMP_1(int16_t x, int16_t y, int16_t width, int16_t height, const void *p)
{
  uint8_t i, j;
  uint8_t k, l;
  uint8_t tmp;
  uint8_t num;
  int16_t buf;
  uint8_t y_buf = Oled_roll_x[2] / 8;
  const char *str = (const char *)p;

  if (y < (Oled_roll_x[2] - 8))
  {
    buf = (Oled_roll_x[2] - y) / 8;
    str += buf * width;
    height -= buf * 8;
    if (height < 1)
      return;
    y += buf * 8;
  }

  if (y < Oled_roll_x[2])
  {
    k = Oled_roll_x[2] - y;
    for (j = 0; j < width; j++)
    {
      buf = x + j;
      if (buf >= Oled_roll_x[0] && buf < Oled_roll_x[1])
      {
        tmp = *str >> k;
        Oled_GRAM[y_buf][buf] |= tmp;
      }
      str++;
    }
    y += 8;
    height -= 8;
    if (height < 1)
      return;
  }
  k = y % 8;
  l = y >> 3;
  num = ((height - 1) >> 3) + 1;
  for (i = 0; i < num; i++)
  {
    if ((l + i) > (Oled_roll_x[3]) + y_buf - 1)
      return;

    for (j = 0; j < width; j++)
    {
      buf = x + j;
      if (buf >= Oled_roll_x[0] && buf < Oled_roll_x[1])
      {
        tmp = *str << k;
        Oled_GRAM[l + i][buf] |= tmp;

        if (k && (l + i) < ((Oled_roll_x[3]) + y_buf - 1))
        {
          tmp = *str >> (8 - k);
          Oled_GRAM[l + i + 1][buf] |= tmp;
        }
      }
      str++;
    }
  }
}

void Oled::BMP(int16_t x, int16_t y, int16_t width, int16_t height, const void *p, uint8_t mode)
{
  if (mode == 1)
    BMP_1(x, y, width, height, p);
  else
    BMP_0(x, y, width, height, p);
}

void Oled::BMP(int16_t x, int16_t y, const void *p, uint8_t mode)
{
  const char *str = (char *)p;
  BMP(x, y, str[0], str[1], str + 2, mode);
}
void Oled::BMP(int16_t y, const void *p)
{
  const char *str = (char *)p;
  BMP(str[0], str[1] + y, str[2], str[3], str + 4, 1);
}

void Oled::BMP(int16_t x, int16_t y, const void *p)
{
  const char *str = (char *)p;
  BMP(x, y, str[0], str[1], str + 2, 1);
}

void Oled::light(uint8_t light)
{
  write_oled(0x81, OLED_CMD);  // contract control
  write_oled(light, OLED_CMD); //
}

void Oled::xy_set(uint8_t x, uint8_t y, uint8_t xx, uint8_t yy)
{
  Oled_roll_x[0] = x;
  Oled_roll_x[1] = xx;
  Oled_roll_x[2] = y << 3;
  if ((y + yy) > PAGE_SIZE)
    Oled_roll_x[3] = PAGE_SIZE - y;
  else
    Oled_roll_x[3] = yy;
}

void Oled::roll(uint8_t x, uint8_t y, uint8_t width, uint8_t height, uint8_t cont, uint8_t roll_mode, uint8_t roll_cont)
{
  signed char i;
  uint8_t j;
  uint8_t tmp;
  int16_t buf;
  uint8_t flg;
  //
  tmp = 8 - cont;

  while (roll_cont--)
  {
    if (roll_mode == UP)
    {
      for (i = 0; i < height; i++)
      {
        buf = y + i;
        if ((buf + 1) < (height + y))
          flg = 1;
        else
          flg = 0;
        for (j = 0; j < width; j++)
        {
          Oled_GRAM[buf][x + j] >>= cont;
          if (flg)
            Oled_GRAM[buf][x + j] |= Oled_GRAM[buf + 1][x + j] << tmp;
        }
      }
    }
    else if (roll_mode == DOWN)
    {
      for (i = height - 1; i > -1; i--)
      {
        buf = y + i;
        if (buf > y)
          flg = 1;
        else
          flg = 0;
        for (j = 0; j < width; j++)
        {
          Oled_GRAM[buf][x + j] <<= cont;
          if (flg)
            Oled_GRAM[buf][x + j] |= Oled_GRAM[buf - 1][x + j] >> tmp;
        }
      }
    }
    else if (roll_mode == LEFT)
    {
      tmp = x + width - 1;
      for (flg = 0; flg < height; flg++)
      {
        buf = x + cont;
        j = x;
        for (;;)
        {
          if (buf > tmp)
            break;
          Oled_GRAM[y + flg][j++] = Oled_GRAM[y + flg][buf];
          Oled_GRAM[y + flg][buf++] = 0;
        }
      }
    }
    else if (roll_mode == RIGHT)
    {
      for (flg = 0; flg < height; flg++)
      {
        buf = x + width - cont - 1;
        j = x + width - 1;
        for (;;)
        {
          if (buf < x)
            break;
          Oled_GRAM[y + flg][j--] = Oled_GRAM[y + flg][buf];
          Oled_GRAM[y + flg][buf--] = 0;
        }
      }
    }
    choose_refresh(x, y, width, height);
    yield();
  }
}

void Oled::display_on(void)
{
  write_oled(0X8D, OLED_CMD); // SET DCDC命令
  write_oled(0X14, OLED_CMD); // DCDC ON
  write_oled(0XAF, OLED_CMD); // DISPLAY ON
}
// 关闭OLED显示
void Oled::display_off(void)
{
  write_oled(0X8D, OLED_CMD); // SET DCDC命令
  write_oled(0X10, OLED_CMD); // DCDC OFF
  write_oled(0XAE, OLED_CMD); // DISPLAY OFF
}
