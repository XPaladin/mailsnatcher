# Descripción #
Utilizando pcap, se pretende capturar los mails leídos usando una conexión insegura.
La idea es mostrar estos mails en una pantalla pública con el fin de concienciar el uso de conexiones seguras, tal como la Wall of Sheep de la DefCon

![http://i.zdnet.com/blogs/wallofsheep2007-480.png](http://i.zdnet.com/blogs/wallofsheep2007-480.png)

En la Wall of Sheep se muestran las claves y se manejan cookies para filtrar información. En este proyecto sólo se pretende leer el correo que pasa por la red sin encriptar, ya sea usando POP3, IMAP o una interfaz web (i.e., protocolo HTTP).

# Plan de trabajo #
El trabajó se dividirá en varias etapas, cada una enfocada en capturar datos en un cierto formato. A continuación se muestra un plan de trabajo inicial, el cual puede verse cambiado dependiendo de los resultados obtenidos,


| Tarea \ Semana | 6 | 7`*` | V | 8 | O | 9 | 10 | 11 | 12 | 13 | 14 | 15`*` |
|:---------------|:--|:-----|:--|:--|:--|:--|:---|:---|:---|:---|:---|:------|
| Lectura de protocolo POP | X | X | X |  |  |  |  |  |  |  |  |  |
| Lectura de protocolo SMTP |  |  |  | X | X | X | X |  |  |  |  |  |
| Lectura de protocolo IMAP |  |  |  |  |  |  | X | X | X |  |  |  |
| Lectura de protocolo HTTP |  |  |  |  |  |  |  |  |  | X | X | X |

  1. Las semanas marcadas con un asterisco (`*`) indican que son semanas que incluyen una entrega del proyecto.
  1. La semana "V" indica la semana de vacaciones, y la semana "O" indica la semana olímpica.

# Trabajo Realizado #
De los cuatro protocolos contemplados inicialmente, la aplicación sólo terminó con la capacidad de leer a tres de ellos. Éstos son `POP3`, `SMTP` y `HTTP`.

La razón de su elección fueron una mezcla de conveniencia e impacto. Los protocolos `POP3` y `SMTP` fueron implementados dada su similitud y grado de simplicidad. El protocolo `HTTP`, sin embargo, contaba con un grado de dificultad bastante más alto. Se decidió implementar este protocolo por sobre `IMAP` dado que es más usado.

## `POP3` y `SMTP` ##
Como se mencionó, éstos son protocolos bastante simples. Sus mensajes consisten únicamente en símbolos `ASCII` de 7 bits, lo cual hace su monitoreo mucho más fácil para una persona. Esto hace que su desarrollo se pueda llevar a cabo de forma más expedita. Otra ventaja es que ambos protocolos son muy similares, por lo cual la implementación de uno sirve para implementar el otro casi completamente.

## `HTML` ##
Este protocolo resultó ser más complicado de lo presupuestado inicialmente. El protocolo `HTML` tiene muchas opciones para transferir datos, lo cual hace que su implementación resulten engorrosa y que sea fácil introducir _bugs_ en el código que arruinen la visualización.

Aunque la posibilidad de enviar datos en formato `ASCII` existe y es usada ampliamente, otra posibilidad bastante usada es la de comprimir los datos utilizando el programa `gzip`, lo cual introduce la necesidad de procesar los datos antes de poder visualizarlos.

Una complejidad aún mayor se añade al utilizar el método de enviar los datos por trozos, llamado _chunking_. Al utilizar este método, los datos se parten en segmentos que se envían precedidos por su largo, y el total debe ser ensamblado por el cliente. Este método de envío de datos es independiente de la codificación de los datos, por lo cual además de estar comprimido, un mensaje puede ser enviado por trozos.

Finalmente, el procesador de `HTTP` implementado puede visualizar datos enviados utilizando _chunking_ y/o comprimidos con `gzip`.

### Sitios vulnerables ###
Se intentó vulnerar principalmente tres sitios de correo, Hotmail, Google Mail y la página de correos Webmail del DCC. Se descubrió que, de estos tres sitios, Google Mail y el servidor del DCC encriptan sus datos, por lo cual es más difícil recuperar los datos de forma que se puedan visualizar. Hotmail, en la otra mano, mostró ser vulnerable al _sniffing_.

# Funcionamiento #
La aplicación desarrollada utiliza la biblioteca `libpcap` para acceder a los datos que se detectan en una interfaz de red. Esta biblioteca luego entrega una estructura de datos que representa los bytes captados desde la red, para disposición del programador.

La forma de filtrar qué paquetes escuchar y qué paquetes no fue en base a los puertos. Se decidió que sólo se "escucharía" en los puertos estándares de cada protocolo. Es decir, el puerto 25 para `SMTP`, el 110 para `POP3` y el 80 para `HTTP`. Esto fue porque resultaba más complejo implementar un filtro que escuche en todos los puertos y determine a partir de los datos de aplicación el protocolo usado.

Al capturar un paquete que se dirija o provenga de algún puerto en el cual se está escuchando, éste se toma y se decide de qué manera procesarlo, lo cual depende de su puerto. Luego se extrae la información contenida dentro de él.

## Interfaz usuario ##
La interfaz final es bastante cruda, y consiste en simplemente llamar a la aplicación mediante una instrucción por línea de comando. Luego, los datos se muestran por terminal.

Se descubrió, sin embargo, que los datos capturados del protocolo `HTTP` se pueden guardar a un archivo y ser visualizados utilizando un navegador.

# Conclusiones #
La conclusión más importante fue que en realidad es relativamente fácil para un atacante espiar la comunicación entre un cliente y un servidor, a menos de que se tomen medidas efectivas en contra de esto. La aplicación muestra que, si los datos no se encriptan, recuperarlos es una labor casi trivial.

Sin embargo, también se llegó a la conclusión de que no es tan fácil como utilizar `tcpdump`. Protocolos como `HTTP` envían datos de forma comprimida o separada por trozos, lo cual no permite fácilmente acceder a los datos, ya que éstos requieren cierto procesamiento antes de ser legibles. Esto es, sin embargo, sólo _seguridad mediante osruridad_, algo en lo cual nunca hay que confiar datos de naturaleza sensible.

## Otra información ##
La biblioteca `libpcap` es una biblioteca de captura de paquetes para el lenguaje de programación `C`. Con ésta pretendimos filtrar y extraer la información pertinente de los paquetes, de forma similar a la cual lo hacen las aplicaciones `tcpdump` y Wireshark (las cuales también utilizan `libpcap`).