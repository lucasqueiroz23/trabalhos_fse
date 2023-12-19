import sys
import json
import socket
import threading
from time import sleep
import settings

condicao_thread = threading.Event()

dados = open("dados.json")
dados = json.load(dados)


def fechar_servidor_central(ip_porta):
    """
    Fechar o servidor central. Manda uma mensagem (para sair do loop while)
    e depois fecha a thread.
    @params ip_porta Tupla com ip e porta do servidor central
    """
    print("Fechando servidor central...")
    condicao_thread.set()
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
        s.connect(ip_porta)
        s.sendall(b"fechar")
        s.close()
    thread_servidor.join(timeout=1)
    sys.exit(0)


def obter_tupla_ip_porta(chave):
    """
    Obter a tupla com ip e porta.
    @params chave Chave que indica qual o servidor de interesse.
    """
    ip_porta = settings.DADOS_CRUZAMENTOS[chave]
    ip = ip_porta[0]
    porta = ip_porta[1]

    return (ip, porta)


def enviar_mensagem(ip_porta, mensagem):
    """
    Notificar, via socket, um cruzamento.
    """
    try:
        with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
            s.connect(ip_porta)
            s.sendall(mensagem)
            s.close()
            print("Mensagem enviada com sucesso")
    except Exception:
        print("Erro ao enviar mensagem")


def alterar_modo(modo):
    mensagem_c1 = ''
    mensagem_c2 = ''
    mensagem_socket = ''

    if modo == "noturno":
        mensagem_c1 = settings.MSG_NOTURNO_C1
        mensagem_c2 = settings.MSG_NOTURNO_C2
        mensagem_socket = settings.MSG_NOTURNO

    elif modo == "emergencia":
        mensagem_c1 = settings.MSG_EMERGENCIA_C1
        mensagem_c2 = settings.MSG_EMERGENCIA_C2
        mensagem_socket = settings.MSG_EMERGENCIA

    print(mensagem_c1)
    enviar_mensagem(obter_tupla_ip_porta(settings.CHAVES_DADOS_CRUZAMENTOS[0]), mensagem_socket)
    print(mensagem_c2)
    enviar_mensagem(obter_tupla_ip_porta(settings.CHAVES_DADOS_CRUZAMENTOS[1]), mensagem_socket)

    sleep(1)


def salvar_json():
    """
    Salvar o arquivo de dados.
    """
    with open("dados.json", "w") as output:
        json.dump(dados, output)


def multar(tipo_infracao):
    """
    Registrar uma ocorrência de multa.
    """
    dados["numero_infracoes"] += 1
    dados[tipo_infracao] += 1
    salvar_json()


def registrar_passagem_carro(cruzamento, via):
    """
    Registrar a passagem de um carro numa via e salvar.
    """
    dados["passagem_carros"][cruzamento][via] += 1
    salvar_json()


def abrir_socket_servidor():
    try:
        ip_porta_central = obter_tupla_ip_porta('SERVIDOR_CENTRAL')
        with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
            s.bind(ip_porta_central)
            s.listen()
            print(f"Servidor central escutando na porta {ip_porta_central[1]}\n")
            while not condicao_thread.is_set():
                conn, addr = s.accept()
                with conn:
                    while True:
                        data = conn.recv(settings.BUFFER_SIZE)
                        if not data:
                            break

                        # multa sinal vermelho
                        if data == settings.MSG_MULTA_VERMELHO:
                            multar("infracoes_vermelho")

                        # multa de velocidade
                        elif data == settings.MSG_MULTA_VELOCIDADE:
                            multar("infracoes_velocidade")

                        elif data == settings.MSG_CARRO_PASSOU_C1_P_1:
                            registrar_passagem_carro("cruzamento1", "principal1")

                        elif data == settings.MSG_CARRO_PASSOU_C1_P_2:
                            registrar_passagem_carro("cruzamento1", "principal2")

                        elif data == settings.MSG_CARRO_PASSOU_C1_A_1:
                            registrar_passagem_carro("cruzamento1", "auxiliar1")

                        elif data == settings.MSG_CARRO_PASSOU_C1_A_2:
                            registrar_passagem_carro("cruzamento1", "auxiliar2")

                        elif data == settings.MSG_CARRO_PASSOU_C2_P_1:
                            registrar_passagem_carro("cruzamento2", "principal1")

                        elif data == settings.MSG_CARRO_PASSOU_C2_P_2:
                            registrar_passagem_carro("cruzamento2", "principal2")

                        elif data == settings.MSG_CARRO_PASSOU_C2_A_1:
                            registrar_passagem_carro("cruzamento2", "auxiliar1")

                        elif data == settings.MSG_CARRO_PASSOU_C2_A_2:
                            registrar_passagem_carro("cruzamento2", "auxiliar2")

    except Exception:
        print("Erro na comunicação\n")


thread_servidor = threading.Thread(target=abrir_socket_servidor)


def verificar_cruzamentos():
    """
    Verificar dados dos cruzamentos.
    """
    print(f"Numero total de infrações: {dados['numero_infracoes']}")
    print(f"Numero de infrações - velocidade: {dados['infracoes_velocidade']}")
    print(f"Numero de infrações - sinal vermelho: {dados['infracoes_vermelho']}")

    print("\n")
    print(80*'*')
    print("Passagem de carros no cruzamento 1:")
    print(f"Na via principal 1: {dados['passagem_carros']['cruzamento1']['principal1']}")
    print(f"Na via principal 2: {dados['passagem_carros']['cruzamento1']['principal2']}")
    print(f"Na via auxiliar 1: {dados['passagem_carros']['cruzamento1']['auxiliar1']}")
    print(f"Na via auxiliar 2: {dados['passagem_carros']['cruzamento1']['auxiliar2']}")
    print(80*'*')

    print("\n")
    print(80*'*')
    print("Passagem de carros no cruzamento 2:")
    print(f"Na via principal 1: {dados['passagem_carros']['cruzamento2']['principal1']}")
    print(f"Na via principal 2: {dados['passagem_carros']['cruzamento2']['principal2']}")
    print(f"Na via auxiliar 1: {dados['passagem_carros']['cruzamento2']['auxiliar1']}")
    print(f"Na via auxiliar 2: {dados['passagem_carros']['cruzamento2']['auxiliar2']}")
    print(80*'*')


def menu():
    """
    Receber uma entrada do usuário e decidir o que fazer com
    base nela.
    """
    print("Escolha uma opção: ")
    print("1 - Verificar cruzamentos")
    print("2 - Modo de emergência")
    print("3 - Modo noturno")
    print("4 - Fechar servidor central")
    print("\n")

    entrada = input(">> ")

    if entrada == "1":
        verificar_cruzamentos()
    elif entrada == "2":
        alterar_modo("emergencia")
    elif entrada == "3":
        alterar_modo("noturno")
    elif entrada == "4":
        fechar_servidor_central(settings.DADOS_CRUZAMENTOS['SERVIDOR_CENTRAL'])
    else:
        print("Insira uma entrada válida!")

    print("\n")


if __name__ == '__main__':

    # abrir thread para ouvir os servidores distribuidos.
    thread_servidor.start()

    sleep(1)

    while True:
        menu()
