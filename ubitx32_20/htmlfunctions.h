
const char* serverIndex = "<form method='POST' action='/update' enctype='multipart/form-data'><input type='file' name='update'><input type='submit' value='Update'></form>";

void ICACHE_FLASH_ATTR printColspan(int ancho)
  { printP(c(tdcolspan_i)); printI(ancho); printP(comillas, mayor);}
  
void ICACHE_FLASH_ATTR cell(PGM_P texto) { printP(td,texto,td_f); }
void ICACHE_FLASH_ATTR cell(int num) { printPiP(td,num,td_f); }
void ICACHE_FLASH_ATTR cell(float num, byte deci) { printP(td); printF(num,deci); printP(td_f); }

void ICACHE_FLASH_ATTR ccell(int ntexto) { printP(td,c(ntexto),td_f); }
void ICACHE_FLASH_ATTR tcell(int ntexto) { printP(td,t(ntexto),td_f); }

void ICACHE_FLASH_ATTR pt(int pos)
{
  char auxlang[20]="";
  if (conf.lang==0) strcpy(auxlang,filespanish);
  if (conf.lang==1) strcpy(auxlang,fileenglish);
  File auxfile=SPIFFS.open(auxlang,letrar);
  if (auxfile)
    {
    auxfile.seek(42*(pos-1), SeekSet);
    auxfile.readBytes(auxdesc,42);
    auxfile.close();
    auxdesc[41]='\0';
    byte n=strlen(auxdesc);
    while ((n>0) && ((auxdesc[n-1]==' ')||(auxdesc[n-1]=='\n')||(auxdesc[n-1]=='\r'))) n--;
    auxdesc[n]='\0';
    }
  printP(auxdesc);
}

void ICACHE_FLASH_ATTR pc(int pos)
{
  File auxfile=SPIFFS.open(filecommon,letrar);
  if (auxfile)
    {
    auxfile.seek(42*(pos-1), SeekSet);
    auxfile.readBytes(auxdesc,42);
    auxfile.close();
    auxdesc[41]='\0';
    byte n=strlen(auxdesc);
    while ((n>0) && ((auxdesc[n-1]==' ')||(auxdesc[n-1]=='\n')||(auxdesc[n-1]=='\r'))) n--;
    auxdesc[n]='\0';
    }
  printP(auxdesc);
}

void ICACHE_FLASH_ATTR espacioSep(int col)  //  espacio separación
  { printP(tr); printColspan(col);printP(td_f,tr_f);  }

void printfincampo() { printP(mayor, menor, barra, c(tinput), mayor); }
void printdisabled() { printP(c(ttext)); printP(comillas,b,c(disabled)); printP(ig,comillas, c(disabled)); }
void printselected(boolean check) { printP(b, check?checked:selected); }

void ICACHE_FLASH_ATTR printcampoI(int numpar, int valactual, byte tam, boolean enabled,boolean printtd)
{
  if (printtd) printP(td);
  printP(menor, c(tinput), b, type, ig, comillas);  
  if (!enabled) printdisabled();
  printP(c(ttext), comillas, b);
  printP(c(namet), ig,comillas);
  printI(numpar);
  printP(comillas,b, tvalue, ig,comillas);
  printI(valactual);
  printP(comillas,b,c(max_length));
  printIP(tam, size_i);
  printIP(tam,comillas);
  printfincampo();
  if (printtd) printP(td_f);
}

void ICACHE_FLASH_ATTR printcampoL(int numpar, long valactual, byte tam, boolean enabled,boolean printtd)
{
  if (printtd) printP(td);
  printP(menor, c(tinput), b, type, ig, comillas);
  if (!enabled) printdisabled();
  printP(c(ttext), comillas, b);
  printP(c(namet), ig,comillas);
  printI(numpar);
  printP(comillas,b, tvalue, ig,comillas);
  printI(valactual);
  printP(comillas,b,c(max_length));
  printIP(tam, size_i);
  printIP(tam, comillas);
  printfincampo();
  if (printtd) printP(td_f);
}

void ICACHE_FLASH_ATTR printcampoF(int numpar, float valactual, int deci)
{
  printP(menor, c(tinput), b, type, ig, comillas);
  printP(c(ttext), comillas, b);
  printP(c(namet), ig,comillas);
  printI(numpar);
  printP(comillas,b, tvalue, ig,comillas);
  printF(valactual, deci);
  printP(comillas,b,c(max_length));
  printIP(10,size_i);
  printIP(10,comillas);
  printfincampo();
}

void ICACHE_FLASH_ATTR printcampoC(int numpar, char *valactual, byte tam, boolean visible, boolean enabled, boolean pass,boolean printtd)
{
  if (printtd) printP(td);
  printP(menor, c(tinput), b, type, ig, comillas);
  if (visible)  {
    printP(pass?c(tpassword):c(ttext));
    if ((!enabled) && (!pass)) { printP(comillas, b, c(disabled)); printP(ig, comillas, c(disabled)); }
    printP(comillas);   }
  else
    {
    printP(c(ttext), comillas, b);
    printP(c(invis), b);  }
  printP(b, c(namet), ig );
  printPiP(comillas,numpar, comillas);
  printP(b, tvalue, ig, comillas, valactual);
  printP(comillas,b, c(max_length));
  printIP(tam-1, size_i);
  printIP(tam-1, comillas);
  printfincampo();
  if (printtd) printP(td_f);
}

void ICACHE_FLASH_ATTR printparCP(const char *titulo, int numpar, char valactual[], byte tam, boolean pass)
{
  printP(tr);
  cell(titulo);
  printcampoC(numpar, valactual, tam, true, true, pass,true);
  printP(tr_f);
}

void ICACHE_FLASH_ATTR printcampoSiNo(int numpar, int valactual)
  {
  printP(c(Select_name), comillas);
  printIP(numpar, comillas);
  printP(mayor, c(optionvalue),comillas, cero, comillas);
  if (valactual==0) printselected(false);
  printP(mayor, OFF, c(option_f));
  printP(c(optionvalue));
  printPiP(comillas, 1, comillas);
  if (valactual) printselected(false);
  printP(mayor, ON, c(option_f));
  printP(c(select_f));
  }

void ICACHE_FLASH_ATTR printparentesis(PGM_P letra, int numero)
{
  printP(paren_i,letra);
  printI(numero);
  printP(paren_f, b, b);
}

void ICACHE_FLASH_ATTR checkBox(int numpar, bool selected, boolean printtd)
{
  if (printtd) printP(td);
  printP(menor, c(tinput), b, type, ig, comillas);
  printP(c(checkbox), comillas, b);
  printP(c(namet), ig,comillas);
  printI(numpar);
  printP(comillas,b, tvalue, ig,comillas);
  printP(uno,comillas);
  if (selected) printselected(true);
  printfincampo();
  if (printtd) printP(td_f);
}

void ICACHE_FLASH_ATTR writeHeader(boolean refreshmode, boolean ajaxmode)
{
  printP(menor, thtml);
  printP(b, c(xmlns), mayor);
  printP(menor, thead, mayor, menor, c(title), mayor);
  printP(c(conuco));
  printP(c(web));
  printP(c(tserver), menor, barra);
  printP(c(title));
  printP(mayor,htmlHead_3);
  printP(ajaxscript);
  printP(c(head_f));
}

void printinicampoCB(int numpar)
{
  printP(c(Select_name), comillas);
  printIP(numpar, comillas);
  printP(mayor);
}

void ICACHE_FLASH_ATTR printcampoCB(int numpar, int valact, int pri, int ult, boolean printtd)
{
  if (printtd) printP(td);
  printinicampoCB(numpar);
  for (byte j=pri; j<=ult; j++)   {
    printP(c(optionvalue));
    printPiP(comillas, j, comillas);
    if (valact==j) printselected(false);
    printPiP(mayor, j, c(option_f));   }
  printP(c(select_f));
  if (printtd) printP(td_f);
}

void ICACHE_FLASH_ATTR printcampoCB(int numpar, int valact, byte lon, PGM_P t[], boolean printtd)
{
  if (printtd) printP(td);
  printinicampoCB(numpar);
  for (byte j=0;j<lon;j++)   {
    printP(c(optionvalue));
    printPiP(comillas, j, comillas);
    if (valact==j) printselected(false);
    printP(mayor,t[j]);
    printP(c(option_f));   }
  printP(c(select_f));
  if (printtd) printP(td_f);
}

void ICACHE_FLASH_ATTR printcampoCB(int numpar, int valact, PGM_P t0, PGM_P t1, boolean printtd)
{
  PGM_P t[]={t0,t1};
  printcampoCB(numpar,valact,sizeof(t)/sizeof(t[0]),t,printtd);
}

void ICACHE_FLASH_ATTR printcampoCB(int numpar, int valact, PGM_P t0, PGM_P t1, PGM_P t2, boolean printtd)
{
  PGM_P t[]={t0,t1,t2};
  printcampoCB(numpar,valact,sizeof(t)/sizeof(t[0]),t,printtd);
}

void ICACHE_FLASH_ATTR printcampoCB(int numpar, int valact, PGM_P t0, PGM_P t1, PGM_P t2, PGM_P t3, boolean printtd)
{
  PGM_P t[]={t0,t1,t2,t3};
  printcampoCB(numpar,valact,sizeof(t)/sizeof(t[0]),t,printtd);
}

void ICACHE_FLASH_ATTR printcampoCB(int numpar, int valact, PGM_P t0, PGM_P t1, PGM_P t2, PGM_P t3, PGM_P t4, boolean printtd)
{
  PGM_P t[]={t0,t1,t2,t3,t4};
  printcampoCB(numpar,valact,sizeof(t)/sizeof(t[0]),t,printtd);
}

void ICACHE_FLASH_ATTR printcampoCB(int numpar, int valact, PGM_P t0, PGM_P t1, PGM_P t2, PGM_P t3, PGM_P t4, PGM_P t5, boolean printtd)
{
  PGM_P t[]={t0,t1,t2,t3,t4,t5};
  printcampoCB(numpar,valact,sizeof(t)/sizeof(t[0]),t,printtd);
}

void printaddr1Wire(byte i)
{
  if (i==0)
    printP("No probe");
  else
    for (uint8_t j=0;j<8;j++) { printH(conf.probecode[i-1][j]); } 
  }

void ICACHE_FLASH_ATTR selectProbe(int numpar, int valact, boolean printtd)
{
  if (printtd) printP(td);
  printinicampoCB(numpar);
  for (byte i=0;i<nTemp+1;i++)   
    {
    printP(c(optionvalue));
    printPiP(comillas, i, comillas);
    if (valact==i) printselected(false);
    printP(mayor); 
    if (i==0)
      printP("No probe");
    else 
      for (uint8_t j=0;j<8;j++) { if (addr1Wire[i-1][j]<16) printP(cero); printH(addr1Wire[i-1][j]); }   
    printP(c(option_f));   }
  printP(c(select_f));
  if (printtd) printP(td_f);
}

void ICACHE_FLASH_ATTR tituloFila(PGM_P texto, int num, PGM_P letra, int indice)
{
  printP(td, paren_i);
  printPiP(letra, indice, paren_f);
  printP(b, b, texto);
  printPiP(b, num, td_f);
}

int ICACHE_FLASH_ATTR getMyIP()
{
  msg=vacio;
  printP(barra);
  HTTPClient http;
  Serial2.print("hostmyip:"); Serial2.println(conf.hostmyip);
  http.begin(conf.hostmyip, 80, msg);
//  http.setConnectTimeout(conf.timeoutNTP);
  http.setConnectTimeout(1000);
  Serial2.print("getMyIP:");Serial2.print("host:");Serial2.println(conf.hostmyip);
  int httpCode=http.GET();
  Serial2.print(" ");Serial2.println(httpCode);
  if (httpCode > 0) {
    if (httpCode == HTTP_CODE_OK) { msg=http.getString(); msg.toCharArray(conf.myippub, msg.length());  } }
  http.end();
  msg=vacio;
  return httpCode;
}

int ICACHE_FLASH_ATTR checkMyIP()
{
  char auxip[16];
  strcpy(auxip, conf.myippub);
  getMyIP();    // busca actual y la guarda en conf.txt
  if (strcmp(conf.myippub, auxip) != 0) // son diferentes
    {
    saveconf();
//    if (conf.iftttenabled) ifttttrigger(conucochar, conf.iftttkey, conf.aliasdevice, "NewIP", conf.myippub);
    }
}

void printTime()
{
  printI(day()); printPiP(barra, month(), barra); printIP(year(),b);
  if (hour()<10) printP(cero); printI(hour()); printP(dp);
  if (minute()<10) printP(cero); printI(minute()); printP(dp);
  if (second()<10) printP(cero); printI(second());
}

void HtmlGetStateData(byte ind)    // 
{
  printP(td);
  if (ind==0) printP("VFO");
  else if (ind==1) printP("Mode");
  else if (ind==2) printP("CW");
  else if (ind==3) printP("RIT");
  else if (ind==4) printP("Split");
  else if (ind==5) printP("Freq. A");
  else if (ind==6) printP("Freq. B");
  else if (ind==7) printP("Freq. actual");
  printP(td_f,td);
  if (ind==0) printP(conf.vfoActive==VFO_A?"A":"B");
  else if (ind==1) printP(conf.isUSB==1?"USB":"LSB");
  else if (ind==2) printP(conf.cwMode>=1?"ON":"OFF");
  else if (ind==3) printP(conf.ritOn>=1?"ON":"OFF");
  else if (ind==4) printP(conf.splitOn>=1?"ON":"OFF");
  else if (ind==5) printL(conf.frequencyA);
  else if (ind==6) printL(conf.frequencyB);
  else if (ind==7) printL(conf.frequency);
  printP(td_f,td,td_f);
}

void ICACHE_FLASH_ATTR HtmlGetStateTime()
{
  printColspan(3);
  printTime();
  printP(td_f);
}


char* ICACHE_FLASH_ATTR textonoff(float valor)
{ if (valor==1) return "ON"; else return "OFF";  }

void ICACHE_FLASH_ATTR writeForm(PGM_P phtm)
{
  printP(c(form_action), phtm);
  printP(comillas,b);
  printP(c(Form_post), menor);
  printP(table, b);
  printP(c(tclass), ig, tnormal, mayor);
}

void ICACHE_FLASH_ATTR writeFooter(int texto, boolean cerrar)
{
  printP(menor, barra, table, mayor);               // final <table>
  printP(menor, c(tinput), b, type, ig, comillas);
  printP(submit);                                   // submit
  printP(comillas, b, tvalue, ig, comillas);      // " value="texto
  pt(texto);                                  
  if (cerrar) printP(comillas, b, c(onclickclose));
  printP(comillas);
  printfincampo();
  printP(c(form_f));
  printP(c(body_f), menor, barra);
  printP(thtml, mayor);
}

void ICACHE_FLASH_ATTR setCookie(byte valor)
{
  msg=vacio;
  printP(c(HTTP11), b);
  printP(c(t301), b, ok, crlf);
  printP(c(setcookie),dp, b);
  printP(c(espsessionid), ig, valor==0?cero:uno);
  printP(crlf,c(location), dp, b, barra, crlf);
  printP(c(cachecontrol),dp, b);
  printP(c(nocache), crlf, crlf);
  server.sendContent(msg);
  msg=vacio;
}

void ICACHE_FLASH_ATTR sendOther(const char *otherURL, int param)
{
  msg=vacio;
  printP(c(HTTP11),b);
  printP(c(t303),b);
  printP(c(seeother),crlf);
  printP(c(location),dp,b,otherURL);
  if (param>=0) { printP(paramn);printI(param);}
  printP(crlf,crlf);
  server.sendContent(msg);
  msg=vacio;
}

void ICACHE_FLASH_ATTR printDiaSem(byte i)
  {
  if (i==0) printP(letraD);  
  if (i==1) printP(letraL);  
  if (i==2) printP(letraM);  
  if (i==3) printP(letraX);  
  if (i==4) printP(letraJ);  
  if (i==5) printP(letraV);  
  if (i==6) printP(letraS);  
  }

void htmlNotFound()
{
  msg=vacio;
  printP("HTTP/1.1", b);
  printP("404", b);
  server.send(404, "text/plain", msg);
  msg=vacio;
}


void panelHTML() {
  msg=vacio;
  if (server.method()==HTTP_POST) return; 
  writeHeader(false,true);
  byte auxI=server.arg(0).toInt();
  writeMenu(1, auxI);
  printP(menor, table,b);
  printP("class", ig, tpanel, mayor,tr);
  
  /////////////  CONTENIDO   ///////////
  printColspan(3);
  printP("uBitx", td_f, tr_f);

  printP(menor,letrat,letrar,b);  printP(c(tid),ig,comilla,letral);
  printI(0);  printP(comilla,mayor);
  HtmlGetStateData(0);
  printP(tr_f);
  
  printP(menor,letrat,letrar,b);  printP(c(tid),ig,comilla,letral);
  printI(1);  printP(comilla,mayor);
  HtmlGetStateData(1);
  printP(tr_f);
  
  printP(menor,letrat,letrar,b);  printP(c(tid),ig,comilla,letral);
  printI(2);  printP(comilla,mayor);
  HtmlGetStateData(2);
  printP(tr_f);
  
  printP(menor,letrat,letrar,b);  printP(c(tid),ig,comilla,letral);
  printI(3);  printP(comilla,mayor);
  HtmlGetStateData(3);
  printP(tr_f);
  
  printP(menor,letrat,letrar,b);  printP(c(tid),ig,comilla,letral);
  printI(4);  printP(comilla,mayor);
  HtmlGetStateData(4);
  printP(tr_f);
  
  printP(menor,letrat,letrar,b);  printP(c(tid),ig,comilla,letral);
  printI(5);  printP(comilla,mayor);
  HtmlGetStateData(5);
  printP(tr_f);
  
  printP(menor,letrat,letrar,b);  printP(c(tid),ig,comilla,letral);
  printI(6);  printP(comilla,mayor);
  HtmlGetStateData(6);
  printP(tr_f);
  
  printP(menor,letrat,letrar,b);  printP(c(tid),ig,comilla,letraf);
  printI(7);  printP(comilla,mayor);
  HtmlGetStateData(7);
  printP(tr_f);
 
  for (byte i=0;i<MAX_BANDS;i++)
    {
    printP(tr,td,"Band ",conf.hamBandName[i],b);
    printP(letram,td_f,td); 
    printI(conf.freqbyband[i][0]); printP(td_f, td);
    printI(conf.freqbyband[i][1]); printP(td_f, tr_f);
    }
  // final
  printP(menor,letrat,letrar,b,c(tid));
  printP(ig,comilla,letrat,letrat,comilla,mayor);
  HtmlGetStateTime();
  printP(tr_f, menor, barra, table, mayor);
  printP(c(body_f), menor, barra,thtml, mayor);
  serversend200();
}

void ICACHE_FLASH_ATTR indexHTML() 
  {
  panelHTML();
  }


void setupDevHTML()
{
  msg=vacio;
  mp=1;
  if (server.method()==HTTP_POST)
    {
    for (int i=0; i<server.args(); i++)
      {
      calcindices(i);
      if (param_number==0)    // callsign
        {
        }
      else if (param_number==1) { conf.calibration = server.arg(i).toInt();  }
      else if (param_number==2) { conf.usbCarrier = server.arg(i).toInt();  }
      else if (param_number==3) { conf.latitud = server.arg(i).toFloat();  }
      else if (param_number==4) { conf.longitud = server.arg(i).toFloat(); }
      else if (param_number==5) { conf.lang = server.arg(i).toInt(); }   
      else if (param_number==6) { conf.cwSpeed = server.arg(i).toInt(); }
      else if (param_number==7) { conf.cwDelayTime = server.arg(i).toInt(); }
      else if (param_number==8) { conf.cwKeyType = server.arg(i).toInt(); }
      else if (param_number==9) { conf.SI5351BX_ADDR = server.arg(i).toInt(); }
      else if (param_number==99) { conf.rstper = server.arg(i).toInt(); } // período rset automatico
      }
    saveconf();
    readconf();
    sendOther(sdhtm,-1);
    return;
    }

  writeHeader(false,false);
  writeMenu(3, 0);
  writeForm(sdhtm);

  printP(tr,td,"CALLSIGN",td_f);
  printcampoC(0, conf.CallSign, 19, true, true, false, true);
  printP(td,td_f,tr_f);

  printP(tr,td,"Calibration/usbCarrier",td_f); 
  printcampoL(1, conf.calibration, 10, true, true);
  printcampoL(2,  conf.usbCarrier, 10, true, true); 

  printP(tr, td, href_i, comillas);
  pc(thttp);
  pc(gmaps);
  printP(comillas, b, c(newpage), mayor);
  printP(t(latitudt), barraesp);
  printP(t(longitudt), href_f, td_f, td);
  printcampoF(3, conf.latitud, 6);  printP(td_f, td);
  printcampoF(4, conf.longitud, 6);  printP(td_f, tr_f);

  printP(tr, td, t(idioma),td_f);
  printcampoCB(5, conf.lang, PSTR("Español"), PSTR("English"),true); 
  printP(td_f,td,td_f,tr_f);

  printP(tr,td,"CW Speed"); 
  printcampoI(6, conf.cwSpeed, 5, true, true); printP(td_f,td,td_f,tr_f);
  printP(tr,td,"cwDelayTime"); 
  printcampoI(7, conf.cwDelayTime, 5, true, true); printP(td_f,td,td_f,tr_f);
  printP(tr,td,"cwKeyType",td_f); 
  printcampoCB(8, conf.cwKeyType, "straight", "iambica", "iambicb", true); 
  printP(td,td_f);
  printP(tr,td,"SI5351BX_ADDR"); 
  printcampoI(9, conf.SI5351BX_ADDR, 5, true, true); printP(td_f,td,td_f,tr_f);
  

  printP(tr, td, "Reset periodico (horas)",td_f,td);
  printcampoCB(99,conf.rstper,1,24,false); 
  printP(td_f,td,td_f,tr_f);

  writeFooter(guardar, false);
  serversend200();
}

void setupBandHTML()
{
  msg=vacio;
  mp=1;
  if (server.method()==HTTP_POST)
    {
    for (int i=0; i<server.args(); i++)
      {
      calcindices(i);
      if (resto==0)
        conf.hamBandRange[indice][0]=server.arg(i).toInt();
      else if (resto==1)
        conf.hamBandRange[indice][1]=server.arg(i).toInt();
      }
    saveconf();
    readconf();
    sendOther(sbhtm,-1);
    return;
    }

  writeHeader(false,false);
  writeMenu(3,2);
  writeForm(sbhtm);
  printP(tr,td,"Banda",td_f,td);
  printP("desde",td_f,td," hasta   khz",td_f);
  printP(tr_f);
  for (byte i=0;i<MAX_BANDS;i++)
    {
    printP(tr);
    printP(td,conf.hamBandName[i],b,letram,td_f); 
    printcampoI(2*i, conf.hamBandRange[i][0], 10, true, true);
    printcampoI(2*i+1, conf.hamBandRange[i][1], 10, true, true);
    printP(tr_f);  
    }
  writeFooter(guardar, false);
  serversend200();
}

void ICACHE_FLASH_ATTR setupNetHTML()
{
  if (!autOK()) { sendOther(loghtm,-1); return; }
  msg=vacio;
  mp=1;
  if (server.method()==HTTP_POST)
    {
    conf.staticIP=0;conf.autoWiFi=0;
    for (int i = 0; i < server.args(); i++)
      {
      calcindices(i);
      if (param_number>=0 && param_number <= 5) { server.arg(i).toCharArray(conf.EEmac[i], 3);  }
      else if (param_number>=6 && param_number <= 8)
        {
        conf.EEip[param_number-6]=server.arg(i).toInt();
        conf.EEgw[2]=conf.EEip[2];
        strcpy(hostraiz,itoa(conf.EEip[0],buff,10)); strcat(hostraiz, punto);
        strcat(hostraiz,itoa(conf.EEip[1],buff,10)); strcat(hostraiz, punto);
        strcat(hostraiz,itoa(conf.EEip[2],buff,10)); strcat(hostraiz, punto);
        }
      else if (param_number>=10 && param_number <= 13) { conf.EEmask[param_number-10] = server.arg(i).toInt(); }
      else if (param_number>=14 && param_number <= 17) { conf.EEgw[param_number-14] = server.arg(i).toInt();   }
      else if (param_number>=18 && param_number <= 21) { conf.EEdns[param_number-18] = server.arg(i).toInt();  }
      else if (param_number==22) { conf.webPort = server.arg(i).toInt();  }
      else if (param_number==41) { conf.wifimode = server.arg(i).toInt(); }
      else if (param_number==42) { server.arg(i).toCharArray(conf.ssidSTA, 20);}
      else if (param_number==43) { server.arg(i).toCharArray(conf.passSTA, 20); }
      //      else if (param_number == 44) {server.arg(i).toCharArray(conf.ssidAP,20);}
      else if (param_number==45) { server.arg(i).toCharArray(conf.passAP, 9); }
      else if (param_number==46) { conf.staticIP = server.arg(i).toInt(); }
      else if (param_number>=47 && param_number <= 50) { conf.EEdns2[param_number - 47] = server.arg(i).toInt(); }
      else if (param_number==56) { conf.canalAP = server.arg(i).toInt()+1; }
      else if (param_number==57) { conf.autoWiFi = server.arg(i).toInt(); }
      }
    //
    //    nAP = 0;
    saveconf();
    sendOther(snehtm,-1); return;
    }

  writeHeader(false,false);
  writeMenu(3, 3);
  writeForm(snehtm);

  printP(tr, td, "Auto WiFi", td_f, conf.autoWiFi==1 ? th : td);
  checkBox(57,conf.autoWiFi,false);
  printP(conf.autoWiFi?th_f:td_f,tr_f);
  
  printP(tr, td, t(Modo), b, td_f);
  printcampoCB(41, conf.wifimode, sta, ap, apsta,true);
  printP(tr_f);

  // ssid
  printP(tr);
  ccell(routerssid);
  printP(td);
  printcampoC(42, conf.ssidSTA, 20, true, true, false,false);
  printP(href_i, comillas, scanap, comillas,mayor, b);
  printP(t(explorar), href_f, td_f, tr_f);

  printparCP(c(routerpass), 43, conf.passSTA, 20, false); 

  printP(tr);
  ccell(apssid);
  cell(conf.ssidAP);
  printP(tr_f);
  printparCP(c(appass), 45, conf.passAP, 9, false);
  printP(tr, td, t(canal), td_f, td);
  printP(c(Select_name),comillas);
  printIP(56, comillas);
  printP(mayor);
  for (byte j = 0; j < 13; j++)   { // canales
    pc(optionvalue);
    printPiP(comillas, j, comillas);
    if (conf.canalAP-1==j) printP(b, selected, ig, comillas, selected, comillas);
    printPiP(mayor, j+1, c(option_f));
  }
  printP(c(select_f), td_f, tr_f);

  espacioSep(2);
  printP(tr);
  ccell(MAC);
  printP(td);
  for (byte i=0; i<5; i++) printP(conf.EEmac[i]); printP(conf.EEmac[5]); 
  printP(td_f, tr_f);
  printP(tr, td, t(staticip), td_f, conf.staticIP ? th : td);
  checkBox(46,conf.staticIP,false);
  printP(conf.staticIP?th_f:td_f,tr_f);

  // print the current IP
  printP(tr, td, t(DIRIP), td_f, td);
  for (byte i=0; i<3; i++) { printI(conf.EEip[i]); printP(punto);  }  printI(conf.EEip[3]);
  printP(td_f, tr_f);

  printP(tr);
  ccell(tIP);
  printP(td);
  for (byte i=0; i<4; i++) printcampoI(6+i, conf.EEip[i],3,i!=3,false);
  printP(td_f, tr_f,tr);
  ccell(tmask);
  printP(td);
  for (byte i=0; i<4; i++) printcampoI(i+10, conf.EEmask[i], 3, true,false);
  printP(td_f, tr_f);
  
  printP(tr, td, c(ngateway), td_f, td);
  for (byte i=0; i<4; i++) printcampoI(i+14, i==2?conf.EEip[i]:conf.EEgw[i],3,i!=2,false);
  printP(td_f, tr_f);

  printP(tr,td);
  printP("DNS");
  printP(td_f,td);
  for (byte i=0; i<4; i++) printcampoI(i+18, conf.EEdns[i], 3, true,false);
  printP(td_f, tr_f);
  
  printP(tr, td, t(ippublica), td_f, td);
  printP(conf.myippub, td_f, tr_f);

  //  printP(tr,td,ttimeoutrem,td_f,td);
  //  printcampoL(53, timeoutrem, 5,true);
  //  printP(td_f,tr_f);
  //  printP(tr,td,c(ttimeoutNTP),td_f,td);
  //  printcampoL(54, conf.timeoutNTP, 5,true);
  //  printP(td_f,tr_f);

  printP(menor, barra, table, mayor, menor, c(tinput));
  printP(b, type, ig, comillas, submit, comillas);
  printP(b, tvalue, ig, comillas);
  printP(tguardar, comillas);
  printP(mayor, menor, barra, c(tinput), mayor);
  pc(form_f);
  printP(c(body_f), menor, barra);
  printP(thtml, mayor);
  serversend200();
}

void ICACHE_FLASH_ATTR setupNetServHTML()
{
  msg=vacio;
  mp=1;
  if (server.method() == HTTP_POST)
    {
    conf.dweetenabled=0; conf.iftttenabled=0; conf.myjsonenabled=0; 
    conf.ftpenable=0; conf.mqttenabled=0; conf.iottweetenable=0;
    for (int i=0; i<server.args(); i++)
      {
      calcindices(i);
      if (param_number==0) { server.arg(i).toCharArray(conf.hostmyip, 30); }
      else if (param_number==1) { conf.ftpenable=server.arg(i).toInt();  } // ftp server enabled      }
      else if (param_number==2) { conf.iftttenabled=server.arg(i).toInt(); } // enable IFTTT
      else if (param_number==3) { server.arg(i).toCharArray(conf.iftttkey, 30); }
      else if (param_number==4) { conf.dweetenabled=server.arg(i).toInt(); }   // modo Dweet.io
      else if (param_number==5) { conf.myjsonenabled=server.arg(i).toInt(); if (conf.myjsonenabled==0) conf.idmyjsonST=0; }  // modo myjson.com
      else if (param_number==6) { conf.iottweetenable=server.arg(i).toInt();  } // iottweet enabled      }
      else if (param_number==7) { server.arg(i).toCharArray(conf.iottweetuser, 10); }
      else if (param_number==8) { server.arg(i).toCharArray(conf.iottweetkey, 15);  }
      else if (param_number==9) { conf.mqttenabled=server.arg(i).toInt(); } // enable MQTT
      else if (param_number==10) { server.arg(i).toCharArray(conf.mqttserver, 40);  }
      else if (param_number>=11) { server.arg(i).toCharArray(conf.mqttpath[param_number-11], 10);  }
      }
    saveconf();
  //  createdashfile();
    sendOther(snshtm,-1);
    return;
    }

  writeHeader(false,false);
  writeMenu(3, 4);
  writeForm(snshtm);

  printP(tr);
  tcell(ippubserver);
  printP(td, td_f);
  printColspan(2);
  printcampoC(0, conf.hostmyip, 30, true, true, false,false);
  printP(td_f, tr_f);

  printP(tr);
  tcell(ftpserver);
  checkBox(1, conf.ftpenable,true);
  printColspan(2);
  printP(tr_f);

  printP(tr, td, href_i, comillas);
  pc(thttps);
  pc(iftttcom);
  printP(comillas, b, c(newpage), mayor);
  pc(ifttt);
  printP(barraesp,c(Key), href_f, td_f, conf.iftttenabled?th:td);
  checkBox(2, conf.iftttenabled,false);
  printP(conf.iftttenabled?th_f:td_f);
  printColspan(2);
  printcampoC(3, conf.iftttkey, 30, true, true, false,false);
  printP(td_f, tr_f);

  printP(tr, td, href_i, comillas);
  pc(thttp);
  pc(dweetio);
  printP(comillas, b, c(newpage), mayor);
  pc(dweet);
  printP(barraesp,c(dweetname), href_f, td_f, conf.dweetenabled ? th : td);
  checkBox(4, conf.dweetenabled,false);
  printP(td_f);
  printColspan(2);
  if (conf.dweetenabled==1)
    {
    printP(href_i);
    pc(urldweet);
    pc(conuco); // 5 últimos dweets 24h
    printP(mac, mayor, c(conuco), mac, href_f);
    }
  printP(td_f, tr_f);

  printP(tr, td, href_i, comillas);
  pc(thttp);
  pc(myjsoncom);
  printP(comillas,b, c(newpage), mayor);
  pc(modomyjsont);
  printP(barraesp, c(Key), href_f, td_f, conf.myjsonenabled ? th : td);
  checkBox(5, conf.myjsonenabled,false);
  printP(conf.myjsonenabled ? th_f : td_f);
  printColspan(2);
  if (conf.myjsonenabled== 1) 
    {
    printP(href_i, comillas);
    pc(thttps);
    printP(c(api), punto);
    pc(myjsoncom);
    printP(c(bins), conf.idmyjson, interr);
    printP(c(pretty), comillas);
    printP(mayor, conf.idmyjson, href_f);
    }
  printP(td_f, tr_f);

  printP(tr, td, href_i, comillas);
  pc(thttp);
  pc(iottweetcom);
  printP(comillas, b, c(newpage), mayor);
  pc(iottweett);
  printP(barraesp, t(usuario), barraesp);
  printP(c(Key), href_f, td_f, conf.iottweetenable == 1 ? th : td);
  checkBox(6, conf.iottweetenable,false);
  printP(conf.iottweetenable==1?th_f:td_f);
  printcampoC(7, conf.iottweetuser, 10, true, true, false,true);
  printcampoC(8, conf.iottweetkey, 15, true, true, false,true);
  printP(tr_f);

  printP(tr,td,c(mqtt),b);
  printP(c(tserver),td_f);
  printP(conf.mqttenabled==1?th:td);
  checkBox(9, conf.mqttenabled,false);
  printP(td_f);
  printColspan(2);
  printcampoC(10, conf.mqttserver, 40, true, true, false,false);
  printP(conf.mqttenabled==1?th_f:td_f,tr_f);
  
  for (byte i=0;i<3;i++)
    {
    printP(tr);
    printColspan(2);
    if (i==0) { printP(c(mqtt),b); pc(tpath); }
    printP(td_f,td);
    printcampoC(11+(i*2), conf.mqttpath[i*2], 10, true, true, false,false);
    printP(barra,td_f,td);
    printcampoC(12+(i*2), conf.mqttpath[i*2+1], 10, true, true, false,false);
    printP(barra,td_f,tr_f); 
    }
  writeFooter(guardarexportar, false);
  serversend200();
}

void ICACHE_FLASH_ATTR setupSegHTML()
{
  if (!autOK()) { sendOther(loghtm,-1); return; }
  msg=vacio;
  mp=1;
  char passDevtemp1[20];
  char passDevtemp2[20];
  if (server.method()==HTTP_POST)
    {
    conf.usepassDev = 0;
    for (int i=0; i<server.args(); i++)
      {
      calcindices(i);
      if (param_number==0) conf.usepassDev = server.arg(i).toInt();
      else if (param_number==1) server.arg(i).toCharArray(conf.userDev, 20);
      else if (param_number==2) server.arg(i).toCharArray(passDevtemp1, 20);
      else if (param_number==3) server.arg(i).toCharArray(passDevtemp2, 20);
      }
    if (conf.usepassDev)    // contraseña activa
      {
      if (strcmp(passDevtemp1, passDevtemp2)==0)   // si coinciden ambas password se almacena
        strcpy(conf.passDev, passDevtemp1);
      else
        conf.usepassDev=0; // no se guarda y se desactiva contraseña
      }
    else    // contraseña NO activa
      if (strcmp(passDevtemp1, conf.passDev) != 0)  // si no se da la contraseña correcta, no se desactiva
        conf.usepassDev = 1;
    saveconf();
    sendOther(sshtm,-1); return;
    }

  /////////////////////
  writeHeader(false,false);
  writeMenu(4, 5);
  writeForm(sshtm);

  printP(tr, td, t(autenticacion), td_f, conf.usepassDev ? th : td);
  checkBox(0, conf.usepassDev,false);
  if (conf.usepassDev) printP(th_f, tr_f); else printP(td_f, tr_f);
  printparCP(t(usuario), 1, conf.userDev, 20, false);
  printparCP(t(contrasena), 2, "", 20, true);
  printparCP(t(confcontrasena), 3, "", 20, true);
  writeFooter(guardar, false);
  serversend200();
}

void ICACHE_FLASH_ATTR scanapHTML()
{
  nAPact=0;
  nAP=WiFi.scanNetworks(false, false);
  msg=vacio;
  writeHeader(false,false);
  printP(menor,table, b);
  printP(c(tclass), ig, tnormal, mayor);
  for (int i=0; i<nAP; i++)
    {
    WiFi.SSID(i).toCharArray(auxchar, 20);
    printP(tr, td);
    printP(href_i, syhtm,interr,letran,ig);
    printI(i);
    printP(amper, letras,letrai, ig, cero, mayor);
    printP(auxchar, td_f, td);
    printI(WiFi.RSSI(i));
    printP(b, c(dbm), td_f, tr_f);
    }
  printP(menor, barra, table, mayor);  
  printP(c(body_f), menor, barra);
  printP(thtml, mayor);
  serversend200();
}

void initupdateserver()
{
  server.on("/firm", HTTP_GET, []() {
    server.sendHeader("Connection", "close");
    server.send(200, "text/html", serverIndex);
  });
  server.on("/update", HTTP_POST, []() {
    server.sendHeader("Connection", "close");
    server.send(200, "text/plain", (Update.hasError())?"FAIL":"OK");
    ESP.restart();
  }, []() {
    HTTPUpload& upload = server.upload();
    if (upload.status == UPLOAD_FILE_START) {
      Serial2.setDebugOutput(true);
      Serial2.printf("Update: %s\n", upload.filename.c_str());
      clearTFT();
      tft.setTextSize(2);
      tft.drawString("Updating firmware...",0,20);
      tft.drawString("No apague el equipo",0,40);
      if (!Update.begin()) { //start with max available size
        Update.printError(Serial2);
      }
    } else if (upload.status == UPLOAD_FILE_WRITE) {
      if (Update.write(upload.buf, upload.currentSize) != upload.currentSize) {
        Update.printError(Serial2);
      }
    } else if (upload.status == UPLOAD_FILE_END) {
      if (Update.end(true)) { //true to set the size to the current progress
        Serial2.printf("Update Success: %u\nRebooting...\n", upload.totalSize);
      } else {
        Update.printError(Serial);
      }
      Serial2.setDebugOutput(false);
    }
  });
}

void ICACHE_FLASH_ATTR espsysHTML()
{
  msg=vacio;
  writeHeader(false,false);
  writeMenu(4, 4);
  printP(menor,table, b);
  printP(c(tclass), ig, tnormal, mayor);
  printP(tr, td, c(Time), td_f, td); printtiempo(millis() / 1000); printP(td_f, tr_f);
//  printP(tr, td, c(Chipid), td_f, td); printL(system_get_chip_id()); printP(td_f, tr_f);
//  printP(tr, td, c(ChipFlashSize), td_f, td);  printL(ESP.getFlashChipRealSize()); printP(td_f, tr_f);
//  printP(tr, td, c(Chipspeed), td_f, td);  printL(ESP.getFlashChipSpeed()); printP(td_f, tr_f);
//  printP(tr, td, c(sdkversion), td_f, td); printP(system_get_sdk_version()); printP(td_f, tr_f);
//  printP(tr, td, c(vdd33), td_f, td); printL(system_get_vdd33()); printP(td_f, tr_f);
//  printP(tr, td, c(adc_read), td_f, td); printL(system_adc_read()); printP(td_f, tr_f);
//  printP(tr, td, c(boot_version), td_f, td); printL(system_get_boot_version());   printP(td_f, tr_f);
//  printP(tr, td, c(Time), td_f, td); printI(conf.wifimode); printP(td_f, tr_f);
//  printP(tr, td, c(boot_mode), td_f, td); printI(system_get_boot_mode());   printP(td_f, tr_f);
//  printP(tr, td, c(userbin_addr), td_f, td); printL(system_get_userbin_addr());   printP(td_f, tr_f);
//  printP(tr, td, c(cpu_freq), td_f, td); printL(system_get_cpu_freq());   printP(td_f, tr_f);
//  printP(tr, td, c(flash_get_id), td_f, td); printL(spi_flash_get_id());   printP(td_f, tr_f);
//  printP(tr, td, c(opmode), td_f, td); printI(wifi_get_opmode());   printP(td_f, tr_f);
//  printP(tr, td, c(opmode_default), td_f, td); printI(wifi_get_opmode_default());   printP(td_f, tr_f);
//  printP(tr, td, c(auto_connect), td_f, td); printI(wifi_get_opmode());   printP(td_f, tr_f);
//  printP(tr, td, c(sleep_type), td_f, td); printI(wifi_get_sleep_type());   printP(td_f, tr_f);
//  printP(tr, td, c(broadcast_if), td_f, td); printI(wifi_get_broadcast_if());   printP(td_f, tr_f);
//  printP(tr, td, c(user_limit_rate_mask), td_f, td); printL(wifi_get_user_limit_rate_mask());   printP(td_f, tr_f);
//  printP(tr, td, c(channelt), td_f, td); printI(wifi_get_channel());   printP(td_f, tr_f);
//  printP(tr, td, c(dhcps_status), td_f, td); printI(wifi_softap_dhcps_status());   printP(td_f, tr_f);
//  printP(tr, td, c(phy_mode), td_f, td); printI(wifi_get_phy_mode());   printP(td_f, tr_f);
  if (conf.wifimode!=1)
    {
//    printP(tr, td, c(connect_status), td_f, td); printI(wifi_station_get_connect_status());   printP(td_f, tr_f);
//    printP(tr, td, c(hostnamet), td_f, td); printP(wifi_station_get_hostname());   printP(td_f, tr_f);
//    printP(tr, td, c(station_num), td_f, td); printI(wifi_softap_get_station_num());   printP(td_f, tr_f);
//    printP(tr, td, c(get_current_ap_id), td_f, td); printI(wifi_station_get_current_ap_id());   printP(td_f, tr_f);
    }
  printP(menor, barra, table, mayor);  
  printP(c(body_f), menor, barra);
  printP(thtml, mayor);
  serversend200();
}

void ICACHE_FLASH_ATTR loginHTML()
{
  msg=vacio;
  if (server.method()==HTTP_POST)
    {
    if (server.hasArg("0") && server.hasArg("1"))
      {
      if ((server.arg(0)==conf.userDev) && (server.arg(1)==conf.passDev))
        { setCookie(1);  return; }
      }
    }
  if (server.hasArg("DISCONNECT")) { setCookie(0); return; }

  writeHeader(false,false);
  pc(body_i);
  pc(form_action);
  printP(loghtm,comillas,b);
  pc(Form_post);
  printP(menor,table,b);
  printP(c(tclass), ig, tnormal, mayor);
  printparCP(t(usuario), 0, conf.userDev, 20, false);
  printparCP(t(contrasena), 1, "", 20, true);
  printP(menor, barra, table, mayor);
  printP(menor, c(tinput), b, type, ig, comillas);
  printP(submit, comillas, b, tvalue, ig, comillas);
  printP(tguardar, comillas, mayor);
  printP(menor, barra, c(tinput), mayor);
  pc(form_f);
  pc(body_f);
  printP(menor, barra, thtml, mayor);
  serversend200();
}

void initFab(void)
{
  dPrint(t(reiniciando)); dPrint(b); dPrint(t(fabrica)); dPrint(crlf);
 // initConf();                  // variables de estructura Conf
  resetWiFi();                 // WiFi y Red
 // initPRG();                   // PROGRAMAS
  saveconf();
}

void ICACHE_FLASH_ATTR resetHTML()
{
  msg=vacio;
  if (server.method() == HTTP_POST)
    {
    for (int i=0; i<server.args(); i++)
      {
      if (server.argName(i).toInt()==0)
        {
        boolean idaccion = server.arg(i).toInt();
        if (idaccion > 0)
          {
          writeHeader(false,false);
          server.sendHeader("Connection", "close");
          server.sendHeader("Access-Control-Allow-Origin", "*");
          server.send(200, "text/html", espere);
          if (idaccion==1)      { ESP.restart(); }
          else if (idaccion==2) { ESP.restart(); }
          else if (idaccion==3) { resetWiFi();  }
          else if (idaccion==4) { initFab(); ESP.restart(); }
          }
        }
      }
    return;
    }
  writeHeader(false,false);
  writeMenu(4, 2);
  writeForm(rshtm);
  printP(tr);
  printP(td, treset, barra);
  printP(trestart, td_f);
  printcampoCB(0, 0, nohacernada, treset, trestart, tresetwifi,tresetfab,true);
  printP(tr_f);
  writeFooter(ejecutar, false);
  serversend200();
}

void handleStateTime() { msg=vacio; HtmlGetStateTime(); serversend200();  }
void handleStateData0() { msg=vacio; HtmlGetStateData(0); serversend200(); }
void handleStateData1() { msg=vacio; HtmlGetStateData(1); serversend200(); }
void handleStateData2() { msg=vacio; HtmlGetStateData(2); serversend200(); }
void handleStateData3() { msg=vacio; HtmlGetStateData(3); serversend200(); }
void handleStateData4() { msg=vacio; HtmlGetStateData(4); serversend200(); }
void handleStateData5() { msg=vacio; HtmlGetStateData(5); serversend200(); }
void handleStateData6() { msg=vacio; HtmlGetStateData(6); serversend200(); }
void handleStateData7() { msg=vacio; HtmlGetStateData(7); serversend200(); }

void handleStateDataf0() { msg=vacio; HtmlGetStateData(0); serversend200(); }
void handleStateDataf1() { msg=vacio; HtmlGetStateData(1); serversend200(); }
void handleStateDataf2() { msg=vacio; HtmlGetStateData(2); serversend200(); }
void handleStateDataf3() { msg=vacio; HtmlGetStateData(3); serversend200(); }
void handleStateDataf4() { msg=vacio; HtmlGetStateData(4); serversend200(); }
void handleStateDataf5() { msg=vacio; HtmlGetStateData(5); serversend200(); }
void handleStateDataf6() { msg=vacio; HtmlGetStateData(6); serversend200(); }
void handleStateDataf7() { msg=vacio; HtmlGetStateData(7); serversend200(); }

void initHTML()
{
  server.onNotFound (htmlNotFound);
  initupdateserver();
 // server.on("/f", filesHTML);
 // server.on("/sy", systemHTML);
  
  /**if (!checkfiles()) { server.on("/", filesHTML); return;  }*/
  server.on("/", indexHTML);
  server.on("/p", panelHTML);
  server.on("/sd", setupDevHTML);
  server.on("/sne", setupNetHTML);
  server.on("/sc", scanapHTML);
  server.on("/sns", setupNetServHTML);
  server.on("/es", espsysHTML);
  server.on("/l", loginHTML);
  server.on("/ss", setupSegHTML);
  server.on("/rs", resetHTML);
  server.on("/sb", setupBandHTML);
/* server.on("/dw", downloadHTML);
  server.on("/j", jsonHTML);
  server.on("/jc", jsonconfHTML);
  server.on("/je", jsonextHTML);
  server.on("/on", onCmd);
  server.on("/of", offCmd);**/
  /**server.on("/rj", rjsonHTML);
  server.on("/rjc", rjsonconfHTML);
  server.on("/rf", setuprfHTML);
  server.on("/sbp", setupbyPanelHTML);
  server.on("/sdr", setupDevRemHTML);
  server.on("/sdrio", setupDevRemioHTML);
  server.on("/se", setupEscHTML);
  server.on("/sf", setupFecHTML);
  server.on("/sio", setupioHTML);
  server.on("/sp", setupPanelHTML);
  server.on("/spr", setupPrgHTML);
  server.on("/sr", setupremHTML);
  server.on("/s150", setupdev150HTML);
  server.on("/ssr", setupsalremHTML);
  server.on("/sse", setupSemHTML);
  server.on("/sv", setupEveHTML);
  server.on("/swc", setupWebCallHTML);
  server.on("/t", termostatoHTML);
  server.on("/v", voicecommandHTML);
  **/
  
  server.on("/l0", handleStateData0);       // data
  server.on("/l1", handleStateData1);       // data
  server.on("/l2", handleStateData2);       // data
  server.on("/l3", handleStateData3);       // data
  server.on("/l4", handleStateData4);       // data
  server.on("/l5", handleStateData5);       // data
  server.on("/l6", handleStateData6);       // data
  server.on("/l7", handleStateData7);       // data
  
  server.on("/f0", handleStateDataf0);       // data fast
  server.on("/f1", handleStateDataf1);       // data fast
  server.on("/f2", handleStateDataf2);       // data fast
  server.on("/f3", handleStateDataf3);       // data fast
  server.on("/f4", handleStateDataf4);       // data fast
  server.on("/f5", handleStateDataf5);       // data fast
  server.on("/f6", handleStateDataf6);       // data fast
  server.on("/f7", handleStateDataf7);       // data fast

  server.on("/tt", handleStateTime);         // Pie
}
