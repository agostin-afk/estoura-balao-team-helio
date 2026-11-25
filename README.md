# estoura-balao-team-helio
**Instituição:** Grupo de Robótica Competitiva (GRC)  
**Equipe:** Equipe Hélio. 

# 1. Introdução
Este documento tem como finalidade apresentar a documentação técnica do projeto de desenvolvimento de um robô móvel para a modalidade Estoura-Balão, realizado no âmbito do Grupo de Robótica Competitiva (GRC).

O projeto envolve o planejamento, a modelagem, o desenvolvimento dos sistemas de controle e elétrico, a integração dos componentes e a montagem de um robô capaz de participar de uma competição cujo objetivo consiste em estourar o balão do adversário enquanto preserva o próprio.

# 3. Objetivos
### 3.1. mecânica
- Desenvolver uma estrutura mecânica que comporte os componentes necessários ao funcionamento do robô.
- Implementar um sistema de acionamento dos motores responsável pela movimentação do robô.
### 3.2. controle
-  Desenvolver um sistema de controle capaz de receber e interpretar comandos remotos.
### 3.3. elétrico
- Projetar e implementar o sistema elétrico responsável pela alimentação e pelo acionamento dos componentes.
### 3.4. por fim
- Por fim desejamos integrar tudo isso em algo funcional (escopo).
# 4. escopo
- implementação da modelagem tridimensional.
- implementação do sistema elétrico e dos circuitos de acionamento.
- Implementação do sistema de controle e comunicação remota.
- Integração dos diferentes subsistemas do robô.
- montagem física do protótipo.
- realização de diversos testes e verificações de desempenho
# 5. requisitos funcionais
- O robô deve receber comandos de um operador por meio de um sistema de controle remoto.
- O robô deve ser capaz de se movimentar a partir dos comandos recebidos.
- O robô deve permitir a execução de movimentos de avanço e recuo.
- O robô deve permitir a realização de curvas para ambos os lados.
- O robô deve possuir um sistema de fixação para transportar um balão.
- o robô deve possuir uma clava espinhosa para estourar o balão adversário
# 5.5. requisitos não funcionais
-	O robô deve apresentar estabilidade durante a movimentação.
-	O sistema de controle deve responder adequadamente aos comandos.
-	O circuito deve possuir proteções elétricas mínimas.
-	A estrutura deve apresentar resistência mecânica suficiente para a competição.
-	O sistema deve apresentar alimentação elétrica estável.
-	O robô deve ser compacto e apresentar boa distribuição de massa.
# 6. restrições
| Componente | Quantidade |
|---|---:|
| Placa de desenvolvimento ESP-WROOM32 | 1 |
| Motor escovado DC de 6 V | 2 |
| Regulador de tensão L7805CV | 2 |
| Driver de motor DRV8833 | 1 |
| Placa perfurada | 1 |
| Capacitores e resistores | Disponível |
| Conectores, fios e protoboards | Disponível |
| Filamento para impressão 3D | Disponível |

IMPORTANTE: a utilização desses recursos deverá orientar as decisões relacionadas à modelagem.
# 7. Metodologia de Desenvolvimento

