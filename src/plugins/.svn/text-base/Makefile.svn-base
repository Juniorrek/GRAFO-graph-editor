# Plugins
PLUGINS = euler.so paley.so vertexexample.so\
          edgeexample.so components.so conected.so\
          getsp.so hw.so delaunay.so chordalize.so\
          stargraphexample.so selectoddexample.so


# Make rules
%.so : %.c
	$(CC) -Wall -fPIC -shared $< -o $@ $(CFLAGS)

all: $(PLUGINS)

clean:
	rm -f $(PLUGINS)
