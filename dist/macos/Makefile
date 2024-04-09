include ../../config

CWD=`pwd`

ifeq ($(VERSION),)
	VOLUME_NAME=$(APPNAME)
else
	VOLUME_NAME=$(APPNAME)-$(VERSION)
endif

installer: $(APPNAME).app
	@echo "** Creating disk image"
	rm -f /tmp/$(VOLUME_NAME).dmg
	hdiutil create -size 16m -fs HFS+ -volname "$(VOLUME_NAME)" /tmp/$(VOLUME_NAME).dmg
	hdiutil attach /tmp/$(VOLUME_NAME).dmg
	cp -r ./$(APPNAME).app /Volumes/$(VOLUME_NAME)
	ln -s /Applications /Volumes/$(VOLUME_NAME)/Applications
	cp ./{LICENSE,INSTALL} /Volumes/$(VOLUME_NAME)
	hdiutil detach /Volumes/$(VOLUME_NAME)
	hdiutil convert /tmp/$(VOLUME_NAME).dmg -format UDZO -o /tmp/$(VOLUME_NAME)_ro.dmg
	@echo "** Copying disk image into $(CWD)"
	mv /tmp/$(VOLUME_NAME)_ro.dmg ./$(VOLUME_NAME).dmg
