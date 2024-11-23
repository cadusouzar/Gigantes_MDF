 # Gigantes de MDF

### Alunos ###

Carlos Eduardo de Souza Ribeiro - 174040

Alvaro Gochi de Sousa - 172158

Neemias Vieira Fernandes - 170550

João Victor Teixeira - 147653

# Sobre o Projeto:

O projeto Gigantes de MDF, vem com o intuito dos alunos aplicarem os conceitos recebidos pela disciplina de Programação de Hardware. O intuito é cada grupo criar seu código de um robô que dispara laser e também detecta laser recebido por meio de sensores. Os componentes utilizados no projeto foram:

- 1x Arduino Nano
- 1x Resistores de 10kohms
- 3x Resisrores de 360ohms
- 5x Botões
- 2x Motores DC
- 1x C.I de ponte H L293D
- 1x Diodo Laser
- 1x LDR de 20mm

O código foi desenvolvido na linguagem C utilizando apenas o ATMEGA328, foi utilizado o programa SimulIde para debugar o codigo, também o próprio programa Arduino para gravar no arduino e testar.

# Pinagem 

### Motores ###
- PD2 -> MOTOR ESQUERDO "FRENTE"
- PD3 -> MOTOR ESQUERDO "TRAS"
- PD4 -> MOTOR DIREITO "FRENTE"
- PD5 -> MOTOR PWM ESQUERDO
- PD6 -> MOTOR PWM DIREITO
- PD7 -> MOTOR DIREITO "TRAS"

### Leds ###

- PC5 -> LED 
- PC3 -> LED
- PC2 -> LED
- PC1 -> DIODO LASER (ELE PRECISA FICAR EM LOOP, SOLTANDO LASER A CADA 1 SEGUNDO AUTOMATICAMENTE)

### Sensores ###

- PCO -> LDR

### Botões ###

- PBO -> BOTAO ANDAR PRA FRENTE (GIRA OS DOIS MOTORES PRA FRENTE)
- PB1 -> BOTAO (APENAS RODA DA DIREITA GIRA PRA FRENTE)
- PB2 -> BOTAO (APENAS RODA DA ESQUERDA GIRA PRA FRENTE)
- PB3 -> BOTAO ALTERNA PARA RÉ, FAZENDO COM QUE OS BOTOES ANTERIORES GIREM AS RODAS PARA TRAS NA DETERMINADA LOGICA: AO CLICAR ESSE BOTAO PB0 GIRA AS DUAS PARA TRAS, PB1 GIRA APENAS A RODA DA DIREITA PARA TRAS E PB2 GIRA - APENAS A RODA DA ESQUERDA PARA TRAS
- PB4 -> BOTAO PARA LIGAR OS LEDS (REINICIA AS VIDAS)
