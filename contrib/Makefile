BUILD_DIR := $(dir $(abspath $(firstword $(MAKEFILE_LIST))))
DOCKER := $(shell test -x /usr/bin/podman && echo podman || echo docker)

build-ubuntu: 22.04

18.04 20.04 22.04 24.04:
	$(DOCKER) build -f $(BUILD_DIR)/build-ubuntu.Dockerfile --build-arg RELEASE=$@ -t build-awscred-lib .
	$(DOCKER) run -ti --rm -v ./:/build build-awscred-lib
