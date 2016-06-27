# Stenography
- stegowav

```
Embeber:
- embed		  Indica que se utilizará stegowav para ocultar información
- in	      Archivo que se va a ocultar
- p 	      Archivo wav portador
- out	  	  Archivo wav salida
- steg  	  Algoritmo de esteganografiado (LSB1, LSB4, LSBE)

OPCIONALES
- a	        Algoritmo de cifrado (aes128, aes192, aes256, des)
- m	        Modo utilizado en el cifrado (cbc, ofb, ecb, cfb)
- pass  	  Contraseña del cifrado 

Ejemplo: ./stegowav –embed –in “data.txt” –p “audio.wav” –out “stegowav.wav” –steg “LSB1”
```
```
Extraer:
- extract		Indica que se utilizará stegowav para extraer información oculta.
- p 	      Archivo wav portador
- out		    Archivo que se va a extraer (nombre sin extensión)
- steg	    Algoritmo de esteganografiado (LSB1, LSB4, LSBE)

OPCIONALES
- a	        Algoritmo de cifrado (aes128, aes192, aes256, des)
- m	        Modo utilizado en el cifrado (cbc, ofb, ecb, cfb)
- pass	    Contraseña del cifrado 

Ejemplo: ./stegowav –extract –p “stegowav.wav” –out “data” –steg “LSB1”
```
# Steganalysis
- steganalysisFileLength:

  Analiza para un archivo wav la posibilidad de que tenga data embebida con cada una de las técnicas LSB implementadas. 
  Sólo recibe como parámetro el path del archivo a analizar.

```
Ejemplo: ./steganalysisFileLength "data/audio.wav"
```

- steganalysisFileContent:

  Levanta, de ser posible, la data posiblemente embebida en el archivo wav portador con la técnica indicada y lo escribe en el archivo que se paso sin extensión para después poder ser analizado con algún programa que analice el formato como trID o el comando file de Linux. 

```
Input:
- p 	      Archivo wav portador
- out		    Archivo que se va a extraer (nombre sin extensión)
- steg	    Algoritmo de esteganografiado (LSB1, LSB4, LSBE)

Ejemplo: ./steganalysisFileContent -p "data/audio.wav" -out "data/unknown" -steg "LSB1" 
```

- steganalysisEOF

Recibe como parámetro el nombre del archivo wav a analizar y compara el tamaño real del archivo wav con el que indica su header, y en caso de no concordar indica en cuantos bytes difieren.

```
Ejemplo: ./steganalysisEOF "data/audio.wav"
```

-------------

ITBA - Cryptography and Security TPE
