include config

APPNAME=ssde

QMAKE_MACOS = /Developer/Qt5.8/5.8/clang_64/bin/qmake 
QMAKE_WIN = C:/Qt/Qt5.8.0/5.8/mingw53_32/bin/qmake.exe
MAKE_WIN = C:/Qt/Qt5.8.0/Tools/mingw530_32/bin/mingw32-make
QMAKE_UNIX=qmake

.PHONY: app clean install dist doc html

all: app 

app:
ifeq ($(PLATFORM), win32)
	(cd src; $(QMAKE_WIN) -spec win32-g++ main.pro; $(MAKE_WIN))
endif
ifeq ($(PLATFORM), macos)
	(cd src; $(QMAKE_MACOS) -spec macx-clang CONFIG+=x86_64 main.pro; make)
endif
ifeq ($(PLATFORM), unix)
	(cd src; $(QMAKE_UNIX) main.pro; make)
endif

doc:
	pandoc -o CHANGELOG.txt CHANGELOG.md
	pandoc -o CHANGELOG.html CHANGELOG.md
	pandoc -o README.txt README.md
	pandoc -o README.html README.md

html:
	pandoc -o README.html README.md
	open -a Safari README.html


clean:
	(cd src; make clean)
	(cd examples; make clean)

clobber: clean
	rm -f doc/lib/*
	\rm -f src/$(APPNAME).app/Contents/MacOS/$(APPNAME)
	\rm -f *~

install:
ifeq ($(PLATFORM), macos)
	cp -r src/$(APPNAME).app $(INSTALL_BINDIR)
else
	cp src/$(APPNAME) $(INSTALL_BINDIR)
endif

MACOS_DIST=/tmp/ssde

macos-dist:
	@echo "** Cleaning"
	make clobber
	@echo "** Configuring for MacOS distribution"
	./configure -platform macos
	@echo "** Building"
	(cd src; make)
	make macos-install
	make macos-installer

macos-install:
	@echo "** Installing in $(MACOS_DIST)"
	rm -rf $(MACOS_DIST)
	mkdir $(MACOS_DIST)
	cp -r src/$(APPNAME).app $(MACOS_DIST)/Ssde.app
	cp ./dist/macos/INSTALL $(MACOS_DIST)/INSTALL
	mkdir $(MACOS_DIST)/examples
	cp -r examples/*.fsd $(MACOS_DIST)/examples
	cp {CHANGELOG.txt,KNOWN-BUGS,LICENSE,README.txt} $(MACOS_DIST)

SSDE_VOLUME=Ssde-$(VERSION)

macos-installer:
	@echo "** Creating disk image"
	rm -f /tmp/Ssde.dmg
	hdiutil create -size 16m -fs HFS+ -volname "$(SSDE_VOLUME)" /tmp/Ssde.dmg
	hdiutil attach /tmp/Ssde.dmg
	cp -r $(MACOS_DIST)/Ssde.app /Volumes/$(SSDE_VOLUME)
	ln -s /Applications /Volumes/$(SSDE_VOLUME)/Applications
	cp -r $(MACOS_DIST)/examples /Volumes/$(SSDE_VOLUME)/Examples
	cp $(MACOS_DIST)/{CHANGELOG.txt,KNOWN-BUGS,LICENSE,README.txt,INSTALL} /Volumes/$(SSDE_VOLUME)
	hdiutil detach /Volumes/$(SSDE_VOLUME)
	hdiutil convert /tmp/Ssde.dmg -format UDZO -o /tmp/Ssde_ro.dmg
	@echo "** Copying disk image into ./binaries"
	mv /tmp/Ssde_ro.dmg ./binaries/Ssde-$(VERSION).dmg

WIN_SRC_DIR=~/Desktop/SF1/Qt

win32-pre:
	@echo "** Preparing Windows version.."
	@echo "** Cleaning source directory.."
	make clean
	@echo "** Copying source tree"
	if [ -d $(WIN_SRC_DIR)/ssde ]; then rm -rf $(WIN_SRC_DIR)/ssde.bak; mv $(WIN_SRC_DIR)/ssde $(WIN_SRC_DIR)/ssde.bak; fi
	(cd ..; cp -r ssde $(WIN_SRC_DIR))
	@echo "** Done"
	@echo "** Now, make win32, win32-install and win32-installer from Windows"
	@echo "** And then, back to MacOS and make win32-post"

win32-post:
	@echo "** Copying win32 installer exe in ./binaries"
	cp $(WIN_SRC_DIR)/ssde/dist/windows/Ssde_setup.exe ./binaries/Ssde-$(VERSION)_setup.exe

win32:
	@echo "******************************************************************************"
	@echo "**** WARNING: this make step must be invoked from a [mingw32(MSYS)] shell ****"
	@echo "******************************************************************************"
	./configure -platform win32
	@echo "** Building"
	make app
	@echo "** Done"

WIN_INSTALL_DIR=./build

DLL_REPO=../../Caml/caph/dlls

win32-install:
	@echo "** Installing in $(WIN_INSTALL_DIR)"
	rm -rf $(WIN_INSTALL_DIR)
	mkdir $(WIN_INSTALL_DIR)
	cp ./src/release/ssde.exe $(WIN_INSTALL_DIR)
	cp $(DLL_REPO)/{Qt5Core,Qt5Gui,Qt5Widgets,libgcc_s_dw2-1,libstdc++-6,libwinpthread-1}.dll $(WIN_INSTALL_DIR)
	mkdir $(WIN_INSTALL_DIR)/platforms
	cp $(DLL_REPO)/qwindows.dll $(WIN_INSTALL_DIR)/platforms
	cp {CHANGELOG.txt,KNOWN-BUGS,LICENSE,README.txt} $(WIN_INSTALL_DIR)
	cp ./dist/windows/icons/*.{bmp,ico} $(WIN_INSTALL_DIR)
	mkdir $(WIN_INSTALL_DIR)/examples
	cp -r examples/*.fsd $(WIN_INSTALL_DIR)/examples
	@echo "Done"

win32-installer:
	@echo "** Building self-installer"
	/C/Program\ Files/Inno\ Setup\ 5/iscc  ./dist/windows/SsdeSetup.iss
