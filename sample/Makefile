LIBS 	= cz-sample.dai cz-management.dai \
	  cz-soil.dai pl-sample.dai pl-management.dai pl-soil.dai \
	  sk-sample.dai sk-management.dai sk-soil.dai it-sample.dai\
	  it-management.dai it-soil.dai dk-soil.dai log-sample.dai \
	  init-soil.dai pedo-soil.dai irrigation.dai dk-management.dai \
	  dk-veg-man.dai
SETUPS  = test-spawn.dai test-cp.dai test-scn.dai \
	test-post.dai test-organic.dai rootmatch.dai \
	test-refman.dai genweather.dai test-GP2D.dai test-colloids.dai \
	test-prd.dai test-aba.dai OpenMI_simple.dai OpenMI_management.dai \
	heat-properties.dai batch.dai sample.dai test.dai DrainDrying.dai \
	test-minimize.dai
DATA = rootdata.ddf \
	dk-foulum.dwf dk-jyndevad.dwf cz-valecov.dwf pl-jadwisin.dwf \
	sk-bratislava.dwf it-bologna.dwf dk-taastrup.dwf example.gwt it-cer.gwt
SAFIR = safir-common.dai safir-site.dai
TEXT	= Makefile README $(SETUPS) $(DATA) $(LIBS)
REMOVE 	=  syntax.h syntax.C

DAISY   = /usr/local/daisy/$(HOSTTYPE)/daisy

all:
	echo Nothing to do here.

# Create a ZIP file with all the sources.
#
daisy-sample.zip:	$(TEXT)
	rm -f daisy-sample.zip
	zip daisy-sample.zip $(TEXT)

# Move it to ftp.
#
dist:
	@if [ "X$(TAG)" = "X" ]; then echo "*** No tag ***"; exit 1; fi
	$(MAKE) daisy-sample.zip
	mv -f daisy-sample.zip $(FTPDIR)/daisy-lib/daisy-sample-$(TAG).zip
	rm -f $(FTPDIR)/daisy-sample.zip
	(cd $(FTPDIR); ln -s daisy-lib/daisy-sample-$(TAG).zip daisy-sample.zip)

# Prepare setup file.
#
setup:
	mkdir $(SETUPDIR)/sample
	cp $(TEXT) $(SETUPDIR)/sample

# Update the repository.
#
checkin: $(TEXT) 
	@if [ "X$(TAG)" = "X" ]; then echo "*** No tag ***"; exit 1; fi
	-git add $(TEXT) 
	-rm -f $(REMOVE) 
	-git rm -f --ignore-unmatch $(REMOVE) 


add:
	git add $(TEXT)

# Check
#
check: new/Cosby-Nitrogen-Balance.sum
	-@if cmp new/Cosby-Nitrogen-Balance.sum old/Cosby-Nitrogen-Balance.sum ; then echo Cosby Nitrogen Balance unchanged; else echo Cosby Nitrogen Balance changed; wdiff -t new/Cosby-Nitrogen-Balance.sum old/Cosby-Nitrogen-Balance.sum; fi

new/Cosby-Nitrogen-Balance.sum: $(DAISY) test-Cosby.dai
	$(DAISY) test-Cosby.dai

accept: 
	cp new/Cosby-Nitrogen-Balance.sum old
