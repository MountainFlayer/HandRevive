import requests
import time

def toggle_motor(motor, pos, ESP32_IP):
    '''Arguments:
       String motor (pulgar, indice, corazon, anular, menique o all)
       String pos (0, 180)
       String ESP32_IP (192.168.X.XX)'''
    url = ("http://" + ESP32_IP + "/?motor={}&pos={}&").format(motor, pos)
    response = requests.get(url)
    if response.status_code == 200:
        print("Motor {} toggled to position {}".format(motor, pos))
    else:
        print("Failed to toggle motor")

def abrirMano(IP):
    toggle_motor("all", "180", IP)

def cerrarMano(IP):
    toggle_motor("all", "0", IP)

def abrirPulgar(IP):
    toggle_motor("pulgar", "180", IP)

def cerrarPulgar(IP):
    toggle_motor("pulgar", "0", IP)

def abrirIndice(IP):
    toggle_motor("indice", "180", IP)

def cerrarIndice(IP):
    toggle_motor("indice", "0", IP)

def abrirCorazon(IP):
    toggle_motor("corazon", "180", IP)

def cerrarCorazon(IP):
    toggle_motor("corazon", "0", IP)

def abrirAnular(IP):
    toggle_motor("anular", "180", IP)

def cerrarAnular(IP):
    toggle_motor("anular", "0", IP)

def abrirMenique(IP):
    toggle_motor("menique", "180", IP)

def cerrarMenique(IP):
    toggle_motor("menique", "0", IP)
