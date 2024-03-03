ARG RELEASE=20.04
FROM ubuntu:$RELEASE
ARG RELEASE=20.04
ENV DEBIAN_FRONTEND=noninteractive
ADD ./ /src/s3fs-fuse-awscred-lib
WORKDIR /src/s3fs-fuse-awscred-lib
RUN .github/workflows/linux-ci-helper.sh ubuntu:${RELEASE}
RUN mkdir -p /src/aws-sdk/sdk_build
WORKDIR /src/aws-sdk
RUN git clone --recurse-submodules https://github.com/aws/aws-sdk-cpp
WORKDIR /src/aws-sdk/sdk_build
RUN cmake ../aws-sdk-cpp -DCMAKE_BUILD_TYPE=Release -DCMAKE_PREFIX_PATH=${PWD} -DBUILD_ONLY="core;identity-management" -DAUTORUN_UNIT_TESTS=OFF
RUN make
RUN make install
WORKDIR /src/s3fs-fuse-awscred-lib
RUN cmake -S . -B build
RUN cmake --build build
VOLUME /build
CMD mkdir -p /build && cp /src/s3fs-fuse-awscred-lib/build/libs3fsawscred.so \
	/src/s3fs-fuse-awscred-lib/build/s3fsawscred_test \
	/usr/local/lib/libaws-cpp-sdk-identity-management.so \
	/usr/local/lib/libaws-cpp-sdk-core.so \
	/usr/local/lib/libaws-cpp-sdk-cognito-identity.so \
	/usr/local/lib/libaws-cpp-sdk-sts.so \
	/build
