
///////////////////////////////////////////////////////////////////////////////
typedef struct {byte pro[6]; unsigned long code[6]; int len[6];} rftype;   // 42 bytes
typedef struct {byte proon[18]; unsigned long codeon[18]; int lenon[18]; byte prooff[18]; unsigned long codeoff[18]; int lenoff[18];} code433type;   // 252 bytes
typedef struct {int devori; int actualizaut; float s[3]; float a1; char ua1[4]; int di[2],ds[2]; 
                long tdi[2],tdo[2]; char mac[13]; char idmyj[10]; float dhtdata[10][2]; } conucodata;
/* Raduino needs to keep track of current state of the transceiver. These are a few variables that do it */
  typedef struct {    // datos configuración
                  boolean modeCalibrate = false;  //this mode of menus shows extended menus to calibrate the oscillators and choose the proper
                                                  //beat frequency
                  int32_t calibration = 180000;
                  unsigned long usbCarrier=11056000;
                  unsigned long cwmCarrier=11056000;
                  byte splitOn = 0; //working split, uses VFO B as the transmit frequency                  byte isUSB = 0;   //upper sideband was selected, this is reset to the default for the 
                  byte isUSB = 0;   //upper sideband was selected, this is reset to the default for the 
                  int cwMode = 0;   //compatible original source, and extend mode //if cwMode == 0, mode check : isUSB, cwMode > 0, mode Check : cwMode
                                    //iscwMode = 0 : ssbmode, 1 :cwl, 2 : cwu, 3 : cwn (none tx)
                  unsigned int cwSpeed = 100; //this is actuall the dot period in milliseconds
                  unsigned long cwTimeout = 0;    //milliseconds to go before the cw transmit line is released and the radio goes back to rx mode
                  byte ritOn = 0;
                  byte advancedFreqOption1;     //255 : Bit0: use IFTune_Value, Bit1 : use Stored enabled SDR Mode, Bit2~Bit3 : dynamic sdr frequency,  bit 7: IFTune_Value Reverse for DIY uBITX
                  byte attLevel = 0;            //ATT : RF Gain Control (Receive) <-- IF1 Shift, 0 : Off, ShiftValue is attLevel * 100; attLevel 150 = 15K
                  byte if1TuneValue = 0;        //0 : OFF, IF1 + if1TuneValue * 100; // + - 12500;
                  byte sdrModeOn = 0;           //SDR MODE ON / OFF
                  unsigned long SDR_Center_Freq; //
                  byte vfoActive = VFO_A;
                  byte vfoA_mode = 0;          //0: default, 1:not use, 2:LSB, 3:USB, 4:CW, 5:AM, 6:FM
                  byte vfoB_mode = 0;          //0: default, 1:not use, 2:LSB, 3:USB, 4:CW, 5:AM, 6:FM
                  unsigned long vfoA=7150000L;
                  unsigned long vfoB=14200000L;
                  unsigned long sideTone=800;
                  byte cwDelayTime = 60;
                  byte delayBeforeCWStartTime = 50;
                  uint8_t SI5351BX_ADDR=0x60;        // I2C address of Si5351   (variable from Version 1.097)
                  byte cwKeyType = 0; //0: straight, 1 : iambica, 2: iambicb
                  byte tuneStepIndex=6; //default Value 6, start Offset is 0 because of check new user
                  byte commonOption0 = 0;   //0: Confirm : CW Frequency Shift,  1 : IF Shift Save
                  byte displayOption1 = 0;
                  byte displayOption2 = 0;
                  byte useHamBandCount = 0;  //0 use full range frequency
                  byte tuneTXType = 0;   //0 : use full range, 1 : just Change Dial speed, 2 : just ham band change, but can general band by tune, 3 : only ham band (just support 0, 2 (0.26 version))
                                         //100 : use full range but not TX on general band, 101 : just change dial speed but.. 2 : jut... but.. 3 : only ham band  (just support 100, 102 (0.26 version))
                  byte isShiftDisplayCWFreq = 1;  //Display Frequency 
                  unsigned int hamBandRange[MAX_LIMIT_RANGE][2]=    // =  //Khz because reduce use memory
                              {{1810,2000},{3500,3800},{5351,5367},{7000,7300},{10100,10150},
                               {14000,14350},{18068,18168},{21000,21450},{24890,24999},{28000,29700}};
                  long LIBRE[5]={10,50,100,500,1000};
                  int ifShiftValue = 0;   //
                  byte sMeterLevels[9];
                  byte KeyValues[16][3];
                  byte userCallsignLength = 0;    //7 : display callsign at system startup, 6~0 : callsign length (range : 1~18)
                  char CallSign[20]="EA4GZI";
                  byte WsprMSGCount = 0;
                  unsigned long frequency=7150000;  //frequency is the current frequency on the dial
                  unsigned long ritRxFrequency;  
                  unsigned long ritTxFrequency;  
                  byte scaledSMeter = 0;
                  byte wifimode=2;    // default AP+STA
                  byte canalAP=3;      // 1 byte, canal ESP en modo AP
                  char ssidSTA[20]="elconuco";       // 20 bytes, SSID en modo STA
                  char passSTA[20]="18921892";       // 20 bytes, Password en modo STA
                  char ssidAP[20]="UBITX";     // 20 bytes, SSID en modo AP
                  char passAP[20]="12341234";       // 20 bytes, Password en modo AP
                  IPAddress EEip={192,168,1,149};   // 4 bytes, dirección IP
                  IPAddress EEgw={192,168,1,1};     // 4 bytes, puerta de enlace
                  IPAddress EEmask={255,255,255,0}; // 4 bytes, máscara de subred
                  IPAddress EEdns={8,8,8,8};        // 4 bytes, servidor DNS primario
                  IPAddress EEdns2={8,8,4,4};       // 4 bytes, servidor DNS secundario
                  char EEmac[6][3];                 // 18 bytes, MAC
                  char myippub[16]="";              // 16 bytes, dirección IP pública
                  byte staticIP=1;                  // 1 byte, IP estática Sí/No
                  boolean ftpenable=true;
                  int webPort=88;                   // 2 bytes, puerto servidor web
                  unsigned long firstIF = 45005000L;
                  long arTuneStep[9]={100000000,10000000,1000000,100000,10000,1000,100,10,1};
                  float latitud=0.0;
                  float longitud=0.0;
                  byte lang=0;                      // 0=español, 1=inglés
                  byte rstper=0;   
                  byte usepassDev=0;                // 1 byte, 0 no usar password,  1 usar password
                  uint8_t probecode[maxTemp][8];            // código de sonda 
                  char userDev[20]="admin";         // 20 bytes, usuario device
                  char passDev[20]="admin";         // 20 bytes, password device
                  byte iftttenabled=0;
                  char iftttkey[30]="";             // 30 bytes, ifttt key
                  byte iottweetenable=0;
                  char iottweetuser[10]="";         // 10 bytes, IoTtweet account user ID
                  char iottweetkey[15]="";          // 15 bytes, IoTtweet account key
                  byte mqttenabled=0;
                  char mqttserver[40]="";           // MQTT broker
                  char mqttpath[6][10]={"","","","","",""};       // MQTT path
                  byte dweetenabled=0;                 // 1 byte, dweet.io desactivado
                  byte myjsonenabled=0;
                  char idmyjson[10]="";             // 10 bytes, ID myjson
                  byte idmyjsonST=0;                // 1 byte, indica si se ha obtenido y almacenado ya la ID de myjson
                  char hostmyip[30]="icanhazip.com";// 30 bytes, URL del servidor de IP pública
                  long freqbyband[MAX_LIMIT_RANGE]={1810000,3500000,5351000,7000000,10100000,14000000,18068000,21000000,24890000,28000000};
                
} conftype;
    conftype conf;
    byte *buffconf = (byte *) &conf; // acceder a conf como bytes

    byte edPin[maxED]={I0,I1,I2,I3};                // pines entradas digitales
    byte sdPin[maxSD]={O0,O1,O2,O3,O4,O5,O6,O7};    // pines salidas digitales/relés
    const byte listgpiovar[maxgpiovar]={2,4,12,13,14,15,21,22,32,33};
    const char idpin8266[15][4]={"t0","t1","t2","a0","e0","e1","s0","s1","id","ip","ipp","c0","c1","c2","all"}; // hasta el 8 son pines.
    const char idpin[34][4]={"t0","t1","t2","t3","t4","t5","t6","t7","e0","e1","e2","e3","s0","s1","s2","s3","s4","s5","s6","s7","x1","x2","id","ip","ipp",
                             "c0","c1","c2","c3","c4","c5","c6","c7","all"};
        // hasta el 22 son pines
    const char sensortype[maxsensortype][15]={"Input","Output","ADC","DAC","DHT","PT1000 JC-A99","NTC 10k Carel","ACS712","",
                                              "","","","","","","","","","","","","","","","","","","",""};

//////  tratamiento de bits /////////////////////
const byte tab[8] = {1,2,4,8,16,32,64,128}; // 8

/********* IMPORTANTE  *****************/
const byte valorpin[2] ={0,1};    // directo 0=LOW,  1=HIGH    

////UPDATER
int verserver=0; 
int versinsttemp=0;
byte actualizauttemp=0;
const char* update_username ="admin";
const char* update_password ="admin";
char fwUrlBasetemp[80]="";
////UPDATER END

String msg;                     // String donde se construye la respuesta HTML qu e se envía al cliente
char buff[20];                  // 20 bytes, auxiliar
char auxchar[130];              // 130 bytes, auxiliar 
char auxdesc[60];               // 60 bytes, auxiliar para lectura de descriptores de archivos
char auxtft[30];               // 30 bytes, auxiliar para varios
int peractpantemp=10;           // 2 período de actualización automática página panel
int peractremtemp=10;           // 2 período de actualización automática a nodo raíz
byte bsumatAtemp[maxEA]={0};   // 1 byte,  mostrar sumaA temp
int valoresTemp[maxTemp]={0,0,0,0,0,0,0,0}; // 8x2, 16 bytes guarda los valores de las sondas dividido por 100
int tiporemotetemp=8266;
byte nTemp=0;                  // número sondas detectadas en cada puerto 1-wire
uint8_t addr1Wire[maxTemp][8];  // tiene los valores conectados
unsigned long mact1,mact2,mact10,mact60,mact3600,mact86400; 
unsigned long tempact[maxSD];   // 8x4, 32 bytes, tiempos desde activación. Al llegar a tempdef se desactiva la salida.
unsigned long tempdes[maxSD];   // 8x4, 32 bytes, tiempos desde desactivación. Al llegar a tempdef se activa la salida.
unsigned long tempactgpio[maxSD];   // 10x4, 40 bytes, tiempos desde activación. Al llegar a tempdef se desactiva la salida.
unsigned long tempdesgpio[maxSD];   // 10x4, 40 bytes, tiempos desde desactivación. Al llegar a tempdef se activa la salida.
byte tipoEDtemp[maxED]={0,0,0,0};   // 4x1, 4 bytes, tipo de la entrada digital: 0=ON/OFF, 1=OFF/ON, 2=DHT11,3=DHT21,4=DHT22,5=RS485 RX/TX, ...
int nAP=0;                      // 2 bytes, redes encontradas con scanAP
int nAPact=0;                   // 2 bytes, redes actual
unsigned long countfaulttime=29999;       // veces que ha fallado la sincronización
int param_number=0;             // variables auxiliares para parámetros en POST
int indice=0;                   // variables auxiliares para parámetros en POST
int resto=0;                    // variables auxiliares para parámetros en POST
int mival=0;                    // variables auxiliares para parámetros en POST
int mp=1;                       // variables auxiliares para parámetros en POST
int mpi=0;                      // variables auxiliares para parámetros en POST
boolean colorea;                // variables auxiliares para parámetros en POST
long timerem[maxdevrem];        // tiempo en minutos desde última conexión con remoto
boolean actirem[maxdevrem];     // 16 bytes, activo remoto
long contaremote[maxsalrem];    // 32x4, 128 bytes, contadores o tiempo de encendido o apagado de señales remotas, en segundos
boolean actisenal[maxsalrem];   // 32 bytes, señales remotas activas
char unitpinAtemp[4];           // 4 char, unidades entradas analógicas
byte evenTemp[nEVE];            // 8 define la señal sobre la que se actua, variable temporal
byte bconactcumple[4];          // valor de cumplimiento actual de la condición
// variables para programación SEMANAL
int TempDesactPrgtemp=600;      // 2 bytes, tiempo desactivacion programacion en segundos, se compara con countfaulttime
int mbtemp[16];                 // temperaturas remotos modbus
int mbcons[16];                 // consignas remotos modbus
byte mbstatus[2];               // estado relés remotos modbus (1 bit cada uno);
byte bstatremote[4];            // 4 bytes, estado de cada posible salida remota
byte tipoedremote[32];          // 32 bytes, tipo de cada posible entrada digital remota, 0=ON/OFF, 1=DHT
float sondaremote[maxsalrem];   // 128 bytes, valores de sondas remotas

char mac[14]="";                // MAC del dispositivo

byte bevenENABLE[2][8];         // 2x8, 16 bytes, define si la condición ha activado ya algo
char host[16]="";
char hostraiz[16]="192.168.";
byte NumOri=0;
boolean statusChange=false;
byte ListOri[16]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
float factorAtemp[maxEA]={1.0,1.0}; // 1x4 factor conversión analógicas locales temp
float offsetAtemp[maxEA]={0.0,0.0}; // 1x4 offset conversión analógicas locales temp

byte MbC8ant[2]={0,0};           // estado anterior de SD y ED: 0:SD0, 1:SD1, 2:ED0, 3:ED1
byte MbC8antgpio[4]={0,0,0,0};   // estado anterior de gpios configurables
int MbR[8]={0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0};      // 0-7 Temperaturas locales
int MbRant[10]={0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0};   // 0-7 Temperaturas locales
float MbRgpio[10]={0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0};      // 0-9 Analógicas/temperaturas gpio
float sumMbRgpio[10]={0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0};   // valor medio de lecturas entre presentación 0-9 Analógicas gpio
int avrcount[10]={0,0,0,0,0,0,0,0,0,0};                           // número de lecturas para la media
long MbRantgpio[10]={0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0};    // 0-9 Analógicas gpio anteriores
byte iftttchange[4]={0,0,0,0};         // 0-7: salidas digitales,  8-11: entradas digitales,  16-31 Gpios como ED/SD
char idmyjsontemp[10]="";
boolean onescenaact=false;
byte secori=0;
int tempbt2=0;
float latitudtemp, longitudtemp;
char conucochar[7]="conuco";

int moddevicetemp=0;     
byte wifimodetemp=2;
byte iddevicetemp=0;     
char aliasdevicetemp[20]="";
char ssidSTAtemp[20]="";
char passSTAtemp[20]="";
byte iftttenabledtemp=0;  
char iftttkeytemp[30]="";
byte modomyjsontemp=0;
byte mododweettemp=0;
byte valinictemp[2]={0,0};
unsigned long tempdefacttemp[2];    // tiempos de activación por defecto. Si 0 es indefinido
unsigned long tempdefdestemp[2];    // tiempos de desactivación por defecto. Si 0 es indefinido
byte iftttpinEDtemp[2]={0};         // 2 bytes, enviar ifttt entradas digitales,byte 1 para ON, byte 2 para OFF
byte iftttpinSDtemp[2]={0};         // 2 bytes, enviar ifttt salidas digitales, byte 1 para ON, byte 2 para OFF
byte posactio=0;
byte posactpin=0;
byte posactsem=0;
byte posactrem=0;
byte posactsalrem=0;
byte posacteve=0;
byte posactfec=0;
byte posactesc=0;
char iottweetusertemp[10];          //IoTtweet account user ID (6 digits, included zero pre-fix)
char iottweetkeytemp[15];           //IoTtweet account user ID (6 digits, included zero pre-fix)
byte iottweetenabletemp=0;
long modbusbaud=19200;
String private_tweet="Mi Conuco";                  //Your private tweet meassage to dashboard
String public_tweet="Conuco";         //Your public tweet message to dashboard
byte lastpro=0;
long lastcode=0;
int lastlen=0;
byte paract=0;
float dhtdata[10][2];
byte panelact=0;
//byte priremio=0;
byte hacerresetrem=0;
long lastReconnectAttempt=0;
boolean pendsave = false;
boolean bmp085enabled=false;
conucodata datosremoto;
long lasttouch=0;
boolean tfton=true;

char admin[]="admin";

boolean filesok=false;
char fileconf[]="/ubitx.cnf";
char filecommon[]="/common.txt";
char filespanish[]="/spanish.txt";
char fileenglish[]="/english.txt";
/**char filezonas[]="/zonas.txt";
char filedevrem[]="/devrem.txt";
char filesalrem[]="/salrem.txt";
char filewebcall[]="/webcall.txt";
char fileurlwebcall[]="/urlwebcall.txt";
char filedescprg[]="/descprg.txt";
char filedescesc[]="/descesc.txt";
char filemacdevrem[]="/macdevrem.txt";
char fileidmyjsonrem[]="/idmyjsonrem.txt";
char fileunitsalrem[]="/unitsalrem.txt";
char filedesclocal[]="/desclocal.txt";
char filedescgpio[]="/descgpio.txt";
char filedesctemp[]="/desctemp.txt";
char filei2ctypes[]="/i2ctypes.txt";
char filelog[]="/log.txt";
char filedash[]="/dash.txt";**/
char flecha[4][3]={"<","<<",">>",">"};
byte tftpage=0;
byte tftapactual=0;

int testvalue=0;
unsigned long tini=0;
String sinput="";
////////////////////////////////////////
const byte maxbt817=2;

///////////////// variables bomba de calor

