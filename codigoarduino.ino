//año 2017
//Proyecto: El sistema activa un rele, a partir de un sensor de temperatura lm35
//Funciona con un comparador de ventana y el seteo de un tiempo en el cual debe finalizar el programa. 
//El sensor se conecta al pin A1
//El rele se conecta al pin D5
//La fuente de alimentacin es de 5V
////Podria declararse el voltaje de referencia y la resolucin del conversor
//para hacerlo generico.
int RelePin=6;
int IntervaloLed=10000;//tiempo en que actualiza la lectura del monitor serial
int PinPulsador=3;//Pin al que se conecta el pulsador de entrada 
int PinSensor=1;//Pin al que se conecta el sensor 



boolean FlagSaltarPasteurizado=1;//Bandera de saltear etapa
boolean FlagSaltarEnfriamiento=1;//Bandera de saltear etapa
int FlagSaltarCoagulacion=1;//Bandera de saltear etapa

boolean FlagPasteurizado=true;
boolean FlagEnfriamiento=true;//Bandera de que el enfriamiento termino.
boolean FlagCultivo=true;//Bandera de que el cultivo ha sido agregado
boolean FlagCoagulacion=true;//Bandera de que el proceso de coagulacion termino

float Tpasteurizado=85;//temperatura de pasteurizado.
float TemperaturaV=0;// Temperatura en milivoltios
float Tmin=42.5;//limite inferior del comparador de ventana
float Tmax=43.5;//limite superior del comparador de ventana
float Tyogurt=43;//Temperatura objetivo del yogurt
float FconversionBtoV=0.00488;//factor de conversion de binario a voltio
float FconversionBtoC=0.4887;//factor de conversion de binario a grado centigrado
float TemperaturaC=0;//Temperatura en grados centigrados
float TemperaturaB=0;//Temperatura en Binario

unsigned long int TiempoActualMs;//tiempo actual del programa en milisegundos 
unsigned long int TiempoFinMs;// tiempo en el que debe finalizar el programa en milisegundos
unsigned long int TiempoInicioY;// tiempo en el que debe finalizar el programa en milisegundos
unsigned long int TiempoEnfriamientoMs;//tiempo de enfriamiento. 
unsigned long int TiempoCoagulacionMs;//tiempo de coagulacion. 
unsigned long int TiempoPasteurizadoMs;//tiempo de Pasteurizado 
float TiempoPasteurizadoHs;
float TiempoEnfriamientoHs;
float TiempoCoagulacionHs;

unsigned long int Marca1;//marca de tiempo para calculos auxiliares.
unsigned long int TiempoHs = 6; //tiempo en que debe finalizar el coagulado en horas

const float FConversionMsAHoras = 1/3600000;
const float FConversionHorasAMs = 3600000;

int Tblink;
void setup()
{

  pinMode(RelePin,OUTPUT);
  pinMode(PinSensor,INPUT);//Sensor de temperatura
  pinMode(PinPulsador,INPUT);//Pulsador para saltear etapa
  Serial.begin(9600);
  unsigned long Tblink;

 pinMode(12,OUTPUT);

  pinMode(11,OUTPUT);

}

void loop()
{///////////////////////PASTEURIZADO//////////////////////////////////////////
  //En esta primer etapa calienta la leche para eliminar las bacterias
 
    digitalWrite(12,HIGH);
    digitalWrite(11,LOW);
  Marca1=millis();
  
  do{
    TemperaturaB=analogRead(PinSensor); //lee la temperatura
    TemperaturaV=TemperaturaB * FconversionBtoV;//convierte binario a voltaje
    TemperaturaC=TemperaturaB * FconversionBtoC;//convierte voltaje a temperatura
    
    TiempoActualMs=millis();
    
    Serial.println("Etapa Pasteurizado");
    Serial.println("Aun no alcanzo la temperatura");
    Serial.print("-Temp grado C ");
    Serial.print(TemperaturaC);//imprime la temperatura en °C
    Serial.print("  Tiempo de Pasteurizado ");
    TiempoPasteurizadoMs=TiempoActualMs-Marca1;//Calcula el tiempo que lleva en pasteurizado en Ms
    TiempoPasteurizadoHs=TiempoPasteurizadoMs*FConversionMsAHoras;
    Serial.print(TiempoPasteurizadoHs);//Tiempo de enfriamiento en Horas
    Serial.println();
    if(TemperaturaC<Tpasteurizado){
    digitalWrite(RelePin,HIGH);// enciende el rele
    blink_low();
    }
    
    if(TemperaturaC>Tpasteurizado){
    digitalWrite(RelePin,LOW);// apaga el rele
       blink_fast();
       FlagPasteurizado=0;//Marca que ya se alcanzo la Temperatura de Pasteurizacion
    }
  
    FlagSaltarPasteurizado=digitalRead(PinPulsador); //Lee el pulsador 
   
    }while((FlagPasteurizado) && (FlagSaltarPasteurizado));//Si ya se alcanzo la temperatura de pasteurizado, continua el programa.
Serial.println("--Status--");
Serial.print("-FlagPasteurizado");
Serial.println(FlagPasteurizado);
Serial.print("-FlagSaltarPasteurizado");
Serial.println(FlagSaltarPasteurizado);

//////////////////////////////ENFRIAMIENTO///////////////////////////////////  
  
    digitalWrite(12,LOW);
    digitalWrite(11,HIGH);
    
  blink_fast2();
  //Tiempo de espera a que se enfrie
  Marca1=millis();//Realiza una marca de cuando comienza a enfriar
  digitalWrite(RelePin,LOW);// apaga el rele
  do {
    TemperaturaB=analogRead(PinSensor); //lee la temperatura
    TemperaturaC=TemperaturaB * FconversionBtoC;//convierte voltaje a temperatura
    
    TiempoActualMs=millis();
    
    Serial.println();
    Serial.print("////////Etapa Enfriamiento///////////");
    Serial.println("Esperando descenso de temperatura  ");
    Serial.print("-Temperatura Actual: ");
    Serial.print(TemperaturaC);//imprime la temperatura actual
    Serial.print("  -Temperatura Objetivo: ");
    Serial.print(Tyogurt);//imprime la temperatura a la que debe alcanzar
    Serial.print("  -Tiempo: ");
    TiempoEnfriamientoMs=TiempoActualMs-Marca1;//Calcula el tiempo que lleva en enfriamiento en Ms
    TiempoEnfriamientoHs=TiempoEnfriamientoMs*FConversionMsAHoras;
    Serial.println(TiempoEnfriamientoHs);//Tiempo de enfriamiento en Horas
    
    blink_low();
    
    if(TemperaturaC<Tyogurt){
    FlagEnfriamiento=0;
    }
    
    FlagSaltarEnfriamiento=digitalRead(PinPulsador); //Lee el pulsador 
        
    
  }while((FlagEnfriamiento) && (FlagSaltarEnfriamiento));

Serial.println("--Status--");
Serial.print("-FlagEnfriamiento: ");
Serial.print(FlagEnfriamiento);
Serial.print(" -FlagSaltarEnfriamiento:");
Serial.println(FlagSaltarEnfriamiento);

    
//////////////////////Agregacion de Yogurt /////////////////////////////////
    blink_fast2();

    Serial.print("Agregue el cultivo y pulse el indicador");//
    do{
    digitalWrite(12,HIGH);
    digitalWrite(11,LOW);
    delay(100);
    digitalWrite(12,LOW);
    digitalWrite(11,HIGH);
    delay(100);
    FlagCultivo=digitalRead(PinPulsador);//Lee el pin    
    }while(FlagCultivo);
      
//////////////////////////////COAGULACION///////////////////////////////////  
    digitalWrite(12,HIGH);
    digitalWrite(11,HIGH);
    
    blink_fast2();
   
    Marca1=millis();//Realiza una marca de cuando comienza la coagulacion

    do{
      
    TiempoActualMs=millis();//toma el tiempo actual
    TiempoCoagulacionMs=TiempoActualMs-Marca1;//Calcula el tiempo que lleva en coagulacion en Ms
    TiempoCoagulacionHs=TiempoCoagulacionMs*FConversionMsAHoras;  
    
    TiempoFinMs=FConversionHorasAMs*TiempoHs;//convierte el tiempo de finalizacion a milisegundos
  
    TemperaturaB=analogRead(1); //lee la temperatura
    TemperaturaV=TemperaturaB * FconversionBtoV;//convierte binario a voltaje
    TemperaturaC=TemperaturaB * FconversionBtoC;//convierte voltaje a temperatura
    Serial.println("////Etapa Coagulacion/////");
    Serial.print("-Temp grado C ");
    Serial.println(TemperaturaC);//imprime la temperatura en °C
    Serial.print("-Tiempo de Coagulacion: ");
    Serial.print(TiempoCoagulacionHs);//Tiempo de coagulacion en Horas
    Serial.println();
        
     if (TemperaturaC >= Tmax) {
       digitalWrite(RelePin,LOW); //apaga el rele
       blink_fast();
       
     }  
     
     else if (TemperaturaC <= Tmin) {
       digitalWrite(RelePin,HIGH);// enciende el rele
       blink_low();
       digitalWrite(RelePin,LOW);// apaga el rele estas lineas adicionales generan un retardo para evitar el sobrecalentamiento del yogurt debido al retardo que presenta
       blink_low();
       blink_low();
       // el sistema y que hace que el calor de la hornalla electrica sobrecaliente la olla luego de apagado. 
     }
     
     else if (TemperaturaC >= Tmin && TemperaturaC <= Tmax) {
       Noblink();
     }
     
     if(TiempoActualMs>(TiempoFinMs+Marca1)){
      digitalWrite(RelePin,LOW);// apaga el rele
      FlagCoagulacion=0;//Marca la bandera
     }
     
     FlagSaltarCoagulacion=digitalRead(3); //Lee el pulsador 

}while ((FlagCoagulacion) && (FlagSaltarCoagulacion));



digitalWrite(RelePin,LOW);// apaga el rele
Serial.print("--Status--");
Serial.print("-FlagCoagulacion: ");
Serial.print(FlagCoagulacion);
Serial.print(" -FlagSaltarCoagulacion:");
Serial.println(FlagSaltarCoagulacion);

/////////////////Fin del Proceso
digitalWrite(12,LOW);
digitalWrite(11,LOW);
    
Serial.println("/////Fin del Proceso//////// ");
Serial.println("Status Final");

Serial.println("////////Etapa Pasteurizado///////////");
Serial.println("--Status--");
Serial.println("Etapa Pasteurizado");
Serial.print("  Tiempo de Pasteurizado ");
Serial.println(TiempoPasteurizadoHs);//Tiempo de enfriamiento en Horas
Serial.print("-FlagPasteurizado");
Serial.println(FlagPasteurizado);
Serial.print("-FlagSaltarPasteurizado");
Serial.println(FlagSaltarPasteurizado);

Serial.println("////////Etapa Enfriamiento///////////");
Serial.print("--Status--");
Serial.print("-Temperatura Objetivo: ");
Serial.println(Tyogurt);//imprime la temperatura a la que debe alcanzar
Serial.print("-Tiempo: ");
Serial.println(TiempoEnfriamientoHs);//Tiempo de enfriamiento en Horas
Serial.print("-FlagEnfriamiento: ");
Serial.println(FlagEnfriamiento);
Serial.print("-FlagSaltarEnfriamiento:");
Serial.println(FlagSaltarEnfriamiento);

Serial.println("////Etapa Coagulacion/////");
Serial.println("--Status--");
Serial.print("-Tiempo de Coagulacion: ");
Serial.println(TiempoCoagulacionHs);//Tiempo de coagulacion en Horas
Serial.print("-FlagCoagulacion: ");
Serial.println(FlagCoagulacion);
Serial.print(" -FlagSaltarCoagulacion:");
Serial.println(FlagSaltarCoagulacion);

Serial.print("Tiempo Total");
Serial.println(TiempoPasteurizadoHs+TiempoEnfriamientoHs+TiempoCoagulacionHs);

do{
        blink_progressive();
        }while(true);//Queda en este bucle
}

///////////////////////Funciones////////////////////////////////
void Noblink(){
TiempoActualMs=millis();
  do{
  digitalWrite(13,HIGH);
  }while (millis()-TiempoActualMs < IntervaloLed);

}


void blink_low(){
TiempoActualMs=millis();
  do{
  Tblink=1000;
  digitalWrite(13,HIGH);
  delay(Tblink);
  digitalWrite(13,LOW);
  delay(Tblink);
  }while (millis()-TiempoActualMs < IntervaloLed);
  
}
  

void blink_fast(){
  TiempoActualMs=millis();
  do{
  Tblink=200;
  digitalWrite(13,HIGH);
  delay(Tblink);
  digitalWrite(13,LOW);
  delay(Tblink);
  }while (millis()-TiempoActualMs < IntervaloLed);
}


void blink_fast2(){
  TiempoActualMs=millis();
  do{
  Tblink=50;
  digitalWrite(13,HIGH);
  delay(Tblink);
  digitalWrite(13,LOW);
  delay(Tblink);
  }while (millis()-TiempoActualMs < 10000);
}


void blink_progressive(){
TiempoActualMs=millis();
  do{
  for(Tblink=30;Tblink<300;){ 
    digitalWrite(13,HIGH);
    delay(Tblink);
    digitalWrite(13,LOW);
    delay(Tblink);
    Tblink=Tblink+10;
  }
  }while (millis()-TiempoActualMs < IntervaloLed);

}
  


