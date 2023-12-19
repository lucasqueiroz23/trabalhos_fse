from utils_file import obter_dicionario_cruzamentos

# ip do servidor. Obviamente, é o localhost.
HOST = '127.0.0.1'

# Dados dos servidores distribuídos e central
DADOS_CRUZAMENTOS = obter_dicionario_cruzamentos()
CHAVES_DADOS_CRUZAMENTOS = ['CRUZAMENTO_1', 'CRUZAMENTO_2', 'SERVIDOR_CENTRAL']

# Mensagens padronizadas que chegam dos distribuídos para o central
MSG_MULTA_VERMELHO = b"multa_vermelho"
MSG_MULTA_VELOCIDADE = b"multa_velocidade"
MSG_CARRO_PASSOU_C1_P_1 = b"c1_principal_1"
MSG_CARRO_PASSOU_C2_P_1 = b"c2_principal_1"
MSG_CARRO_PASSOU_C1_P_2 = b"c1_principal_2"
MSG_CARRO_PASSOU_C2_P_2 = b"c2_principal_2"

MSG_CARRO_PASSOU_C1_A_1 = b"c1_aux_1"
MSG_CARRO_PASSOU_C2_A_1 = b"c2_aux_1"
MSG_CARRO_PASSOU_C1_A_2 = b"c1_aux_2"
MSG_CARRO_PASSOU_C2_A_2 = b"c2_aux_2"

# Mensagens padronizadas que vão do central para os distribuídos
MSG_EMERGENCIA = b"emergencia"
MSG_EMERGENCIA_C1 = "Enviando mensagem ao cruzamento 1: emergencia\n"
MSG_EMERGENCIA_C2 = "Enviando mensagem ao cruzamento 2: emergencia\n"

MSG_NOTURNO = b"noturno"
MSG_NOTURNO_C1 = "Enviando mensagem ao cruzamento 1: noturno\n"
MSG_NOTURNO_C2 = "Enviando mensagem ao cruzamento 2: noturno\n"

# Tamanho máximo da mensagem
BUFFER_SIZE = 25
