#!/bin/bash
init:
	./scripts/init-env.sh

cmake-init:
	echo "Initializing cmake"
	RACK_DIR="${shell pwd}/Rack-SDK"}
	${shell export RACK_DIR=$(RACK_DIR)}
	rm -r ./Nash-Suite/cmake-build
	rm -r ./Nash-Suite/cmake-dist
	mkdir -p ./Nash-Suite/cmake-build
	cd ./Nash-Suite/cmake-build && cmake .. -DLIBTORCH_DIR=${shell cd "./Nash-Suite/dep/libtorch"; pwd}\

cmake-build:
		cd ./Nash-Suite/cmake-build && cmake --clean-first . && cmake --build .