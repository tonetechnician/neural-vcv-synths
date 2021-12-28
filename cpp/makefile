#!/bin/bash
init:
	./scripts/init-env.sh

cmake-init:
	echo "Initializing cmake"
	RACK_DIR="${shell pwd}/Rack-SDK"}
	${shell export RACK_DIR=$(RACK_DIR)}
	mkdir -p ./Nash-Suite/cmake-build
	cd ./Nash-Suite/cmake-build && cmake .. -DLIBTORCH_DIR=${shell cd "./Nash-Suite/dep/libtorch"; pwd}\

cmake-build:
		cd ./Nash-Suite/cmake-build && cmake --build .