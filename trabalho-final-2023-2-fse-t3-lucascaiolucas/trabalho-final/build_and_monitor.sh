#!/bin/bash

### antes de rodar esse script, certifique-se de rodar `get_idf` no terminal

idf.py set-target esp32
idf.py -p /dev/ttyUSB0 flash
idf.py monitor
