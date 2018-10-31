#include <Wire.h>
#include <LiquidCrystal_I2C.h>   
LiquidCrystal_I2C lcd(0x27,16,2); 
#define key 4
int zhuangtai=0;
int dizhi=0 ;
struct PARAMS 
{
    
    long P1_0;  //显示PM1.0
    long _P1_0;
    long P2_5;  //显示PM2.5
    long _P2_5;
    long P10;   //显示PM10
    long _P10;  
    long _K1;    //颗粒物浓度
    long K1;
    long _K2;
    long K2;
    long _K3;
    long K3;
    long _K4;
    long K4;
    long _K5;
    long K5;
    long _K6;
    long K6;
    long jiaquan;
    long wendu;
    long shidu;
    long _jiaquan;
    long _wendu;
    long _shidu;
    //long _K6;
    //long K6;
} _params;         //结构体
void setup()
{   pinMode(key,INPUT_PULLUP);
       lcd.init();                      // initialize the lcd 
     lcd.init();                
    lcd.backlight();
    lcd.setCursor(1,0);
    lcd.print("    Welcome!");  //屏幕设置
    delay(1500);
    lcd.setCursor(2,0);
    lcd.print("HCHO Detector");   
    delay(1500);  
    Serial.begin(9600);        //串口   
}

//传感器 相关变量
static unsigned char ucRxBuffer[250];
static unsigned char ucRxCnt = 0;


//循环计数器
unsigned char loopCnt = 0;



//获取PM2.5的值
void getPM25(unsigned char ucData) 
{
    ucRxBuffer[ucRxCnt++] = ucData;

    if (ucRxBuffer[0] != 0x42 && ucRxBuffer[1] != 0x4D)  
    {
        ucRxCnt = 0;
    }
    if (ucRxCnt > 31) 
    {
_params._P1_0 = (float)ucRxBuffer[10] * 256 + (float)ucRxBuffer[11]; //pm1.0
_params._P2_5 = (float)ucRxBuffer[12] * 256 + (float)ucRxBuffer[13]; //pm2.5
_params._P10 = (float)ucRxBuffer[14] * 256 + (float)ucRxBuffer[15]; //pm10
_params._K1 = (float)ucRxBuffer[16] * 256 + (float)ucRxBuffer[17];//0.3颗粒物
_params._K2  = (float)ucRxBuffer[18] * 256 + (float)ucRxBuffer[19]; //0.5颗粒物
_params._K3  = (float)ucRxBuffer[20] * 256 + (float)ucRxBuffer[21]; //1.0颗粒物
_params._K4  = (float)ucRxBuffer[22] * 256 + (float)ucRxBuffer[23]; //2.5颗粒物
_params._K5  = (float)ucRxBuffer[24] * 256 + (float)ucRxBuffer[25]; //5.0颗粒物
_params._K6  = (float)ucRxBuffer[26] * 256 + (float)ucRxBuffer[27]; //10颗粒物
_params._jiaquan = (float)ucRxBuffer[28] * 256 + (float)ucRxBuffer[29]; //甲醛值
_params._wendu  = (float)ucRxBuffer[30] * 256 + (float)ucRxBuffer[31];//温度
_params._shidu  = (float)ucRxBuffer[32] * 256 + (float)ucRxBuffer[33];//湿度
ucRxCnt = 0;
}
}
#define FILTER_N 5
int Filter(long p) //滤波
{             //函数Filter对数据进行处理
    int i;
    int filter_sum = 0;
    int filter_max, filter_min;
    int filter_buf[FILTER_N];
    for (i = 0; i < FILTER_N; i++) 
    {
        filter_buf[i] = p;     //处理的数据是_params._P
        delay(1);
    }
    filter_max = filter_buf[0];
    filter_min = filter_buf[0];
    filter_sum = filter_buf[0];
    for (i = FILTER_N - 1; i > 0; i--) 
    {
        if (filter_buf[i] > filter_max)
            filter_max = filter_buf[i];
        else if (filter_buf[i] < filter_min)
            filter_min = filter_buf[i];
        filter_sum = filter_sum + filter_buf[i];
        filter_buf[i] = filter_buf[i - 1];
    }
    i = FILTER_N - 2;
    filter_sum = filter_sum - filter_max - filter_min + i / 2; // +i/2 的目的是为了四舍五入
    filter_sum = filter_sum / i;
    return filter_sum;
}


void loop() 
{ zhuangtai = digitalRead(key);
  if (zhuangtai ==LOW)
{ dizhi=dizhi+1;}
 else {dizhi=dizhi;}
 delay(200);
 while(dizhi==5)
 {dizhi=1;}
    while (Serial.available())   
    {
        getPM25(Serial.read());     //通过函数获取数据
    }
    if (loopCnt % 50 == 0) 
    {
    
        _params.P1_0 = Filter(_params._P1_0);        //通过Filter函数滤波处理数据赋给_params.P
      _params.P2_5 = Filter(_params._P2_5);        
        _params.P10 = Filter(_params._P10);
      //  _params.K1 = Filter(_params._K1);    
       // _params.K2 = Filter(_params._K2);  
       // _params.K3 = Filter(_params._K3);  
      //  _params.K4 = Filter(_params._K4);  
       //_params.K5 = Filter(_params._K5);  
    // _params.K6 = Filter(_params._K6); 
     _params._jiaquan = Filter(_params._jiaquan); 
     _params._wendu = Filter(_params._wendu);
     _params._shidu = Filter(_params._shidu);
    // if (zhuangtai != lastzhuangtai) 
    
      }
    
        //1602显示
        //lcd.begin();                  
        //lcd.backlight();
        lcd.setCursor(2,1);
        lcd.setCursor(2,1); 
if (dizhi==0){lcd.print("PUSH BUTTON");   } //打印jq数值 String("") + "jq:"+_params.P10+ " ug/m3 "
else if (dizhi==2){lcd.print( String("")+"PM1.0:"+_params._P1_0+ "mg/m3 ");}
else if (dizhi==3){lcd.print( String("")+"PM2.5:"+_params._P2_5+ "mg/m3 ");}
else if (dizhi==4){lcd.print( String("")+"PM10:"+_params._P10+ "mg/m3 ");}
//else if (dizhi==4){lcd.print( String("") + "KLW0.3:"+_params._K1);}
//else if (dizhi==5){lcd.print( String("") + " klw0.5:"+_params._K2);}
//else if (dizhi==6){lcd.print( String("") + " klw1.0:"+_params._K3);}
//else if (dizhi==7){lcd.print( String("") + " klw2.5:"+_params._K4);}
//else if (dizhi==8){lcd.print( String("") + " KLW5.0:"+_params._K5);}
//else if (dizhi==9){lcd.print( String("") + " KLW10.0:"+_params._K6);}
else if( dizhi==1){lcd.print(String("")+"HCHO:"+_params._jiaquan+ "ug/m3 ");}
else if (dizhi==6){lcd.print(String("")+"WENDU:"+_params._wendu/10+ "℃ ");}
else if( dizhi==5){lcd.print(String("")+"SHIDU:"+_params._shidu/10+ "RH ");}
        
        //1602显示
 

   if (++loopCnt > 99) 
    {
        loopCnt = 0;
    }
/*
    //串口显示-------------------------------------------------------------
    //Serial.println(_params.P1_0);
   // Serial.println(_params.P2_5);
    //Serial.println(_params.P10);
    //Serial.println(_params.K1);
   // Serial.println(_params.K2);
    //Serial.println(_params.K3);
   // Serial.println(_params.K4);
    //Serial.println(_params.K5);
    //Serial.println(_params.K6);
    //串口显示-------------------------------------------------------------*/
}

