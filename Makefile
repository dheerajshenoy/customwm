CXX		:= g++
LIB		:= X11
WMNAME 		= customwm
EXEC		= $(WMNAME)


SRC	:= src
BIN	:= bin
INCLUDE :=

PREFIX?= /usr
BINDIR?= $(PREFIX)/bin

all: $(BIN)/$(EXEC)

install: all
	@echo "Installing..."
	install -Dm 755 $(BIN)/$(EXEC) $(DESTDIR)$(BINDIR)/$(WMNAME)
	@echo "Done"

$(BIN)/$(EXEC): $(SRC)/$(WMNAME).cpp
	@echo "Building..."
	$(CXX) -l$(LIB) $^ -o $(BIN)/$(EXEC)

clean:
	@echo "Clearing..."
	-rm $(BIN)/*
