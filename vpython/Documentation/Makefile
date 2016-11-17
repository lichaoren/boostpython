

define HOWTO_MAKEDIR_lib
        if test ! -d ${OBJSDIR} ; then \
          mkdir      ${OBJSDIR} 2> /dev/null ; \
          chmod 777  ${OBJSDIR} ; \
        fi
endef


all: install 
	doxygen vpython.cfg
	
# Copy only first time
install-light:
	-@if test ! -d html/Reference; \
	    then mkdir -p html/Reference ;\
	    cp ./Reference/*.pdf html/Reference; \ 
	 fi

# Force install of ref. documentation
install:
	-@if test ! -d html/Reference; then mkdir -p html/Reference ; fi
	-@cp ./Reference/*.pdf html/Reference 

clean:
	-@if test html; then \rm -rf html 2> /dev/null ; fi
	-@if test latex; then \rm -rf latex 2> /dev/null ; fi
	-@if test xml; then \rm -rf xml 2> /dev/null ; fi

pdf: all
	cd latex; make

