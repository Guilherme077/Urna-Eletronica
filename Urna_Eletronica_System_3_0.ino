/*

THIS CODE IS PROPERTY OF GUILHERME PILZ MOREIRA

Versão 3.0

Fabricado por: Guilherme P. Moreira
Com base no sistema: SOG Arduino
Para: Tribunal Municipal Eleitoral de Nova Acemão (TME)

+------------------------===------------------------+

DESCRIÇÃO DAS FUNÇÕES:
  setup          Primeiras execuções do sistema
  loop           Em Loop durante a execução do sistema
  ZeraMemoria    Zera os votos armazenados na EEPROM
  initscreen     Tela inicial do sistema e divulgação dos votos armazenados
  startup        Tela de Boas vindas do eleitor
  configuracao   Tela de escolha de voto 1
  configuracaoII Tela de escolha de voto 2
  falsewhiles    Retira a execução de aguardo de clique
  telafim        Exibe a mensagem fim ao eleitor

FUNÇÕES COM PARAMETROS:
  *Cores são 3 valores inteiros
  DrawaWindow(texto, cor de fundo);
  DrawaButton(texto, colunainicio, linhainicio, colunafim, linhafim, cor de fundo, cor de borda, cor do texto);
  DrawaMessage(texto pequeno, texto grande)
  Confirma(numero voto) 99 para branco
  


CODIGO DE CORES:
AZUL CLARO rgb(12,192,223)
ROXO       rgb(140,82,255)
AMARELO    rgb(255,222,89)
PRETO      rgb(0,0,0)
BRANCO     rgb(255,255,255)
*/
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////CONFIG DA TELA
#include <UTFT.h>
#include <UTouch.h>
#include <EEPROM.h>

// Variaveis auxiliares de controle de voto
byte voto = 0;

// Variáveis de controle da tela
int suby;
int adiy;
int dispx, dispy, text_y_center;
extern uint8_t SmallFont[];
extern uint8_t BigFont[];

UTFT myGLCD(17, 38, 39, 40, 41);  //3.5" TFTLCD for arduino 2560 from mcufriend.com
UTouch myTouch(6, 5, 4, 3, 2);


bool desenhowhile = true;
bool configwhile = false;
int screen = 1;
int temp1 = 10000;

// Configuração da EEPROM
#define espacoEEPROM 1000

//                               SETUP                                     //
void setup() {
  // CONFIGURAÇÃO INICIAL DA TELA
  myGLCD.InitLCD();
  myGLCD.clrScr();

  myTouch.InitTouch();
  myTouch.setPrecision(PREC_MEDIUM);
  dispx = myGLCD.getDisplayXSize();
  dispy = myGLCD.getDisplayYSize();
  text_y_center = (dispy / 2) - 6;
  myGLCD.setFont(BigFont);

  //Configuração SERIAL
  Serial.begin(9600);
  Serial2.begin(9600);

  //CHAMADA DE ROTINAS INICIAIS

  //ZeraMemoria();  //DESCOMENTE PARA ZERAR VOTOS

  Serial.println(" ");
  Serial.println("-=-=-=-=-=-=");
  Serial.println(" ");
  initscreen();
  startup();
}

void loop() {
  long x, y;

  myTouch.read();
  x = myTouch.getX();
  y = myTouch.getY();

  //Conta para inverter (Criada por: GUILHERME PILZ MOREIRA)
  suby = y - 115;   // para saber quanto falta para 115 (metade da tela aprox.)
  adiy = suby * 2;  // multiplica para poder chegar a metade e depois até o equivalente ao Y mas ao contrário ( como se o 115 fosse um espelho)
  y = y - adiy;     // transforma o Y no valor correto

  if ((y >= 175) and (y <= 225))  // linha de botões de baixo
  {
    if ((x >= 105) and (x <= 215))  // botão: estacionar
    {
      if (screen == 1) {
        delay(1000);
        configwhile = true;
        configuracao();
      }
    }
  }
}

void ZeraMemoria() {
  EEPROM.write(5, 0);
  EEPROM.write(4, 0);
  EEPROM.write(3, 0);
  EEPROM.write(2, 0);
  EEPROM.write(1, 0);
  EEPROM.write(0, 0);
}

void initscreen() {
  //APRESENTAÇÃO VIA SERIAL
  Serial.println("-=-=-=-=-=-=-=INICIANDO URNA V.3.0=-=-=-=-=-=-=-=-");
  Serial.println("-=-=-=-Tribunal Municipal Eleitoral (TME)-=-=-=-=-");
  Serial.println("-=-=-=-=-=-=-=Sistema SOG Arduino-=-=-=-=-=-=-=-=-");
  Serial.println(" ");
  Serial.println("INICIANDO URNA PARA VOTACAO");
  Serial.println(" ");
  Serial.println("Resultados registrados:");
  Serial.println("Prefeito(a):");
  //Serial.println("Voto 0:");
  //Serial.println(EEPROM.read(0));
  Serial.println("Voto 3:");
  Serial.println(EEPROM.read(1));
  Serial.println("Voto 7:");
  Serial.println(EEPROM.read(2));
  Serial.println("Branco:");
  Serial.println(EEPROM.read(3));
  /*
  Serial.println("Amigo Secreto:");
  Serial.println("SIM:");
  Serial.println(EEPROM.read(4));
  Serial.println("NAO:");
  Serial.println(EEPROM.read(5));
  */
  //APRESENTAÇÃO VIA TELA
  myGLCD.setColor(130, 130, 130);         // cinza // fundo do grande bloco
  myGLCD.fillRect(0, 0, dispx - 1, 230);  // desenha o bloco abaixo do de cima // COMO USAR ESTA LINHA: o segundo número é a linha de inicio do desenho, e o último é o último (óbvio).
  //escreve os textos estáticos
  myGLCD.setBackColor(130, 130, 130);  //AZUL // cor do fundo das letras (fora delas)
  myGLCD.setColor(255, 255, 255);      // preto // letras
  myGLCD.setFont(BigFont);

  myGLCD.print("INICIALIZANDO O SOG", CENTER, 105);  // Info. Principal
  myGLCD.setFont(SmallFont);
  myGLCD.print("ATMEGA 2560 / ARDUINO / TME", CENTER, 1);              // Info. do sistema
  myGLCD.print("(C) 2023 GUANDER SISTEMAS, Guilherme P.", LEFT, 215);  // Informação de Copyright

  myGLCD.setColor(130, 130, 130);           // cinza // fundo do grande bloco
  myGLCD.fillRect(0, 155, dispx - 1, 190);  // desenha o preenchimento
  delay(200);
  myGLCD.setColor(255, 255, 255);  // preto // letras
  myGLCD.print("Kernel iniciado! A executar 'startup.exe'", CENTER, 160);
  myGLCD.setFont(BigFont);
  delay(3000);
}

void startup() {
  screen = 1;
  DrawaWindow("Inicio", 140, 82, 255);
  //escreve os textos estáticos
  myGLCD.setFont(BigFont);
  myGLCD.setBackColor(140, 82, 255);  // ROXO // fundo das letras
  myGLCD.setColor(255, 255, 255);     // BRANCO // letras
  myGLCD.print("BEM VINDO (A)!", LEFT, 30);
  myGLCD.print("Voce vai votar para", LEFT, 80);
  myGLCD.print("Prefeito (Fev/Mar).", LEFT, 100);
  myGLCD.print("Ajude a Democracia!", LEFT, 150);

  //Desenha o botão INICIO
  DrawaButton("VOTAR", 105, 225, 215, 175, 255, 222, 89, 12, 192, 223, 0, 0, 0);
}

void configuracao() {
  Serial.println("STATUS: NA JANELA DE ESCOLHA DE VOTO");
  screen = 5;

  //DESENHA A JANELA
  DrawaWindow("Escolha de voto", 140, 82, 255);

  //escreve os textos estáticos
  myGLCD.setFont(BigFont);
  myGLCD.setBackColor(140, 82, 255);  // ROXO // fundo das letras
  myGLCD.setColor(255, 255, 255);     // BRANCO // letras
  myGLCD.print("Voto: PREFEITO(A)", LEFT, 30);

  //Desenha os botões
  DrawaButton("Voto 3", 10, 105, 115, 55, 255, 255, 255, 255, 222, 89, 0, 0, 0);
  DrawaButton("Voto 7", 125, 105, 230, 55, 255, 255, 255, 255, 222, 89, 0, 0, 0);
  DrawaButton("Branco", 10, 175, 115, 125, 255, 255, 255, 255, 222, 89, 0, 0, 0);

  while (configwhile == true) {
    long x, y;
    while (myTouch.dataAvailable() == true) {
      myTouch.read();
      x = myTouch.getX();
      y = myTouch.getY();
      //Conta para inverter (Criada por: GUILHERME PILZ MOREIRA)
      suby = y - 115;   // para saber quanto falta para 115 (metade da tela aprox.)
      adiy = suby * 2;  // multiplica para poder chegar a metade e depois até o equivalente ao Y mas ao contrário ( como se o 115 fosse um espelho)
      y = y - adiy;     // transforma o Y no valor correto
      // FIM DA CONTA E DA GAMBIARRA
      int temp1 = 5000;
      if ((y >= 55) and (y <= 105))  // LINHA DE BOTÕES 1
      {
        if ((x >= 10) and (x <= 115))  // botão: voto 1
        {
          Confirma(3);
        }
        if ((x >= 125) and (x <= 230))  // botão: voto 2
        {
          Confirma(7);
        }
      }
      if ((y >= 125) and (y <= 175))  // LINHA DE BOTÕES 2
      {

        if ((x >= 10) and (x <= 115))  // botão: Branco
        {
          Confirma(99);
        }
      }
    }
  }
}

void configuracaoII() {
  Serial.println("STATUS: NA JANELA DE ESCOLHA DE VOTO");
  screen = 5;

  //DESENHA A JANELA
  DrawaWindow("Escolha de voto", 140, 82, 255);

  //escreve os textos estáticos
  myGLCD.setFont(BigFont);
  myGLCD.setBackColor(140, 82, 255);  // ROXO // fundo das letras
  myGLCD.setColor(255, 255, 255);     // BRANCO // letras
  myGLCD.print("Voto: PREFEITO(A)", LEFT, 30);

  //Desenha os botões
  DrawaButton("Voto 3", 10, 105, 115, 55, 255, 255, 255, 255, 222, 89, 0, 0, 0);
  DrawaButton("Voto 7", 125, 105, 230, 55, 255, 255, 255, 255, 222, 89, 0, 0, 0);
  DrawaButton("Branco", 10, 175, 115, 125, 255, 255, 255, 255, 222, 89, 0, 0, 0);

  while (configwhile == true) {
    long x, y;
    while (myTouch.dataAvailable() == true) {
      myTouch.read();
      x = myTouch.getX();
      y = myTouch.getY();
      //Conta para inverter (Criada por: GUILHERME PILZ MOREIRA)
      suby = y - 115;   // para saber quanto falta para 115 (metade da tela aprox.)
      adiy = suby * 2;  // multiplica para poder chegar a metade e depois até o equivalente ao Y mas ao contrário ( como se o 115 fosse um espelho)
      y = y - adiy;     // transforma o Y no valor correto
      // FIM DA CONTA E DA GAMBIARRA
      int temp1 = 5000;
      if ((y >= 55) and (y <= 105))  // LINHA DE BOTÕES 1
      {
        if ((x >= 10) and (x <= 115))  // botão: voto 1
        {
          Confirma(3);
        }
        if ((x >= 125) and (x <= 230))  // botão: voto 2
        {
          Confirma(7);
        }
      }
      if ((y >= 125) and (y <= 175))  // LINHA DE BOTÕES 2
      {

        if ((x >= 10) and (x <= 115))  // botão: Branco
        {
          Confirma(99);
        }
      }
    }
  }
}

void falsewhiles() {
  configwhile = false;
  desenhowhile = false;
}

void telafim() {
  falsewhiles();
  DrawaWindow("Voto Registrado!", 255, 255, 255);
  myGLCD.setColor(0, 0, 0);
  myGLCD.setBackColor(255, 255, 255);  // BRANCO // fundo das letras
  myGLCD.setFont(BigFont);
  myGLCD.print("FIM", CENTER, 120);
  myGLCD.print("Pode deixar a Cabine", CENTER, 200);
  Serial.println("STATUS: EXIBINDO MENSAGEM FIM");
  Serial.println("STATUS: GRAVANDO E EMBARALHANDO DADOS");
  delay(15000);
  Serial.println("STATUS: NA TELA INICIAL");
  Serial.println("STATUS: PREPARADO PARA PROXIMO ELEITOR");
  Serial.println(" ");
  Serial.println("-=-=-=-=-=-=");
  Serial.println(" ");
  startup();
}

void DrawaWindow(String text, int cf1, int cf2, int cf3) {
  //BARRA SUPERIOR
  myGLCD.setColor(12, 192, 223);         // AZUL CLARO //cor do preenchimento de cima
  myGLCD.fillRect(0, 0, dispx - 1, 20);  // desenha o preenchimento
  myGLCD.setColor(255, 222, 89);         // AMARELO // linha divisória
  myGLCD.fillRect(267, 0, 270, 20);      //desenha a linha
  myGLCD.setColor(0, 0, 0);              // PRETO // letras
  myGLCD.setBackColor(12, 192, 223);     // AZUL CLARO // fundo das letras
  myGLCD.setFont(BigFont);
  myGLCD.print(text, LEFT, 1);
  myGLCD.setColor(255, 222, 89);  // AMARELO // cor das letras
  myGLCD.print("TME", RIGHT, 1);
  //FUNDO
  myGLCD.setColor(cf1, cf2, cf3);          // fundo da janela
  myGLCD.fillRect(0, 22, dispx - 1, 230);  // desenha o fundo // COMO USAR ESTA LINHA: o segundo número é a linha de inicio do desenho, e o último é o último (óbvio).
}

void DrawaButton(String text, int col1, int lin1, int col2, int lin2, int cf1, int cf2, int cf3, int cb1, int cb2, int cb3, int ct1, int ct2, int ct3) {
  myGLCD.setColor(cf1, cf2, cf3);  //COR DE FUNDO
  myGLCD.fillRoundRect(col1, lin1, col2, lin2);
  myGLCD.setColor(cb1, cb2, cb3);  //COR DA BORDA
  myGLCD.drawRoundRect(col1, lin1, col2, lin2);
  myGLCD.setColor(ct1, ct2, ct3);  //COR DO TEXTO
  myGLCD.setFont(BigFont);
  myGLCD.setBackColor(cf1, cf2, cf3);  //Cor de fundo das letras
  myGLCD.print(text, (col1 + 5), (lin1 - 30));
}

void DrawaMessage(String text1, String text2) {
  myGLCD.setColor(255, 222, 89);  //AMARELO //fundo mensagem
  myGLCD.fillRoundRect(30, 155, 290, 45);
  myGLCD.setColor(255, 255, 255);  //BRANCO //contorno mensagem
  myGLCD.drawRoundRect(30, 155, 290, 45);
  myGLCD.setColor(0, 0, 0);  //PRETO //letras
  myGLCD.setFont(SmallFont);
  myGLCD.setBackColor(255, 222, 89);  //AMARELO // fundo texto
  myGLCD.print(text1, 35, 55);
  myGLCD.setFont(BigFont);
  myGLCD.print(text2, 35, 95);
}

void Confirma(int votoConfirma) {
  //CONFIG DA CONFIRMAÇÃO
  int posEEPROM;
  String nome;
  int idade;
  switch (votoConfirma) {
    case 0:
      posEEPROM = 0;
      nome = "Eduarda P M";
      idade = 13;
      break;
    case 3:
      posEEPROM = 1;
      nome = "Eduardo S M";
      idade = 42;
      break;
    case 7:
      posEEPROM = 2;
      nome = "Guilherme P M";
      idade = 16;
      break;
    case 99:
      posEEPROM = 3;
      nome = "BRANCO";
      idade = 0;
      break;
    default:
      DrawaMessage("ERRO INTERNO", "CONTATE O TME");
      break;
  }

  //JANELA CONFIRMAÇÃO
  DrawaWindow("Confirmacao", 140, 82, 255);
  myGLCD.setFont(BigFont);
  myGLCD.setBackColor(140, 82, 255);  // ROXO // fundo das letras
  myGLCD.setColor(255, 255, 255);     // BRANCO // letras
  myGLCD.print("Confirma seu voto?", LEFT, 30);
  myGLCD.print("Nome: " + nome, LEFT, 70);
  myGLCD.print("Numero: " + String(votoConfirma), LEFT, 90);
  myGLCD.print("Idade: " + String(idade), LEFT, 110);

  //Desenha os botões
  DrawaButton("SIM", 10, 225, 115, 175, 255, 255, 255, 255, 222, 89, 0, 0, 0);
  DrawaButton("NAO", 125, 225, 230, 175, 255, 255, 255, 255, 222, 89, 0, 0, 0);



  while (configwhile == true) {

    long x, y;
    while (myTouch.dataAvailable() == true) {
      myTouch.read();
      x = myTouch.getX();
      y = myTouch.getY();
      //Conta para inverter (Criada por: GUILHERME PILZ MOREIRA)

      suby = y - 115;   // para saber quanto falta para 115 (metade da tela aprox.)
      adiy = suby * 2;  // multiplica para poder chegar a metade e depois até o equivalente ao Y mas ao contrário ( como se o 115 fosse um espelho)
      y = y - adiy;     // transforma o Y no valor correto

      // FIM DA CONTA E DA GAMBIARRA
      int temp1 = 5000;

      if ((y >= 175) and (y <= 225))  // linha de botões de baixo
      {

        if ((x >= 10) and (x <= 115))  // botão: sim
        {
          DrawaMessage("Voto Confirmado!", nome);
          voto = EEPROM.read(posEEPROM);
          voto++;
          EEPROM.write(posEEPROM, voto);
          Serial.println("STATUS: EXIBINDO VOTO");
          delay(temp1);
          telafim();
        }

        if ((x >= 125) and (x <= 230))  // botão: nao
        {
          configuracao();
        }
      }
    }
  }
}
