ARG RELEASE=20.04
FROM ubuntu:$RELEASE
ENV DEBIAN_FRONTEND=noninteractive
WORKDIR /src
RUN apt-get update && apt-get install -qy devscripts \
	libcurl4-openssl-dev libssl-dev uuid-dev zlib1g-dev libpulse-dev \
	git cmake
RUN git clone --recurse-submodules https://github.com/aws/aws-sdk-cpp
RUN mkdir sdk_build && cd sdk_build && \
	cmake ../aws-sdk-cpp -DCMAKE_BUILD_TYPE=Release -DCMAKE_PREFIX_PATH=${PWD} \
	-DBUILD_ONLY="core;identity-management" -DAUTORUN_UNIT_TESTS=OFF && make && make install
ADD ./ /src/s3fs-fuse-awscred-lib
RUN cd s3fs-fuse-awscred-lib && \
	cmake -S . -B build && \
	cmake --build build
VOLUME /build
CMD mkdir -p /build && cp /src/s3fs-fuse-awscred-lib/build/libs3fsawscred.so \
	/src/s3fs-fuse-awscred-lib/build/s3fsawscred_test \
	/usr/local/lib/libaws-cpp-sdk-identity-management.so \
	/usr/local/lib/libaws-cpp-sdk-core.so \
	/usr/local/lib/libaws-cpp-sdk-cognito-identity.so \
	/usr/local/lib/libaws-cpp-sdk-sts.so \
	/build
