[![Open in Visual Studio Code](https://classroom.github.com/assets/open-in-vscode-718a45dd9cf7e7f842a935f5ebbe5719a5e09af4491e668f4dbf3b35d5cca122.svg)](https://classroom.github.com/online_ide?assignment_repo_id=12044155&assignment_repo_type=AssignmentRepo)

# Trabalho 1 (2023-2)

Trabalho 1 da disciplina de Fundamentos de Sistemas Embarcados (2023/2).
[Link para apresentação](https://www.youtube.com/watch?v=0DvMSRVjzeY).

- Aluno: Lucas Henrique Lima de Queiroz
- Matrícula: 190091703

## Como rodar

### Servidores distribuídos

Na pasta `servidor_distribuido`, use o comando `make` para compilar o arquivo `cruzamento`.

O modo de uso do binário é: `./cruzamento <opcao> `, sendo que `<opcao>` pode ser `--c1`, para o cruzamento 1, ou `--c2`, para o cruzamento 2.

Sendo assim: para rodar o cruzamento 1, use `./cruzamento --c1`. Para rodar o cruzamento 2, use `./cruzamento --c2`. 

OBS: caso queira executar os dois cruzamentos em um único sistema computacional, compile o arquivo `abrir_servidores_distribuidos.c` e rode o binário resultante. 

As configurações (IP e porta) dos servidores distribuídos encontram-se no arquivo `config_servidores`, que está na pasta raíz do repositório.

### Servidor central

Na pasta `servidor_central`, use o comando `python3 servidor_central.py` para rodar o servidor central.

As configurações do servidor central (IP e porta) estão, também, no arquivo `config_servidores`, na pasta raíz.

## Arquitetura do sistema

### Servidores distribuídos

#### Módulos

Os módulos dos servidores distribuídos são divididos em arquivos .h e .c. Nos arquivos .h, estão definidos structs, constantes, assinaturas e documentações de funções. Nos arquivos .c, estão definidas as funções em si.

- `cruzamento.c`: arquivo principal de um servidor distribuído. Ele controla um cruzamento (definido como o conjunto de sensores e semáforos). Cada semáforo tem uma thread própria. O servidor distribuído conversa com o servidor central por meio de sockets, em uma conexão TCP/IP.
- `semaforos`: módulo relacionado à criação e gerenciamento de semáforos, i.e., realiza a mudança de estados, verifica se o timebox dos semáforos está sendo respeitado, troca de modos (normal, emergência, noturno), etc.
- `sensores`: módulo relacionado aos sensores de um cruzamento. Calcula a velocidade de um carro e toma decisões a partir disso.
- `utils_raspberry`: módulo de utilidades relacionado à inicialização e "fechamento" da placa.
- `utils_sockets`: módulo de utilidades relacionado à utilização de sockets, processamento e envio de mensagens entre servidor distribuído e central, etc.
- `utils_file`: módulo de utilidades relacionado a arquivos. Nesse caso, esse módulo serve para carregar os conteúdos do arquivo `config_servidores`, que está na pasta raíz.
- `abrir_servidores_distribuidos.c`: Programa extra. Caso queira abrir os dois processos distribuídos no mesmo sistema computacional, compile-o e rode o binário resultante.
- `reset_gpio.c`: Programa extra. Caso queira resetar todos os pinos da placa, compile-o (OBS: use a flag -lbcm2835) e rode o binário resultante.



### Servidor Central

#### Módulos

- `servidor_central.py`: arquivo principal do servidor central. Ele faz o interfaceamento com os servidores distribuídos (recupera dados, envia sinais para troca de modos de uso dos cruzamentos, etc).
- `settings.py`: arquivo com configurações úteis.
- `utils_file.py`: módulo com uma única função, que carrega os dados do arquivo `config_servidores` em um dicionário python.
- `dados.json`: arquivo que guarda os dados dos cruzamentos, ou seja, quantidade de carros, infrações, quantidade de infrações por tipo, etc.
