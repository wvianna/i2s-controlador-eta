#include<WiFi.h>
#include<PCF8574.h>
#include<Adafruit_ADS1015.h>
#include<ModbusIP_ESP8266.h>
#include<WebServer.h>
#include<EEPROM.h>

PCF8574 expansor(0x20); //definindo que o pcf8574 sera chamado como expansor
Adafruit_ADS1115 analogico; //definindo que o ads sera chamado como analogico
ModbusIP modb;//definindo que as functions relacionadas ao ModBus serao precedida por modb

WebServer server(80);

//Funções do Watchdog
hw_timer_t *timer = NULL;
void IRAM_ATTR resetModule() {
  ets_printf("Reiniciando devido ao (watchdog)\n");
  esp_restart();
}


//Declaração de constantes relacionadas aos pinos de entrada
#define HS_REMOTO 34 //PINO 13 QUANDO TROCAR PRO UM ESP EM QUE O PINO NAO ESTEJA QUEIMADO E ATIVAR HS_508 NO CODIGO

#define HS_500 12
#define HS_501 14
#define HS_502 27
#define HS_504 26
#define HS_505 25
#define HS_503 33
#define HS_506 32
#define HS_507 35
#define HS_508 34
#define PSH_100 39
#define PSH_101 36
#define LSL_203 23
#define LSL_204 19 //FIOS PRETO E AZUL
#define LSH_206 18


//Declaração de constantes relacionadas aos pinos de saida
#define MB_500 15
#define MB_501 2
#define XV_502 4
#define MB_504 16
#define MB_505 17
#define XV_503 P0
#define MB_506 P1
#define MB_507 P2
#define MB_508 P3
#define LLL_200 P4
#define LLH_200 P5
#define LLL_205 P6
#define LLH_205 P7

//Variaveis de login
char userLog[30], userPass[30];

//Variaveis de rede
char ssid[30], password[30], serverIP[30], servergate[30], servermask[30], serverDNS[30];

//Variaveis de operação
char selecao[30], lt200Alto[30], lt200Baixo[30], lt205Baixo[30], lt205Alto[30];

//Memoria do botao virtual AutoMan
char AutoManEEPROM[30];

//Configurações do AP
IPAddress ipAP(10, 80, 0, 128);
IPAddress gatewayAP(10, 80, 0, 1);
IPAddress subnetAP(255, 255, 0, 0);
const char* ssidAP     = "AP-ETA";
const char* passwordAP = "c4rn31r0";

//Configurações WiFi
IPAddress secondaryDNS(8, 8, 4, 4);
int vetorIP[4] = {0, 0, 0, 0}, vetorGateway[4] = {0, 0, 0, 0}, vetorSubnet[4] = {0, 0, 0, 0}, vetorDNS[4] = {0, 0, 0, 0};

//Declaração de variaveis virtuais
bool AutoMan; //true-->Automatico e false-->Manual
bool botaoAutoMan;
bool alterna_bombas = 0; //Variavel virtual que permite a troca entre as bomodbas de captação
bool processando, processo2, descartando, distribuindo, manual = 0;
bool captando, protege_MB_500, protege_MB_501;
int16_t nivel_LT_200, nivelBaixo_LT_200, nivelAlto_LT_200, nivel_LT_205, nivelBaixo_LT_205, nivelAlto_LT_205, ultimaMedida_LT_200; //Variaveis relacionadas aos tanques
bool estado_LSL_203, estado_LSL_204, estado_LSH_206, estado_PSH_100, estado_PSH_101;
bool estado_HS_500, estado_HS_501, estado_HS_502, estado_HS_504, estado_HS_505, estado_HS_503, estado_HS_506, estado_HS_507, estado_HS_508 = 0, estado_HS_Remoto; //Variaveis para operação manual
bool retorno_MB_500, retorno_MB_501, retorno_XV_502, retorno_MB_504, retorno_MB_505, retorno_XV_503, retorno_MB_506, retorno_MB_507, retorno_MB_508 = 0; //Variaveis para armazenar retorno da contatora
bool estado_MB_500, estado_MB_501, estado_XV_502, estado_MB_504, estado_MB_505, estado_XV_503, estado_MB_506, estado_MB_507, estado_MB_508 = 0; //Variaveis para operação e monitoramento
bool estado_LLL_200, estado_LLH_200, estado_LLL_205, estado_LLH_205; //Variaveis para operação e monitoramento

int tempoDeTroca;//Variavel para Debounce
int alertaFluxo;
long int tempo, tempodechecagem, tempoanalogico, tempomodbus, tempoMeio_LT_200, tempoMeio_LT_205, tempoDeFluxo, tempowifi;
int pct_LT_200, pct_LT_205;
String estadoCaptando, estadoProcessando, estadoDistribuindo, estadoAutoMan, estadoLocalRemoto;


void setup() {

  //----------------Watchdog---------
  timer = timerBegin(0, 80, true); //utiliza timer0,frequencia(80MHZ) dividida por 80, ativa contador
  timerAttachInterrupt(timer, &resetModule, true); //confere variavel timer, se acionado chama a função resetModule, deixa a interrupção ativada
  timerAlarmWrite(timer, 15000000, true); //Configurado para 15s --> 80MHZ/80 = 1MHZ --> 1/1MHZ=1us
  timerAlarmEnable(timer);
  //---------------------------------

  pinMode(HS_REMOTO, INPUT);
  pinMode(HS_500, INPUT);
  pinMode(HS_501, INPUT);
  pinMode(HS_502, INPUT);
  pinMode(HS_504, INPUT);
  pinMode(HS_505, INPUT);
  pinMode(HS_503, INPUT);
  pinMode(HS_506, INPUT);
  pinMode(HS_507, INPUT);
  pinMode(HS_508, INPUT);
  pinMode(HS_506, INPUT);
  pinMode(PSH_100, INPUT);
  pinMode(PSH_101, INPUT);
  pinMode(LSL_203, INPUT);
  pinMode(LSL_204, INPUT);
  pinMode(LSH_206, INPUT);

  pinMode(MB_500, OUTPUT);
  pinMode(MB_501, OUTPUT);
  pinMode(XV_502, OUTPUT);
  pinMode(MB_504, OUTPUT);
  pinMode(MB_505, OUTPUT);
  expansor.pinMode(XV_503, OUTPUT);
  expansor.pinMode(MB_506, OUTPUT);
  expansor.pinMode(MB_507, OUTPUT);
  expansor.pinMode(MB_508, OUTPUT);
  expansor.pinMode(LLL_200, OUTPUT);
  expansor.pinMode(LLH_200, OUTPUT);
  expansor.pinMode(LLL_205, OUTPUT);
  expansor.pinMode(LLH_205, OUTPUT);


  //Comunicações Seriais
  Serial.begin(9600);
  expansor.begin();
  analogico.begin();
  EEPROM.begin(2048);


  //Iniciar a EEPROM
  lerEEPROM();
  //Configurações do WiFi
  conectarWifi();

  //Valores Iniciais para os niveis quando ainda não tiver um valor salvo na memoria
  if (EEPROM.read(210) == 255) {
    String dado;
    dado = "30";
    dado.toCharArray(lt200Baixo, 30);
  }
  if (EEPROM.read(240) == 255) {
    String dado;
    dado = "70";
    dado.toCharArray(lt200Alto, 30);
  }
  if (EEPROM.read(270) == 255) {
    String dado;
    dado = "30";
    dado.toCharArray(lt205Baixo, 30);
  }
  if (EEPROM.read(300) == 255) {
    String dado;
    dado = "70";
    dado.toCharArray(lt205Alto, 30);
  }
  //Valores Iniciais de Login e Senha quando ainda não tiver um valor salvo na memória

  if (EEPROM.read(330) == 255) {
    String dado;
    dado = "admin";
    dado.toCharArray(userLog, 30);
  }
  if (EEPROM.read(360) == 255) { //Valor Iniciais de senha quando ainda não tiver um valor cadastrado na memória
    String dado;
    dado = "12345678";
    dado.toCharArray(userPass, 30);
  }
  if(String(AutoManEEPROM) == "Manual"){
    AutoMan=false;
    }
  else AutoMan=true;

  //Start e Registradores Modbus
  modb.slave();
  modb.addHreg(1);//HOLDING REGISTER [1] será o endereço do primeiro Sensor de nível(ADS1115 A0-A1)
  modb.addHreg(2);//HOLDING REGISTER [2] será o endereço do segundo Sensor de nível(ADS1115 A2-A3)
  modb.addCoil(0); //COIL[0] leitura do HS-REMOTO
  modb.addCoil(1); //COIL[1] será referente a MB-500
  modb.addCoil(2); //COIL[2] será referente a MB-501
  modb.addCoil(3); //COIL[3] será referente a MB-504
  modb.addCoil(4); //COIL[4] será referente a MB-505
  modb.addCoil(5); //COIL[5] será referente a MB-506
  modb.addCoil(6); //COIL[6] será referente a MB-507
  modb.addCoil(7); //COIL[7] será referente a MB-508
  modb.addCoil(8); //COIL[8] será referente a XV-502
  modb.addCoil(9); //COIL[9] será referente a XV-503
  modb.addCoil(10); //COIL[10] será referente a LLL-200
  modb.addCoil(11); //COIL[11] será referente a LLH-200
  modb.addCoil(12); //COIL[12] será referente a LLL-205
  modb.addCoil(13); //COIL[13] será referente a LLH-205
  modb.addCoil(14); //COIL[14] endereço livre
  modb.addCoil(15); //COIL[15] endereço livre
  modb.addCoil(16); //COIL[16] será referente ao botaoAutoMan
  modb.addCoil(17); //COIL[17] será referente a HS-500
  modb.addCoil(18); //COIL[18] será referente a HS-501
  modb.addCoil(19); //COIL[19] será referente a HS-504
  modb.addCoil(20); //COIL[20] será referente a HS-505
  modb.addCoil(21); //COIL[21] será referente a HS-506
  modb.addCoil(22); //COIL[22] será referente a HS-507
  modb.addCoil(23); //COIL[23] será referente a HS-508
  modb.addCoil(24); //COIL[24] será referente a HS-502
  modb.addCoil(25); //COIL[25] será referente a HS-503
  modb.addCoil(26); //COIL[26] será referente a PSH-100
  modb.addCoil(27); //COIL[27] será referente a PSH-101
  modb.addCoil(28); //COIL[28] será referente a HS-AUTO
  modb.addCoil(29); //COIL[29] será referente a LSL-203
  modb.addCoil(30); //COIL[30] será referente a LSL-204
  modb.addCoil(31); //COIL[31] será referente a LSH-206
  modb.addCoil(32); //COIL[32] será referente ao AutoMan
  modb.addCoil(33); //COIL[33] será referente ao Retorno da MB-500
  modb.addCoil(34); //COIL[34] será referente ao Retorno da MB-501
  modb.addCoil(35); //COIL[35] será referente ao Retorno da MB-504
  modb.addCoil(36); //COIL[36] será referente ao Retorno da MB-505
  modb.addCoil(37); //COIL[37] será referente ao Retorno da MB-506
  modb.addCoil(38); //COIL[38] será referente ao Retorno da MB-507
  modb.addCoil(39); //COIL[39] será referente ao Retorno da MB-508
  modb.addCoil(40); //COIL[40] será referente ao Retorno da XV-502
  modb.addCoil(41); //COIL[41] será referente ao Retorno da XV-503



  server.on("/", paginaInicial);

  server.on("/operacao", HTTP_POST, paginaOpera);

  server.on("/auto-man", HTTP_POST, []() {
    Processo_AutoMan();
    paginaOpera();
  });

  server.on("/MB-500", HTTP_POST, []() {
    Processo_MB_500();
    paginaOpera();
  });

  server.on("/MB-501", HTTP_POST, []() {
    Processo_MB_501();
    paginaOpera();
  });

  server.on("/XV-502", HTTP_POST, []() {
    Processo_XV_502();
    paginaOpera();
  });

  server.on("/XV-503", HTTP_POST, []() {
    Processo_XV_503();
    paginaOpera();
  });

  server.on("/MB-504", HTTP_POST, []() {
    Processo_MB_504();
    paginaOpera();
  });

  server.on("/MB-505", HTTP_POST, []() {
    Processo_MB_505();
    paginaOpera();
  });

  server.on("/MB-506", HTTP_POST, []() {
    Processo_MB_506();
    paginaOpera();
  });

  server.on("/MB-507", HTTP_POST, []() {
    Processo_MB_507();
    paginaOpera();
  });

  server.on("/MB-508", HTTP_POST, []() {
    Processo_MB_508();
    paginaOpera();
  });

  server.on("/login", HTTP_POST, paginaAdministrar);

  server.on("/trocarconfig", HTTP_POST, atualizarConfigs);

  server.on("/lt200", []() {
    server.send(200, "text/html", String(pct_LT_200));
  });
  server.on("/lt205", []() {
    server.send(200, "text/html", String(pct_LT_205));
  });
  server.on("/statusCaptacao", []() {
    server.send(200, "text/html", estadoCaptando);
  });
  server.on("/statusProcesso", []() {
    server.send(200, "text/html", estadoProcessando);
  });
  server.on("/statusDistribuicao", []() {
    server.send(200, "text/html", estadoDistribuindo);
  });
  server.on("/statusLocalRemoto", []() {
    server.send(200, "text/html", estadoLocalRemoto);
  });
  server.on("/statusAutoManual", []() {
    server.send(200, "text/html", estadoAutoMan);
  });

  server.on("/loginInfo", []() {
    server.send(200, "text/html", String(userLog));
  });
  server.on("/senhaInfo", []() {
    server.send(200, "text/html", String(userPass));
  });
  server.on("/ssidRede", []() {
    server.send(200, "text/html", String(ssid));
  });
  server.on("/senhaRede", []() {
    server.send(200, "text/html", String(password));
  });
  server.on("/ipRede", []() {
    server.send(200, "text/html", String(serverIP));
  });
  server.on("/maskRede", []() {
    server.send(200, "text/html", String(servermask));
  });
  server.on("/gatewayRede", []() {
    server.send(200, "text/html", String(servergate));
  });
  server.on("/dnsRede", []() {
    server.send(200, "text/html", String(serverDNS));
  });
  server.on("/bombaOp", []() {
    server.send(200, "text/html", String(selecao));
  });
  server.on("/baixo200Op", []() {
    server.send(200, "text/html", String(lt200Baixo));
  });
  server.on("/alto200Op", []() {
    server.send(200, "text/html", String(lt200Alto));
  });
  server.on("/baixo205Op", []() {
    server.send(200, "text/html", String(lt205Baixo));
  });
  server.on("/alto205Op", []() {
    server.send(200, "text/html", String(lt205Alto));
  });
  server.on("/estadoMb500", []() {
    server.send(200, "text/html", String(estado_MB_500));
  });
  server.on("/estadoMb501", []() {
    server.send(200, "text/html", String(estado_MB_501));
  });
  server.on("/estadoXv502", []() {
    server.send(200, "text/html", String(estado_XV_502));
  });
  server.on("/estadoMb504", []() {
    server.send(200, "text/html", String(estado_MB_504));
  });
  server.on("/estadoMb505", []() {
    server.send(200, "text/html", String(estado_MB_505));
  });
  server.on("/estadoXv503", []() {
    server.send(200, "text/html", String(estado_XV_503));
  });
  server.on("/estadoMb507", []() {
    server.send(200, "text/html", String(estado_MB_507));
  });
  server.on("/estadoMb506", []() {
    server.send(200, "text/html", String(estado_MB_506));
  });
  server.on("/estadoMb508", []() {
    server.send(200, "text/html", String(estado_MB_508));
  });
  server.on("/estadoAutoMan", []() {
    server.send(200, "text/html", String(AutoMan));
  });


  server.begin();

  captando = processando = processo2 = descartando = distribuindo = false;
  atualizarNiveis();
  tempo = 0;
  tempodechecagem = 0;
  tempoanalogico = 0;
  tempoMeio_LT_200 = 0;
  tempoMeio_LT_205 = 0;
  tempomodbus = 0;
  protege_MB_500 = false;
  protege_MB_501 = false;
  tempoDeFluxo = 0;
  alertaFluxo = 0;
  delay(200);
}

void loop() {
  server.handleClient();
  checarWifi();
  if (millis() - tempomodbus > 20) { //delay para não sobrecarregar conexão
    modb.task();//Função que atualiza o modbus
    tempomodbus = millis();
  }
  timerWrite(timer, 0); //Reseta tempo de inatividade monitorado pelo watchdog
  if (millis() - tempodechecagem > 200) {
    Checar_Tanques();
    tempodechecagem = millis();
  }
  //Função que checa as entradas(boias e pressostatos) conectados aos tanques
  //Serial.println(digitalRead(HS_REMOTO));
  if (digitalRead(HS_REMOTO) == true && AutoMan == true) {
    estadoAutoMan = "Automático";
    estadoLocalRemoto = "Remoto";
    if (millis() - tempo < 0) { //Evita problema caso o millis() volte a 0
      tempo = 0;
    }
    if ((millis() - tempo) > 1000) {
      //Serial.println("Auto");
      if (manual == true) {         //Detecta a mudança da chave AUTO/MAN
        Serial.println("Desativando Bombas e Valvulas");
        Zerar_Atuadores();          //Função para desativas as saídas na mudança da chave automatica manual
        Zerar_Estados();             //Função para zerar as variaveis na transição da chave automatica manual
        manual = false;
      }
      if (nivel_LT_200 <= nivelBaixo_LT_200 && captando == false && (protege_MB_500 == false || protege_MB_501 == false)) {
        if ((atoi(selecao) == 0 && protege_MB_500 == false) || (atoi(selecao) == 1 && protege_MB_501 == false) || atoi(selecao) == 2) {
          Serial.println("Captação Ativada"); //Mensagem para testes
          Serial.print("NIVEL LT-200 "); Serial.println(nivel_LT_200);
          Serial.print("NIVEL Baixo LT-200 "); Serial.println(nivelBaixo_LT_200);
          Iniciar_Captar();             //função que ativa bombas de captação de água bruta
        }
      }
      else if ((nivel_LT_200 >= nivelAlto_LT_200) && captando == true) {
        Serial.print("NIVEL LT 200 "); Serial.print(nivel_LT_200);
        Serial.print("NIVEL Alto LT-200 "); Serial.print(nivelAlto_LT_200);
        Serial.println("Captação Desativada");          //Mensagem para testes
        Parar_Captar();
      }

      if (nivel_LT_205 < nivelBaixo_LT_205 && (millis() - tempoMeio_LT_200 > 25000) && processando == false) {
        Serial.println("Processo Iniciado");
        Iniciar_Etapa1();             //Função que ativa Valvula inicial, bombas peristalticas e MB_505
        processando = true;
      }
      if ((nivel_LT_205 > nivelAlto_LT_205 || nivel_LT_200 < nivelBaixo_LT_200) && processando == true) {
        Serial.println("Processo Parado");
        Parar_Etapa1();
        Parar_Etapa2();
        processando = false;
      }
      if (estado_LSL_204 == false && processando == true && processo2 == false) {
        Serial.println("Etapa2 iniciada");
        Iniciar_Etapa2();
      }
      if (estado_LSL_204 == true) {
        Serial.println("Etapa2 interrompida");
        Parar_Etapa2();
      }
      if (processando == true && estado_LSL_203 == false && descartando == false) {           //Ligar boia com fios AZUL e PRETO
        Serial.println("Descarte iniciado");
        Iniciar_Descarte();
      }
      if (estado_LSL_203 == true) {            //Ligar boia com fios AZUL e PRETO
        Serial.println("Descarte interrompido");
        Parar_Descarte();
      }
      if ((millis() - tempoMeio_LT_205 > 25000) && estado_LSH_206 == false && distribuindo == false) {
        Iniciar_EtapaBlocoA();
      }
      if (nivel_LT_205 < nivelBaixo_LT_205 || (estado_LSH_206 == true && distribuindo == true)) {
        Parar_EtapaBlocoA();
      }
      tempo = millis();
    }
  }
  else if (digitalRead(HS_REMOTO) == true && AutoMan == false) {
    estadoAutoMan = "Manual";
    estadoLocalRemoto = "Remoto";
    if (manual == false) {        //Detecta a mudança da chave AUTO/MAN
      Serial.println("Manual");
      Serial.println("Desligando Bombas e Valvulas Ativadas");
      Zerar_Atuadores();          //Função para desativas as saídas na mudança da chave automatica manual
      Zerar_Estados();
      manual = true;
      captando = processando = processo2 = descartando = distribuindo = false;
    }
    Checar_Botoes();
    if (nivel_LT_200 > nivelAlto_LT_200) {           //Desliga bombas caso o tanque de agua bruta esteja cheio
      digitalWrite(MB_500, 0);
      digitalWrite(MB_501, 0);
      estado_MB_500 = 0;
      estado_MB_501 = 0;
    }
    if (nivel_LT_205 > nivelAlto_LT_205 || estado_LSL_204 == true ) {           //Desliga MB-507 caso o tanque de agua limpa esteja cheio
      expansor.digitalWrite(MB_507, 0);
      digitalWrite(MB_507, 0);
      estado_MB_507 = 0;
    }
    if (nivel_LT_205 < nivelBaixo_LT_205 || estado_LSH_206 == true ) {           //Desliga MB-508 caso o tanque de agua limpa esteja vazio
      expansor.digitalWrite(MB_508, 0);
      digitalWrite(MB_508, 0);
      estado_MB_508 = 0;
    }
    if (estado_LSL_203 == true) {             //desliga bomba de descarte caso a boia do tanque de agua suja indique queda ou subida
      estado_MB_506 = 0;
      expansor.digitalWrite(MB_506, 0);
    }
    if (digitalRead(XV_502) == false) { //Não deixar as bombas peristalticas funcionarem enquanto a valvula XV-502 estiver fechada
      digitalWrite(MB_504, 0);
      estado_MB_504 = 0;
    }
  }
  if (digitalRead(HS_REMOTO) == false) {
    estado_HS_Remoto=digitalRead(HS_REMOTO);
    estadoLocalRemoto = "Local";
    estadoAutoMan = "Manual";
  }
  
  Checar_Retornos();
  Sincroniza_Modbus();

}

/* Os niveis(altos e baixos) são ajustados em forma de porcentagem enquanto a leitura é realizada em 16bits. Para fazer essa conversão foi criada a função
    atualizarNiveis() onde sera feita uma conversão nos baseando nos valores minimos e maximos encontrados pelo trandutor de pressão(4ma a 20ma).
    Nessa fase de desenvolvimento os valores foram calculados com a utilização de um circuito contendo uma fonte de 12v e um trimpor de 10 regulado entre valores proximos
    de 200 e 600 Ohms por isso é provavel uma diferença quanto ao sensor real. Bastará ajustar os valores constantes nessa função.
*/
int v20ma = 14100; // corresponde a 20ma nos testes
int v4ma = 4100; //corresponde a 4ma nos testes

void atualizarNiveis() {
  nivelBaixo_LT_200 = int(((100 - atoi(lt200Baixo)) * v4ma) + ((atoi(lt200Baixo) - 0) * v20ma)) / 100;
  nivelBaixo_LT_205 = int(((100 - atoi(lt205Baixo)) * v4ma) + ((atoi(lt205Baixo) - 0) * v20ma)) / 100;
  nivelAlto_LT_200 = int(((100 - atoi(lt200Alto)) * v4ma) + ((atoi(lt200Alto) - 0) * v20ma)) / 100;
  nivelAlto_LT_205 = int(((100 - atoi(lt205Alto)) * v4ma) + ((atoi(lt205Alto) - 0) * v20ma)) / 100;
  Serial.println("Nivel baixo Lt200: " + String(nivelBaixo_LT_200));
  Serial.println("Nivel Alto Lt200: " + String(nivelAlto_LT_200));
  Serial.println("Nivel baixo Lt205: " + String(nivelBaixo_LT_205));
  Serial.println("Nivel Alto Lt205: " + String(nivelAlto_LT_205));

}

void splitIP() {
  //função para fazer a transcrição do vetor char do IP estático para inteiro - adaptada do código do medidor de energia

  char * sIP;
  int auxiliar = 0;
  char auxiliarIP[40];
  strcpy(auxiliarIP, serverIP)  ;
  sIP = strtok(auxiliarIP, ".");
  while (sIP != NULL)
  {
    vetorIP[auxiliar] = atoi(sIP);
    sIP = strtok(NULL, ".");
    auxiliar++;
  }
}

void splitGateway() {

  char * sGW;
  int auxiliar = 0;
  char auxiliarGateway[40];
  strcpy(auxiliarGateway, servergate);
  sGW = strtok(auxiliarGateway, ".");
  while (sGW != NULL)
  {
    vetorGateway[auxiliar] = atoi(sGW);
    sGW = strtok(NULL, ".");
    auxiliar++;
  }
}

void splitMask() {

  char * sSN;
  int auxiliar = 0;
  char auxiliarSubnet[40];
  strcpy(auxiliarSubnet, servermask);
  sSN = strtok(auxiliarSubnet, ".");
  while (sSN != NULL)
  {
    vetorSubnet[auxiliar] = atoi(sSN);
    sSN = strtok(NULL, ".");
    auxiliar++;
  }

}

void splitDNS() {

  char * sDNS;
  int auxiliar = 0;
  char auxiliarDNS[40];
  strcpy(auxiliarDNS, serverDNS);
  sDNS = strtok(auxiliarDNS, ".");
  while (sDNS != NULL)
  {
    vetorDNS[auxiliar] = atoi(sDNS);
    sDNS = strtok(NULL, ".");
    auxiliar++;
  }
}

void checarWifi() {
  if (WiFi.status() != WL_CONNECTED) {
    if (millis() - tempowifi > 60000) {
      refazWifi();
    }
  }
}

void conectarWifi() {
  WiFi.disconnect();
  splitIP();
  splitGateway();
  splitMask();
  splitDNS();
  IPAddress clientIP(vetorIP[0], vetorIP[1], vetorIP[2], vetorIP[3]);
  IPAddress clientGateway(vetorGateway[0], vetorGateway[1], vetorGateway[2], vetorGateway[3]);
  IPAddress clientMask(vetorSubnet[0], vetorSubnet[1], vetorSubnet[2], vetorSubnet[3]);
  IPAddress clientdns(vetorDNS[0], vetorDNS[1], vetorDNS[2], vetorDNS[3]);
  Serial.print(vetorIP[0]);
  Serial.print(vetorIP[1]);
  Serial.print(vetorIP[2]);
  Serial.println(vetorIP[3]);
  Serial.print(vetorGateway[0]);
  Serial.print(vetorGateway[1]);
  Serial.print(vetorGateway[2]);
  Serial.println(vetorGateway[3]);
  Serial.print(vetorSubnet[0]);
  Serial.print(vetorSubnet[1]);
  Serial.print(vetorSubnet[2]);
  Serial.println(vetorSubnet[3]);
  Serial.print(vetorDNS[0]);
  Serial.print(vetorDNS[1]);
  Serial.print(vetorDNS[2]);
  Serial.println(vetorDNS[3]);
  //const char* ssid2 = "wIFF_administrativa"; const char* password2 =  "pOLO@2016";

  WiFi.begin(ssid, password);
  WiFi.config(clientIP, clientGateway, clientMask, clientdns, secondaryDNS);
  //WiFi.config(IPAddress{10,80,0,129}, IPAddress{10,80,0,1}, IPAddress{255,255,0,0}, IPAddress{8,8,8,8},IPAddress{8,8,4,4});
  WiFi.mode(WIFI_AP_STA);
  Serial.println(WiFi.localIP());
  WiFi.softAP(ssidAP, passwordAP);
  delay(100);
  WiFi.softAPConfig(ipAP, gatewayAP, subnetAP);
  Serial.println(WiFi.softAPIP());
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("Wifi disconnected");
  }

  // Mostrando IP se conectado
  if (WiFi.status() == WL_CONNECTED)
  {
    Serial.println("");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
  }
  tempowifi = millis();
  checarWifi();
}

void refazWifi() {
  Serial.println("Restabelecendo conexão...");
  WiFi.disconnect(true);
  WiFi.begin(ssid, password);
  tempowifi = millis();
}

void Iniciar_Captar() {                 //função que ativa bombas de captação de água bruta
  if (atoi(selecao) != 0 && atoi(selecao) != 1 && atoi(selecao) != 2) {
    String temp;
    temp = "2";
    temp.toCharArray(selecao, 30);
  }
  if (atoi(selecao) == 2) {
    if (protege_MB_500 == true && protege_MB_500 == true) {
      Serial.println("Ambas as bombas foram desacionadas por falta de fluxo");
    }
    if (alterna_bombas == true && protege_MB_500 == false) {
      digitalWrite(MB_500, 1);
      Serial.println("Bomba MB-500 Selecionada");        //Mensagem para testes
      captando = true;
      tempoDeFluxo = millis();
      Serial.println("Bomba MB-500 Ligada");
    }
    else if (alterna_bombas == false && protege_MB_501 == false) {
      digitalWrite(MB_501, 1);
      Serial.println("Bomba MB-501 Selecionada");        //Mensagem para testes
      captando = true;
      tempoDeFluxo = millis();
      Serial.println("Bomba MB-501 Ligada");
    }
    alterna_bombas = !alterna_bombas; //Troca a variavel de estado permitindo a alternancia entre as bombas
  }
  if (atoi(selecao) == 0) {
    if (protege_MB_500 == true) {
      Serial.println("A Bomba MB-500 foi desacionada por falta de fluxo");
    }
    else {
      digitalWrite(MB_500, 1);
      Serial.println("Bomba MB-500 Selecionada");        //Mensagem para testes
      captando = true;
      tempoDeFluxo = millis();
      Serial.println("Bomba MB-500 Ligada");
    }
  }
  if (atoi(selecao) == 1) {
    if (protege_MB_501 == true) {
      Serial.println("A Bomba MB-501 foi desacionada por falta de fluxo");
    }
    else {
      digitalWrite(MB_501, 1);
      Serial.println("Bomba MB-501 Selecionada");        //Mensagem para testes
      captando = true;
      tempoDeFluxo = millis();
      Serial.println("Bomba MB-501 Ligada");
    }
  }
}

void Parar_Captar() {                     //função que desativa bombas de captação de água bruta
  digitalWrite(MB_500, 0);
  digitalWrite(MB_501, 0);
  Serial.println("Bombas de Captação Desligadas");    //Mensagem para testes
  captando = false;
}

void Iniciar_Etapa1() {                   //Função que ativa Valvula inicial, bombas peristalticas e MB_505
  digitalWrite(XV_502, 1);
  digitalWrite(MB_504, 1);
  digitalWrite(MB_505, 1);
  estado_XV_502 = 1;
  expansor.digitalWrite(XV_503, 1);
  estado_XV_503 = 1;
  Serial.println("Etapa 1 ativada");
}

void Parar_Etapa1() {                     //Função que desativa Valvula inicial, bombas peristalticas e MB_505
  digitalWrite(XV_502, 0);
  digitalWrite(MB_504, 0);
  digitalWrite(MB_505, 0);
  expansor.digitalWrite(XV_503, 0);
  estado_XV_502 = 0;
  estado_XV_503 = 0;
  Serial.println("Etapa 1 desativada");
}

void Iniciar_Etapa2() {                   //Função que ativa o bombeamento da agua em processo em direção aos filtros
  Serial.println("Bombeamento para os filtros iniciada");
  expansor.digitalWrite(MB_507, 1);
  estado_MB_507 = 1;
  processo2 = true;
}

void Parar_Etapa2() {                       //Função que ativa o bombeamento da agua em processo em direção aos filtros
  Serial.println("Bombeamento para os filtros interrompida");
  expansor.digitalWrite(MB_507, 0);
  estado_MB_507 = 0;
  processo2 = false;
}

void Iniciar_Descarte() {                   //Função que inicia o descarte da agua no tanque de agua suja
  Serial.println("Processo de descarte iniciado");
  expansor.digitalWrite(MB_506, 1);
  estado_MB_506 = 1;
  descartando = true;
}

void Parar_Descarte() {                     //Função que interrompe o descarte da agua no tanque de agua suja
  Serial.println("Processo de descarte interrompido");
  expansor.digitalWrite(MB_506, 0);
  estado_MB_506 = 0;
  descartando = false;
}

void Checar_Retornos() {
  retorno_MB_500 = !digitalRead(HS_500);
  retorno_MB_501 = !digitalRead(HS_501);
  retorno_XV_502 = !digitalRead(HS_502);
  retorno_MB_504 = !digitalRead(HS_504);
  retorno_MB_505 = !digitalRead(HS_505);
  retorno_XV_503 = !digitalRead(HS_503);
  retorno_MB_506 = !digitalRead(HS_506);
  retorno_MB_507 = !digitalRead(HS_507);
  //retorno_MB_508=!digitalRead(HS_508);
}

void Checar_Tanques() {                     //Função que checa as entradas(boias e pressostatos) conectados aos tanques
  //Serial.println("Checando estado das boias e sensores");
  tempoanalogico = millis();
  estado_PSH_100 = !digitalRead(PSH_100);
  estado_PSH_101 = !digitalRead(PSH_101);
  estado_LSL_203 = !digitalRead(LSL_203);
  estado_LSL_204 = !digitalRead(LSL_204);
  estado_LSH_206 = !digitalRead(LSH_206);
  nivel_LT_200 = analogico.readADC_Differential_0_1();
  nivel_LT_205 = analogico.readADC_Differential_2_3();

  //Conferir se precisa acionar algum alarme de nivel
  if (nivel_LT_200 < nivelBaixo_LT_200) {
    tempoMeio_LT_200 = millis();
    expansor.digitalWrite(LLL_200, 1);
    estado_LLL_200 = 1;
  }
  else if (estado_LLL_200 == 1) {
    expansor.digitalWrite(LLL_200, 0);
    estado_LLL_200 = 0;
  }

  if (nivel_LT_200 > nivelAlto_LT_200) {
    expansor.digitalWrite(LLH_200, 1);
    estado_LLH_200 = 1;
  }
  else if (estado_LLH_200 == 1) {
    expansor.digitalWrite(LLH_200, 0);
    estado_LLH_200 = 0;
  }

  if (nivel_LT_205 < nivelBaixo_LT_205) {
    tempoMeio_LT_205 = millis();
    expansor.digitalWrite(LLL_205, 1);
    estado_LLL_205 = 1;
  }
  else if (estado_LLL_205 == 1) {
    expansor.digitalWrite(LLL_205, 0);
    estado_LLL_205 = 0;
  }

  if (nivel_LT_205 > nivelAlto_LT_205) {
    expansor.digitalWrite(LLH_205, 1);
    estado_LLH_205 = 1;
  }
  else if (estado_LLH_205 == 1) {
    expansor.digitalWrite(LLH_205, 0);
    estado_LLH_205 = 0;
  }

  //Confere o Fluxo a cada 30s
  if (millis() - tempoDeFluxo > 30000) {
    if (captando == true) {
      Checar_Fluxo();
    }
  }
  pct_LT_200 = int((nivel_LT_200 - v4ma) * 100) / (v20ma - v4ma); //Corrigir valores medidos no transdutor de nivel para ajustar fórmula
  pct_LT_205 = int((nivel_LT_205 - v4ma) * 100) / (v20ma - v4ma); //Corrigir valores medidos no transdutor de nivel para ajustar fórmula
  if (captando == true)
    estadoCaptando = "Sim";
  else {
    estadoCaptando = "Não";
    if (protege_MB_500 == true && protege_MB_501 == false)
      estadoCaptando = "Bomba MB-500 sem fluxo";
    else if (protege_MB_500 == false && protege_MB_501 == true)
      estadoCaptando = "Bomba MB-501 sem fluxo";
    else if (protege_MB_500 == true && protege_MB_501 == true)
      estadoCaptando = "Bombas MB-500 e MB-501 sem fluxo";
  }
  if (processando == true || processo2 == true)
    estadoProcessando = "Sim";
  else
    estadoProcessando = "Não";
  if (distribuindo == true)
    estadoDistribuindo = "Sim";
  else
    estadoDistribuindo = "Não";
}

void Iniciar_EtapaBlocoA() {                //Função que inicia o processo de distribuição de agua para o blocoA
  expansor.digitalWrite(MB_508, 1);
  estado_MB_508 = 1;
  distribuindo = 1;
}

void Parar_EtapaBlocoA() {                  //Função que interrompe o processo de distribuição de agua para o blocoA
  expansor.digitalWrite(MB_508, 0);
  estado_MB_508 = 0;
  distribuindo = 0;
}

void Zerar_Atuadores() {                    //Função para desativas as saídas na mudança da chave automatica manual
  digitalWrite(MB_500, 0);
  digitalWrite(MB_501, 0);
  digitalWrite(XV_502, 0);
  digitalWrite(MB_504, 0);
  digitalWrite(MB_505, 0);
  expansor.digitalWrite(XV_503, 0);
  expansor.digitalWrite(MB_506, 0);
  expansor.digitalWrite(MB_507, 0);
  expansor.digitalWrite(MB_508, 0);
}

void Zerar_Estados() {                       //Função para zerar as variaveis na transição da chave automatica manual
  estado_HS_500 = estado_HS_501 = estado_HS_502 = estado_HS_504 = estado_HS_505 = estado_HS_503 = estado_HS_506 = estado_HS_507 = estado_HS_508 = 0;
  estado_MB_500 = estado_MB_501 = estado_XV_502 = estado_MB_504 = estado_MB_505 = estado_XV_503 = estado_MB_506 = estado_MB_507 = estado_MB_508 = 0;
}

void Checar_Botoes() {                      //Função que identifica se algum botão foi pressionado
  if (digitalRead(HS_REMOTO) == true) {
    estado_HS_500 = (modb.Coil(17));
    if (estado_HS_500 == true) {
      if (millis() - tempoDeTroca > 800) {
        Processo_MB_500();
        tempoDeTroca = millis();
      }
    }
    estado_HS_501 = (modb.Coil(18));
    if (estado_HS_501 == true) {
      if (millis() - tempoDeTroca > 800) {
        Processo_MB_501();
        tempoDeTroca = millis();
      }
    }
    estado_HS_502 = (modb.Coil(24));      //botao da valvula de descida
    if (estado_HS_502 == true) {
      if (millis() - tempoDeTroca > 800) {
        Processo_XV_502();
        tempoDeTroca = millis();
      }
    }
    estado_HS_504 = (modb.Coil(19));         //botao das bombas peristalticas
    if (estado_HS_504 == true) {
      if (millis() - tempoDeTroca > 800) {
        Processo_MB_504();
        tempoDeTroca = millis();
      }
    }
    estado_HS_505 = (modb.Coil(20));   //botao do MB_505
    if (estado_HS_505 == true) {
      if (millis() - tempoDeTroca > 800) {
        Processo_MB_505();
        tempoDeTroca = millis();
      }
    }
    estado_HS_503 = (modb.Coil(25));         //botao da valvula que leva ao tanque de agua suja
    if (estado_HS_503 == true) {
      if (millis() - tempoDeTroca > 800) {
        Processo_XV_503();
        tempoDeTroca = millis();
      }
    }
    estado_HS_506 = (modb.Coil(21));             //botao da bomba de descarte
    if (estado_HS_506 == true) {
      if (millis() - tempoDeTroca > 800) {
        Processo_MB_506();
        tempoDeTroca = millis();
      }
    }
    estado_HS_507 = (modb.Coil(22));             //botao da bomba que leva aos filtros
    if (estado_HS_507 == true) {
      if (millis() - tempoDeTroca > 800) {
        Processo_MB_507();
        tempoDeTroca = millis();
      }
    }
    botaoAutoMan = modb.Coil(16);
    if (botaoAutoMan == true) {
      if (millis() - tempoDeTroca > 800) {
        Processo_AutoMan();
        tempoDeTroca = millis();
      }
    }
    /*estado_HS_508 = (modb.Coil(23));             //botao da bomba que enche o tanque do bloco A (QUANDO HS_REMOTO FOR CONFIGURADO PARA O PINO 13 PODE DESCOMENTAR)
      if (estado_HS_508 == true) {
        if (millis() - tempoDeTroca > 800) {
          Processo_MB_508();
          tempoDeTroca = millis();
        }
      }*/
  }
}

void Processo_MB_500() {                                //processo que ativa manualmente a MB_500
  estado_MB_500 = !estado_MB_500;
  digitalWrite(MB_500, estado_MB_500);
}

void Processo_MB_501() {                                //processo que ativa manualmente a MB_501
  estado_MB_501 = !estado_MB_501;
  digitalWrite(MB_501, estado_MB_501);
}

void Processo_XV_502() {                              //processo que ativa manualmente a valvula de descida
  estado_XV_502 = !estado_XV_502;
  digitalWrite(XV_502, estado_XV_502);
}

void Processo_MB_504() {                               //processo que ativa manualmente as bombas peristalticas
  estado_MB_504 = !estado_MB_504;
  digitalWrite(MB_504, estado_MB_504);
}

void Processo_MB_505() {                            //processo que ativa manualmente o MB_505
  estado_MB_505 = !estado_MB_505;
  digitalWrite(MB_505, estado_MB_505);
}

void Processo_XV_503() {                              //processo que ativa manualmente a valvula que vai ao tanque de agua suja
  estado_XV_503 = !estado_XV_503;
  expansor.digitalWrite(XV_503, estado_XV_503);
}

void Processo_MB_506() {                                //processo que ativa manualmente a bomba de descarte
  estado_MB_506 = !estado_MB_506;
  expansor.digitalWrite(MB_506, estado_MB_506);
}

void Processo_MB_507() {                                //processo que ativa manualmente a bomba que leva aos filtros
  estado_MB_507 = !estado_MB_507;
  expansor.digitalWrite(MB_507, estado_MB_507);
}

void Processo_MB_508() {                                //processo que ativa manualmente a bomba do laboratorio
  estado_MB_508 = !estado_MB_508;
  expansor.digitalWrite(MB_508, estado_MB_508);
}

void Processo_AutoMan() {                            //processo que ativa manualmente o MB_505
  AutoMan = !AutoMan;
  String textoAutoMan;
  if(AutoMan==true){
    textoAutoMan="Auto";
    }
  else{
    textoAutoMan="Manual";
    }
  textoAutoMan.toCharArray(AutoManEEPROM,30);
  gravarEEPROM();
  
}

void Sincroniza_Modbus() {
  modb.Coil(0, digitalRead(HS_REMOTO));
  modb.Coil(1, digitalRead(MB_500));
  modb.Coil(2, digitalRead(MB_501));
  modb.Coil(3, digitalRead(MB_504));
  modb.Coil(4, digitalRead(MB_505));
  modb.Coil(5, estado_MB_506);
  modb.Coil(6, estado_MB_507);
  modb.Coil(7, estado_MB_508);
  modb.Coil(8, estado_XV_502);
  modb.Coil(9, estado_XV_503);
  modb.Coil(10, estado_LLL_200);
  modb.Coil(11, estado_LLH_200);
  modb.Coil(12, estado_LLL_205);
  modb.Coil(13, estado_LLH_205);
  modb.Coil(26, estado_PSH_100);
  modb.Coil(27, estado_PSH_101);
  modb.Coil(29, estado_LSL_203);
  modb.Coil(30, estado_LSL_204);
  modb.Coil(31, estado_LSH_206);
  modb.Coil(32, retorno_MB_500);
  modb.Coil(33, retorno_MB_501);
  modb.Coil(34, retorno_MB_504);
  modb.Coil(35, retorno_MB_505);
  modb.Coil(36, retorno_MB_506);
  modb.Coil(37, retorno_MB_507);
  modb.Coil(38, retorno_MB_508);
  modb.Coil(39, retorno_XV_502);
  modb.Coil(40, retorno_XV_503);
  modb.Coil(41, AutoMan);
  modb.Hreg(1, pct_LT_200);
  modb.Hreg(2, pct_LT_205);

}

void Checar_Fluxo() {
  if (nivel_LT_200 < (ultimaMedida_LT_200 + 200)) //Caso não haja aumento no Fluxo o alerta é acrescido. OBS: 200 é uma margem de erro pela variação do ADS
    alertaFluxo++;
  if (alertaFluxo > 2) {
    if (digitalRead(MB_500) == 1) {
      Serial.println("Detectada falta de fluxo");
      protege_MB_500 = true;
      Parar_Captar();
      alertaFluxo = 0;
    }
    if (digitalRead(MB_501) == 1) {
      Serial.println("Detectada falta de fluxo");
      protege_MB_501 = true;
      Parar_Captar();
      alertaFluxo = 0;
    }
  }
  ultimaMedida_LT_200 = nivel_LT_200;
  tempoDeFluxo = millis();
  if (protege_MB_500 == true && protege_MB_501 == true)
    Serial.println("As 2 bombas estão travadas por medidas de proteção(falta de fluxo)");
}

void paginaInicial() {
  const char* page =
#include "pgInicial.h"
    ;

  server.send(200, "text/html", page);
}

void paginaAdministrar() {

  char userC[30], passC[30];
  String uconf, pconf;

  //recupera usuário e senha digitados na página raiz para fazer a autenticação
  uconf = server.arg("login");
  pconf = server.arg("senha");

  //Converte String em char array
  uconf.toCharArray(userC, 30);
  pconf.toCharArray(passC, 30);
  
 
  
  //compara usuário e senha com valor da EEPROM
  if ( strcmp(userC, userLog) == 0 && strcmp(passC, userPass) == 0 ) {
    const char* pagina =  
    #include "pgAdministrar.h"
    ;
    server.send(200, "text/html", pagina);
  }
  else {
    String pagina;
    pagina += "<html><head><title>Estação de Tratamento de Água</title></head>";
    pagina += "<h1><center>Erro no Login</center></h1>";
    
    server.send(200, "text/html", pagina);
  }
  
}

void atualizarConfigs() {
  String pgTroca = "<html><head><title>Estação de Tratamento de Água</title></head>" ;

  pgTroca += "<h1><center>Formulario de Confirmacao das Configuracoes</center></h1>";

  pgTroca += "<p>";
  if (server.hasArg("login")) {
    pgTroca += "Login: ";
    pgTroca += server.arg("login");
    server.arg("login").toCharArray(userLog, 30);
  }
  else {
    pgTroca += "-----";
  }
  pgTroca += "</p>";

  pgTroca += "<p>";
  if (server.hasArg("authpass")) {
    pgTroca += "Senha: ";
    pgTroca += server.arg("authpass");
    server.arg("authpass").toCharArray(userPass, 30);
  }
  else {
    pgTroca += "-----";
  }
  pgTroca += "</p>";

  pgTroca += "<p>";
  if (server.hasArg("ssid")) {
    pgTroca += "SSID: ";
    pgTroca += server.arg("ssid");
    server.arg("ssid").toCharArray(ssid, 30);
  }
  else {
    pgTroca += "-----";
  }
  pgTroca += "</p>";

  pgTroca += "<p>";
  if (server.hasArg("wifipass")) {
    pgTroca += "Senha: ";
    pgTroca += server.arg("wifipass");
    server.arg("wifipass").toCharArray(password, 30);
  }
  else {
    pgTroca += "-----";
  }
  pgTroca += "</p>";

  pgTroca += "<p>";
  if (server.hasArg("serverip")) {
    pgTroca += "IP: ";
    pgTroca += server.arg("serverip");
    server.arg("serverip").toCharArray(serverIP, 30);
  }
  else {
    pgTroca += "-----";
  }
  pgTroca += "</p>";

  pgTroca += "<p>";
  if (server.hasArg("servermask")) {
    pgTroca += "Mascara: ";
    pgTroca += server.arg("servermask");
    server.arg("servermask").toCharArray(servermask, 30);
  }
  else {
    pgTroca += "-----";
  }
  pgTroca += "</p>";

  pgTroca += "<p>";
  if (server.hasArg("servergate")) {
    pgTroca += "Gateway: ";
    pgTroca += server.arg("servergate");
    server.arg("servergate").toCharArray(servergate, 30);
  }
  else {
    pgTroca += "-----";
  }
  pgTroca += "</p>";

  pgTroca += "<p>";
  if (server.hasArg("serverdns")) {
    pgTroca += "DNSr: ";
    pgTroca += server.arg("serverdns");
    server.arg("serverdns").toCharArray(serverDNS, 30);
  }
  else {
    pgTroca += "-----";
  }
  pgTroca += "</p>";

  pgTroca += "<p>";
  if (server.hasArg("bombasconfig")) {
    pgTroca += "Utilização de Bombas: ";
    pgTroca += server.arg("bombasconfig");
    server.arg("bombasconfig").toCharArray(selecao, 30);
  }
  else {
    pgTroca += "-----";
  }
  pgTroca += "</p>";

  pgTroca += "<p>";
  if (server.hasArg("lt200baixo")) {
    pgTroca += "Nível Baixo de Água Bruta(LT-200): ";
    pgTroca += server.arg("lt200baixo");
    server.arg("lt200baixo").toCharArray(lt200Baixo, 30);
  }
  else {
    pgTroca += "-----";
  }
  pgTroca += "</p>";

  pgTroca += "<p>";
  if (server.hasArg("lt200alto")) {
    pgTroca += "Nível Alto de Água Bruta(LT-200): ";
    pgTroca += server.arg("lt200alto");
    server.arg("lt200alto").toCharArray(lt200Alto, 30);
  }
  else {
    pgTroca += "-----";
  }
  pgTroca += "</p>";

  pgTroca += "<p>";
  if (server.hasArg("lt205baixo")) {
    pgTroca += "Nível Baixo de Água Bruta(LT-205):  ";
    pgTroca += server.arg("lt205baixo");
    server.arg("lt205baixo").toCharArray(lt205Baixo, 30);
  }
  else {
    pgTroca += "-----";
  }
  pgTroca += "</p>";

  pgTroca += "<p>";
  if (server.hasArg("lt205alto")) {
    pgTroca += "Nível Alto de Água Bruta(LT-205): ";
    pgTroca += server.arg("lt205alto");
    server.arg("lt205alto").toCharArray(lt205Alto, 30);
  }
  else {
    pgTroca += "-----";
  }
  pgTroca += "</p>";

  pgTroca += "<form method='POST' action='/reset'>";
  pgTroca += "<p><center><input name=button3 type=submit value=Reiniciar /></center></p>";
  pgTroca += "</form>";
  pgTroca += "<p><center><a href=/>Pagina Inicial</a></center></p>";
  pgTroca += "</body></html>";

  // Enviando HTML para o servidor
  server.send(200, "text/html", pgTroca);
  gravarEEPROM();
  atualizarNiveis();

}

void paginaOpera() {
  const char* paginaop=
  #include "pgOperar.h"
  ;
  server.send(200, "text/html", paginaop);
}

void lerEEPROM() {
  unsigned int endereco;
  endereco = 0;
  EEPROM.get(endereco, ssid); //endereço=0
  endereco += 30;
  EEPROM.get(endereco, password); //endereço=30
  endereco += 30;
  EEPROM.get(endereco, serverIP); //endereço=60
  endereco += 30;
  EEPROM.get(endereco, servermask); //endereço=90
  endereco += 30;
  EEPROM.get(endereco, servergate); //endereço=120
  endereco += 30;
  EEPROM.get(endereco, serverDNS); //endereço=150
  endereco += 30;
  EEPROM.get(endereco, selecao); //endereço=180
  endereco += 30;
  EEPROM.get(endereco, lt200Baixo); //endereço=210
  endereco += 30;
  EEPROM.get(endereco, lt200Alto); //endereço=240
  endereco += 30;
  EEPROM.get(endereco, lt205Baixo); //endereço=270
  endereco += 30;
  EEPROM.get(endereco, lt205Alto); //endereço=300
  endereco += 30;
  EEPROM.get(endereco, userLog); //endereço=330
  endereco += 30;
  EEPROM.get(endereco, userPass); //endereço=360
  endereco += 30;
  EEPROM.get(endereco, AutoManEEPROM); //endereço=360
  endereco += 30;
  endereco = 0;
  char dado[30];
  for ( byte i = 0; i < 14; i++)
  {
    EEPROM.get(endereco, dado);
    Serial.println(dado);
    endereco += 30;
  }
}

void gravarEEPROM() {
  unsigned int endereco;
  endereco = 0;

  EEPROM.put(endereco, ssid);
  endereco += 30;
  EEPROM.put(endereco, password);
  endereco += 30;
  EEPROM.put(endereco, serverIP);
  endereco += 30;
  EEPROM.put(endereco, servermask);
  endereco += 30;
  EEPROM.put(endereco, servergate);
  endereco += 30;
  EEPROM.put(endereco, serverDNS);
  endereco += 30;
  EEPROM.put(endereco, selecao);
  endereco += 30;
  EEPROM.put(endereco, lt200Baixo);
  endereco += 30;
  EEPROM.put(endereco, lt200Alto);
  endereco += 30;
  EEPROM.put(endereco, lt205Baixo);
  endereco += 30;
  EEPROM.put(endereco, lt205Alto);
  endereco += 30;
  EEPROM.put(endereco, userLog);
  endereco += 30;
  EEPROM.put(endereco, userPass);
  endereco += 30;
  EEPROM.put(endereco, AutoManEEPROM);
  endereco += 30;
  EEPROM.commit();
  endereco = 0;
  char dado[30];
  for ( byte i = 0; i < 14; i++)
  {
    EEPROM.get(endereco, dado);
    Serial.println(dado);
    endereco += 30;
  }
}
