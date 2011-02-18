# -
# Copyright (c) 2009-2010 by Fstorage Team.
# Author: Lukasz Marcin Podkalicki <lukasz@fstorage.pl>
# -

.PHONY: dist clean

VERSION=0.3
DIST_NAME=fstorage_client_php-$(VERSION)

clean:
	 -rm -rf $(DIST_NAME)
	 -rm *.tar *.tar.gz

dist:
	-rm -rf $(DIST_NAME)
	-mkdir $(DIST_NAME)
	cp *.h *.cc LICENSE README config.m4 $(DIST_NAME)/
	tar cvf $(DIST_NAME).tar $(DIST_NAME)/
	gzip -f --best $(DIST_NAME).tar
	@echo $(DIST_NAME).tar.gz generated.

