# GRAFO-graph-editor
### AlgLab
### ARG - Algorithm Research Group - UFPR
O programa GRAFO foi desenvolvido para auxiliar no aprendizado de teoria dos grafos e algoritmos.
### Autores:
- Murilo Vicente Goncalves da Silva <murilo@inf.ufpr.br>
- Oliver Matias van Kaick <oliver@inf.ufpr.br>
- Ulisses Cordeiro Pereira <ulisses@cordeiropereira.com.br>
### Instalação:
#### Compile:
`cd src;`

`./configure`

then:

`make`

or

`make CFLAGS=<your flags>`

You need gtk 2.0 libraries to compile the gtk interface.
If gtkglarea 2.0, GL and GLU are present, the gtk interface is
compiled with opengl support.

#### Install:
`make install`
      
Installs the executable in the system (directory /usr/bin).
You need to be root to do this.

`make user_install`

Installs the executable in the user directory (directory${HOME}/bin).


#### Configuration file:
      The file '$(HOME)/.graforc' is created by the program. Preferences
      are stored in this file.

### Downloads:
[tar.bz2](http://www.inf.ufpr.br/arg/alglab/grafo/downloads/grafo_2008.0704.063.00.tar.bz2) (versão 2008.0704.063.00)

[binário](http://www.inf.ufpr.br/arg/alglab/grafo/downloads/grafo) (preparado para rodar nas servidoras do DINF)

### Documentação:
Trabalho de Graduação de Ulisses Cordeiro Pereira ([html](http://www.inf.ufpr.br/arg/alglab/grafo/docs/grafodoc.html) [pdf](http://www.inf.ufpr.br/arg/alglab/grafo/docs/20080701_TGII-GRAFO-graph-editor.pdf))
