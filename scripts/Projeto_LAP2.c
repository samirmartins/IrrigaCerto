//------------------------------------------------------------------------------//
// Essa Rotina ? respons?vel pelo controle do sistema de irriga??o inteligente  //
// "Irrigacerto"                                                                //
// Data In?cio:13/05/2010              Data T?rmino: 19/06/2010                 //
// Programadores:   Jo?o Paulo Vieira                                           //
//                  Samir Angelo Milani Martins                                 //
//------------------------------------------------------------------------------//
#include <18F4550.h> // Inclui arquivo de defini??es do microcontrolador
#device  ADC=10;      // Configurando conversor AD para 10 bits
#use     delay(clock=8000000) //Definindo clock do microcontrolador
#FUSES   INTRC_IO,NOWDT,NOPUT,NOPROTECT,NOMCLR,NODEBUG,NOBROWNOUT,NOLVP,NOCPD,NOWRT
#include "D:\Desktop\LAPII\LAP2\Projeto18F4550\Rotinas_JPSAMIR\driver_lcd.c" // Incluindo arquivo com driver do LCD


//------------------------------------------------------------------------------
// Associando TAGs aos pinos de entrada e sa?da
//------------------------------------------------------------------------------

#define tcl_enter       (!input(PIN_D5)) 
#define tcl_baixo       (!input(PIN_D6))
#define tcl_cima        (!input(PIN_D4))
#define liga_bomb       output_high(PIN_D0)
#define des_bomb        output_low(PIN_D0)
#define abre_valv       output_high(PIN_D1)
#define fecha_valv      output_low(PIN_D1)     
#define LED                PIN_D2
#define state_cima            1
#define state_baixo           2
#define state_enter           3
#define debounce              0
#define ligado                1
#define desligado             0
#define state_bomb         input_state(PIN_D0)
#define state_valv         input_state(PIN_D1)

//------------------------------------------------------------------------------
// Prot?tipos das fun??es que ser?o utilizadas
//------------------------------------------------------------------------------

void  update_display(void);
void  check_buttons(void);
int16 read_AD(int ch);
void  update_sp(void);

//------------------------------------------------------------------------------
// Definindo vari?veis globais
//------------------------------------------------------------------------------
int1 tcl_cima_press=0,tcl_baixo_press=0,tcl_enter_press=0;  // Definindo vari?vel de 1 bit para estados das teclas de entrada
int8 filtro1=debounce;                                      // Definindo valores para filtros debounce das teclas
int8 filtro3=debounce;
int8 filtro2=debounce;
int8 state=1;                                               //Definindo valores iniciais para a m?quina de estados
int16 set_point_on=2;                                       //Definindo valor inicial do set_point_on  2
int16 set_point_off=4;                                      //Definindo valor inicial do set_point_off 4
int16 aux,aux1;                                             //Vari?veis auxiliares
float umi,tempamb;                                          //Vari?vel umidade e temperatura ambiente
int8 aux3=8;
int8 aux2=1;

// Fun??o para limpar a tela do lcd

void clr_scr(void) {

lcd_gotoxy(1,1);
printf(lcd_putc, "                ");
lcd_gotoxy(1,2);
printf(lcd_putc, "                ");
delay_us(30);
}

//------------------------------------------------------------------------------
// Fun??o para leitura das teclas e filtro debounce
//------------------------------------------------------------------------------

void check_buttons(void)
{
//------------------------------------------------------------------------------
// ENTRADA1-Tcl_acima
//------------------------------------------------------------------------------
        if(tcl_cima)     // Relativo ao pino Pino A2 do PIC                              
          {
            if(!(tcl_cima_press))   // Flag indicando se bot?o foi pressionado
            {
               if(filtro1!=0)      // valor do debounce
               {
                  filtro1--;
               }
               else
               {
               tcl_cima_press=1;  //  flag que indica se botao foi pressionado ou nao depois do filtro
                                 
               }
            }
          }
        else
            {  
              filtro1=debounce;
              tcl_cima_press=0;               
            }        
//------------------------------------------------------------------------------
// ENTRADA2-tcl_baixo
//------------------------------------------------------------------------------
        if(tcl_baixo)     // Relativo ao pino Pino A3 do PIC                              
           {
            if(!(tcl_baixo_press))   // Flag indicando se bot?o foi pressionado
            {
               if(filtro2!=0)      // valor do debounce
               {
                  filtro2--;
               }
               else
               {
               tcl_baixo_press=1;  //  flag que indica se botao foi pressionado ou nao depois do filtro
                                    
               }
         
            }
            
           }
        else
            {  
              filtro2=debounce;
              tcl_baixo_press=0; 
            }
//------------------------------------------------------------------------------
// ENTRADA3-tcl_enter
//------------------------------------------------------------------------------
        if (tcl_enter)     // Relativo ao pino Pino A4 do PIC                              
          {
            if(!(tcl_enter_press))   // Flag indicando se bot?o foi pressionado
            {
               if(filtro3!=0)      // valor do debounce
                {
                  filtro3--;
                }
               else
               {
                 tcl_enter_press=1;  //  flag que indica se botao foi pressionado ou nao depois do filtro                                
               }
            }
          }
        else
            {  
              filtro3=debounce;
              tcl_enter_press=0;            
            }
}

//------------------------------------------------------------------------------
// Fun??o que l? entrada anal?gica ch do PIC
//------------------------------------------------------------------------------
int16 read_AD(int ch)
{
  int16 temp;          //Definindo vari?vel auxiliar para leitura do A/D
  set_adc_channel(ch); //Selecionando o canal A/D que ir? ser feita convers?o
  delay_us(30);        //Tempo para carga capacitor de Hold 
  temp=read_adc();     //Efeturando convers?o 
  return temp;         //Retornando valor da convers?o 
}


//------------------------------------------------------------------------------
// Fun??o que atualiza os valores dos pontos de atua??o da bomba
//------------------------------------------------------------------------------

void update_sp(void)
{
  if (state==6)
   {
    check_buttons();
    if(tcl_baixo_press)
       set_point_on--;
     else if(tcl_cima_press)
       set_point_on++;
   }
  if(state==7)
   {
    check_buttons();
    if(tcl_baixo_press)
       set_point_off--;
     else if(tcl_cima_press)
       set_point_off++;
   }
}
//------------------------------------------------------------------------------
// M?quina de estados para cria??o dos menus no LCD
//------------------------------------------------------------------------------
void update_display(void)
{
   switch(state)
 {
        
        case 1:           // Estado 1 : Estado inicial de opera??o inicial
            clr_scr();
            lcd_gotoxy(1,1);
            printf(lcd_putc, "Sistema Controle");
            lcd_gotoxy(3,2);  // Deslocandos cursor para linha 2 coluna 3
            printf(lcd_putc,"IrriGa Certo");
          
            check_buttons();  // Lendo teclas de entrada
            if(tcl_enter_press) 
              state =2;      // Aponta para o pr?ximo estado
            
        
            
            break;           // Sai do switch
            
        case 2:           //Estado 2: Estado de indica??o de temperatura no LCD 
            clr_scr();
            lcd_gotoxy(1,1);
            printf(lcd_putc, " Temp. Ambiente");
            lcd_gotoxy(6,2); // Deslocando cursor para linha 2 coluna 6
            printf(lcd_putc,"%2.2f C",tempamb); // Colocando valor de temperatura ambiente 
            check_buttons(); // Lendo teclas de entrada
            if(tcl_baixo_press) //Verifica??o da transi??o para pr?ximo estado
               state=3;    // Apontando para o estado 3 (Umidade) 
            
            break;
            
        case 3: // Estado 3: Estado de indica??o de umidade no LCD
            clr_scr();
            lcd_gotoxy(1,1);
            printf(lcd_putc, " Umidade do Solo");
            lcd_gotoxy(6,2); // Deslocando cursor para linha 2, coluna 6
            printf(lcd_putc,"%2.2f C",umi); // Colocando valor de umidade no solo;
                       
            check_buttons(); // Lendo teclas de entrada
            if(tcl_cima_press) //Verifica??o da transi??o para pr?ximo estado
                state=2;       // Apontando para o estado 2 (Temperatura)
              else if(tcl_baixo_press)
                state=4;         
           
           break;

        case 4:
            clr_scr();
            lcd_gotoxy(1,1);
            printf(lcd_putc, "Bomba~ ");
            if (state_bomb==ligado)
              {
               lcd_gotoxy(10,1); // Deslocando cursor para primeira linha, sexta coluna
               printf(lcd_putc," ON",); // Colocando estado da bomba na tela;
              }   
             else if (state_bomb==desligado)
              {
                lcd_gotoxy(10,1); // Deslocando cursor para primeira linha, sexta coluna
                printf(lcd_putc,"OFF",); // Colocando valor de umidade no solo;
              }
            lcd_gotoxy(1,2); // Deslocando cursor para segunda linha, primeira coluna
            printf(lcd_putc, " Valvula~");
            if (state_valv==ligado)
              {
                lcd_gotoxy(11,2); // Deslocando cursor para primeira linha, sexta coluna
                printf(lcd_putc,"ON",); // Colocando estado da bomba na tela;
              }   
             else if (state_valv==desligado)
              {
                lcd_gotoxy(10,2); // Deslocando cursor para linha 1 coluna 6
                printf(lcd_putc,"OFF",); // Colocando valor de umidade no solo no LCD
              }
          
            check_buttons(); // Lendo teclas de entrada
           if(tcl_cima_press)  // Transi??o para o estado Umidade do solo
                state = 3;
            else if(tcl_baixo_press) // Transi??o para o estado Ajuste Setpoint
                state = 5;
                      
           break;
               
           
        case 5:
            clr_scr();
            lcd_gotoxy(1,1);
            printf(lcd_putc,"Ajuste Setpoint",);
            lcd_gotoxy(3,2); 
            printf(lcd_putc," Press Enter",);
                              
            check_buttons(); // Lendo teclas de entrada
           
            if(tcl_enter_press) // Transi??o entre estados 
               state = 6;
             else if (tcl_cima_press)  
               state = 4;
            
            break;
   
        case 6:
            clr_scr();
            lcd_gotoxy(1,1);
            printf(lcd_putc," Ajuste Setpoint",); // Colocando valor de set point da valvula;
            lcd_gotoxy(4,2); // Deslocando cursor para primeira linha, sexta coluna
            printf(lcd_putc,"ON= %ld",set_point_on); // Colocando valor de set point da valvula; 
                       
            update_sp(); // Atualizando o valor dos setpoints
           
            check_buttons();
           
            if (tcl_enter_press)
             {
               state=7;
             }
            
            break;
        case 7:
            clr_scr();
            lcd_gotoxy(1,1);
            printf(lcd_putc,"Ajuste Setpoint",); // Colocando valor de set point da valvula;
            lcd_gotoxy(4,2); // Deslocando cursor para Linha 2, coluna 4
            printf(lcd_putc,"OFF= %ld",set_point_off); // Colocando valor de setpoint_off da valvula;
           
            update_sp();// Atualizando o valor dos setpoints 
 
            check_buttons();
           
            if (tcl_enter_press)
              state=5;
 
            break;
         
        default:
            break;
 }
}

//------------------------------------------------------------------------------
// Rotina Princiapal
//------------------------------------------------------------------------------
void main(void)
{    

//------------------------------------------------------------------------------
// Inicializa??es
//------------------------------------------------------------------------------

   setup_oscillator(OSC_8MHZ|OSC_INTRC); 
   setup_adc_ports(AN0_TO_AN1);  //Inicializando conversores A/D do PIC para portas A0,A1
   setup_adc(ADC_CLOCK_INTERNAL); // Configurando frequ?ncia de amostragem
   lcd_init();                    // Inicializando LCD  

//------------------------------------------------------------------------------
// Loop princiapal do programa
//------------------------------------------------------------------------------


for(;;)
 {
//------------------------------------------------------------------------------
// Monitoramento de execu??o do programa 1
if (aux3!=aux2)
clr_scr();

aux2 = state;

//------------------------------------------------------------------------------
    output_low(LED);
    delay_ms(300);
//------------------------------------------------------------------------------
// Leitura dos Canais A/D do PIC
//------------------------------------------------------------------------------
    aux=read_AD(0);          // Leitura do canal AN0 referente ao sensor de temperatura
    tempamb=aux*(5.0/1023);  // Polin?mio de calibra??o do sensor de temperatura
    aux1=read_AD(1);         // Leitura do canal AN1 referente ao sensor de umidade
    umi=aux1*(5.0/1023);     // Polin?mio de calibra??o do sensor de temperatura
 
//------------------------------------------------------------------------------
// Atualiza??o do display
//------------------------------------------------------------------------------ 
    update_display(); // M?quina de Estados para gera??o de menus 
    delay_ms(220);    // Delay para transi??o entre estados do display
 
//------------------------------------------------------------------------------
// L?gica de Controle do Sistema de irriga??o
//------------------------------------------------------------------------------ 

    if(umi<=set_point_on)
     {
      abre_valv;
      liga_bomb; 
     }
     else if (umi>=set_point_off)
      {
       fecha_valv;
       des_bomb;
      }
 
//------------------------------------------------------------------------------
// Monitoramento de execu??o do programa 2
//------------------------------------------------------------------------------ 
    output_high(LED);
    delay_ms(300);

aux3 = state;
    
 }   
      
}

