
ROOT_PATH=$(shell pwd)
SFD_dir=$(ROOT_PATH)/FaceDetection
SFA_dir=$(ROOT_PATH)/FaceAlignment
SFI_dir=$(ROOT_PATH)/FaceIdentification

.PHONY: all
all: FaceIdentification
	@echo "all modules are build successfully"

.PHONY: FaceDetection
FaceDetection:
	if [ ! -e $(SFD_dir)/build/Makefile ]; then \
		rm -rf $(SFD_dir)/build && mkdir $(SFD_dir)/build; \
		cd $(SFD_dir)/build; \
		cmake ..; \
	fi; \
	cd $(SFD_dir)/build; \
	make -j2; \
	mkdir -p $(ROOT_PATH)/bin; \
	cp -rf $(SFD_dir)/model $(ROOT_PATH)/bin; \
	cp $(SFD_dir)/build/facedet_test $(ROOT_PATH)/bin;

.PHONY: FaceAlignment
FaceAlignment: FaceDetection
	if [ ! -e $(SFA_dir)/build/Makefile ]; then \
		rm -rf $(SFA_dir)/build && mkdir $(SFA_dir)/build; \
		cd $(SFA_dir)/build; \
		cp $(SFD_dir)/include/face_detection.h $(SFA_dir)/include;\
		cp $(SFD_dir)/build/libseeta_facedet_* $(SFA_dir)/build;\
		cmake ..; \
	fi; \
	cd $(SFA_dir)/build; \
	make -j2; \
	mkdir -p $(ROOT_PATH)/bin; \
	cp -rf $(SFA_dir)/model $(ROOT_PATH)/bin; \
	cp $(SFA_dir)/build/fa_test $(ROOT_PATH)/bin;\
	cp -rf $(SFA_dir)/data $(ROOT_PATH)/bin;

.PHONY: FaceIdentification
FaceIdentification: FaceAlignment
	rm -rf $(SFI_dir)/build && mkdir $(SFI_dir)/build
	cd $(SFI_dir)/build; \
	cp $(SFD_dir)/include/face_detection.h $(SFI_dir)/include;\
	cp $(SFA_dir)/include/face_alignment.h $(SFI_dir)/include;\
	cp $(SFD_dir)/build/libseeta_fa* $(SFI_dir)/build;\
	cp $(SFA_dir)/build/libseeta_fa* $(SFI_dir)/build;\
	cmake ..; \
	make -j2; \
	mkdir -p $(ROOT_PATH)/bin; \
	cp -rf $(SFI_dir)/model $(ROOT_PATH)/bin; \
	cp $(SFI_dir)/build/src/test/test_face_* $(ROOT_PATH)/bin; \
	cp -rf $(SFI_dir)/data $(ROOT_PATH)/bin;




