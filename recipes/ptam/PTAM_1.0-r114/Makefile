# DO NOT DELETE THIS LINE -- make depend depends on it.

LINKFLAGS = -llapack -lblas -lf2c -lGL -lGLU -lreadline -lcurses -lGVars3 -lcvd -lopencv_core -lopencv_imgproc -lopencv_highgui
COMPILEFLAGS = -D_LINUX -D_REENTRANT -Wall

# Edit this line to change video source
VIDEOSOURCE = VideoSource_Linux_OpenCV.o

OBJECTS=	main.o\
		GLWindow2.o\
		GLWindowMenu.o\
		$(VIDEOSOURCE)\
		System.o \
		ATANCamera.o\
		KeyFrame.o\
		MapPoint.o\
		Map.o\
		SmallBlurryImage.o\
		ShiTomasi.o \
		HomographyInit.o \
		MapMaker.o \
		Bundle.o \
		PatchFinder.o\
		Relocaliser.o\
		MiniPatch.o\
		MapViewer.o\
		ARDriver.o\
		EyeGame.o\
		Tracker.o

CALIB_OBJECTS=	GLWindow2.o\
		GLWindowMenu.o\
		$(VIDEOSOURCE)\
		CalibImage.o \
		CalibCornerPatch.o\
		ATANCamera.o \
		CameraCalibrator.o

All: PTAM CameraCalibrator

PTAM: $(OBJECTS)
	$(CC) -o PTAM $(OBJECTS) $(LINKFLAGS) $(LDFLAGS)

CameraCalibrator:$(CALIB_OBJECTS)
	$(CC) -o CameraCalibrator $(CALIB_OBJECTS) $(LINKFLAGS) $(LDFLAGS)


%.o: %.cc
	$(CXX) $< -o $@ -c $(CFLAGS) $(COMPILEFLAGS)

clean:
	rm *.o


depend:
	rm dependecies; touch dependencies
	makedepend -fdependencies $(INCLUDEFLAGS) $(MOREINCS) *.cc *.h


-include dependencies









