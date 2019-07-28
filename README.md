# WebCam2C
Creación de imágenes 3D con la técnica del Anaglifo

Son necesarias dos librerías de OpenCV: opencv_world410.dll para el proyecto en Release o opencv_world410d.dll para el proyecto en Debug.

Tratando imágenes de fichero y fusionando:
WebCam2C fichIzq fichDch fichRes
Fusiona las dos imágenes y guarda el resultado en fichRes

Para webcam
WebCam2C

En modo webcam
- Esc: Cierra el programa.
- F1, F2 y F3 : Muestran u ocultan las pantallas izquierda,\n\t\tderecha y de anaglifo, respectivamente.
- I,i : Intercambia las cámaras.
- F,f : Toma una foto y la guarda en un fichero.
- D,d : Toma una foto y la guarda en dos ficheros, uno por cada ojo.
- T,t : Toma una foto y la guarda en tres ficheros, uno por cada ojo y el anaglifo.
- V,v : Inicia y finaliza la grabación de vídeo.
- C,c : Centra la imágenes, a la posición inicial.
- P,p : Intercambia el color de filtro de cada ojo. No equivale\n\t\ta I, porque I influye en la perspectiva.
- K,k : Cambia los colores de cada filtro.
- Flechas izquierda, derecha, arriba y abajo : Mueven la imagen de\n\t\tla cámara derecha a izquierda, derecha, arriba y abajo; respectivamente.
- AvPag y RePag : Rotan la imagen de la cámara de la derecha.
Estas funcionalidades sirven para calibrar por software la alineación\n\t\tde las cámaras.

En esta nueva versión permite guardar 3 imágenes, Opc=T y además muestra las resoluciones compatibles de las cámaras para seleccionar.

Es un programa cuya finalidad es la experimentación y la didáctica. El código puede optimizarse enormemente.
