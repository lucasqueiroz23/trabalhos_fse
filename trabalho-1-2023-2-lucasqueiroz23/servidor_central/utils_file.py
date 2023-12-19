def obter_dicionario_cruzamentos():
    """
    Obter um dicionário com os dados dos cruzamentos
    (ip, porta) e do servidor central
    """
    file = open("../config_servidores", "r")
    texto_arquivo = file.read()
    texto_arquivo = texto_arquivo.split("\n\n")

    dict_retorno = {}
    for texto in texto_arquivo:
        t = texto.split("\n")

        ip = t[1].replace("IP=", "")
        host = int(t[2].replace("PORTA=", ""))
        dict_retorno[t[0]] = (ip, host)

    return dict_retorno

