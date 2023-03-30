#include <Wire.h>
#include <MPU6050.h>


MPU6050 mpu;
int bandera=0, contador=0, movimientoAccel=0, movimientoAccelX=0, movimientoAccelY=0, movimientoAccelZ=0, movimientoProx=0, cambioHumed, proximidad, humedad;
float acumuladorX=0, acumuladorY=0, acumuladorZ=0, acumuladorProx=0, acumuladorHumed=0, calibracionX=0, calibracionY=0, calibracionZ=0, calibracionProx=0, calibracionHumed=0;
char riesgo='a';

void setup() 
{
  Serial.begin(115200);

  //Serial.println("Initialize MPU6050");

  while(!mpu.begin(MPU6050_SCALE_2000DPS, MPU6050_RANGE_2G))
  {
    //Serial.println("Could not find a valid MPU6050 sensor, check wiring!");
    delay(500);
  }
    
  checkSettings();
}

void checkSettings()
{
  //Serial.println();
  
  //Serial.print(" * Sleep Mode:            ");
  //Serial.println(mpu.getSleepEnabled() ? "Enabled" : "Disabled");
  
  //Serial.print(" * Clock Source:          ");
  switch(mpu.getClockSource())
  {
    case MPU6050_CLOCK_KEEP_RESET:     //Serial.println("Stops the clock and keeps the timing generator in reset"); break;
    case MPU6050_CLOCK_EXTERNAL_19MHZ: //Serial.println("PLL with external 19.2MHz reference"); break;
    case MPU6050_CLOCK_EXTERNAL_32KHZ: //Serial.println("PLL with external 32.768kHz reference"); break;
    case MPU6050_CLOCK_PLL_ZGYRO:      //Serial.println("PLL with Z axis gyroscope reference"); break;
    case MPU6050_CLOCK_PLL_YGYRO:      //Serial.println("PLL with Y axis gyroscope reference"); break;
    case MPU6050_CLOCK_PLL_XGYRO:      //Serial.println("PLL with X axis gyroscope reference"); break;
    case MPU6050_CLOCK_INTERNAL_8MHZ:  Serial.println("Internal 8MHz oscillator"); break;
  }
  
  //Serial.print(" * Accelerometer:         ");
  switch(mpu.getRange())
  {
    case MPU6050_RANGE_16G:            //Serial.println("+/- 16 g"); break;
    case MPU6050_RANGE_8G:             //Serial.println("+/- 8 g"); break;
    case MPU6050_RANGE_4G:             //Serial.println("+/- 4 g"); break;
    case MPU6050_RANGE_2G:             Serial.println("+/- 2 g"); break;
  }  

  //Serial.print(" * Accelerometer offsets: ");
  //Serial.print(mpu.getAccelOffsetX());
  //Serial.print(" / ");
  //Serial.print(mpu.getAccelOffsetY());
  //Serial.print(" / ");
  //Serial.println(mpu.getAccelOffsetZ());
  
  //Serial.println();
}

void loop(){
  
  Vector normAccel = mpu.readNormalizeAccel();
  proximidad=analogRead(A0);
  humedad=analogRead(A1);
  
  if(millis()>=5000&&bandera==0&&contador<30){
    
    calibracionX+=normAccel.XAxis;
    calibracionY+=normAccel.YAxis;
    calibracionZ+=normAccel.ZAxis;
    calibracionProx+=proximidad;
    calibracionHumed=humedad;
    contador++;
    
  }
  
  else if(bandera==0&&millis()>=5000){
    
    bandera=1;
    calibracionX=calibracionX/30;
    calibracionY=calibracionY/30;
    calibracionZ=calibracionZ/30;
    calibracionProx=calibracionProx/30;

      /*
      Serial.print(" X = ");
      Serial.print(calibracionX);
      Serial.print(" Y = ");
      Serial.print(calibracionY);
      Serial.print(" Z = ");
      Serial.println(calibracionZ);
      Serial.println("Acelerometro calibrado");
      Serial.print(" Prox = ");
      Serial.println(calibracionProx);
      Serial.println("Proximidad calibrada");
      Serial.print(" Humed = ");
      Serial.println(calibracionHumed);
      Serial.println("Humedad inicial");
      */
      
    delay(1000);
    
  }
  
  else if(bandera==1){
    
      for(int i=0;i<5;i++){
        movimientoAccel=0;
        movimientoProx=0;
        cambioHumed=0;
        acumuladorX+=normAccel.XAxis;
        acumuladorY+=normAccel.YAxis;
        acumuladorZ+=normAccel.ZAxis;
        acumuladorProx+=proximidad;
        delay(10);
      }
      
        acumuladorX=acumuladorX/5;
        acumuladorY=acumuladorY/5;
        acumuladorZ=acumuladorZ/5;
        acumuladorProx=acumuladorProx/5;

      /*  
      Serial.print(" X = ");
      Serial.print(acumuladorX);
      Serial.print(" Y = ");
      Serial.print(acumuladorY);
      Serial.print(" Z = ");
      Serial.println(acumuladorZ);
      Serial.print(" Prox = ");
      Serial.println(acumuladorProx);
      Serial.print(" Humed = ");
      Serial.println(humedad);
      */
      
      movimientoAccelX=acumuladorX-calibracionX;
      if(movimientoAccelX>3||movimientoAccelX<-3){
        movimientoAccel=2;
      }
      else if(movimientoAccelX>1&&movimientoAccelX<3||movimientoAccelX<-1&&movimientoAccelX>-3){
        movimientoAccel=1;
      }
      movimientoAccelY=acumuladorY-calibracionY;
      if(movimientoAccelY>3||movimientoAccelY<-3){
        movimientoAccel=2;
      }
      else if(movimientoAccelY>1&&movimientoAccelY<3||movimientoAccelY<-1&&movimientoAccelY>-3){
        movimientoAccel=1;
      }
      movimientoAccelZ=acumuladorZ-calibracionZ;
      if(movimientoAccelZ>3||movimientoAccelZ<-3){
        movimientoAccel=2;
      }
      else if(movimientoAccelZ>2&&movimientoAccelZ<3||movimientoAccelZ<-2&&movimientoAccelZ>-3){
        movimientoAccel=1;
      }

      if(acumuladorProx>1000){
        //Serial.println("MOVIMIENTO PROX");
        movimientoProx=1;
        //delay(1000);
      }

      cambioHumed=humedad-calibracionHumed;
      
      if(movimientoAccel==2||movimientoProx==1||cambioHumed>500||cambioHumed<-500){
        riesgo='d';
        
      }
      else if(movimientoAccel==1&&cambioHumed>200||cambioHumed<-200){
        riesgo='c';
        
      }
      else if(movimientoAccel==1||cambioHumed>200||cambioHumed<-200){
        riesgo='b'; 
        
      }
      else{
        riesgo='a';
        
      }
      Serial.println(riesgo);
  }
}
