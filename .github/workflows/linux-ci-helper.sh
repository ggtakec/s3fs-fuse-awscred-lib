#!/bin/sh
#
# s3fs-fuse-awscred-lib ( s3fs-fuse credential I/F library for AWS )
#
#     Copyright 2022 Takeshi Nakatani <ggtakec@gmail.com>
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#

#-----------------------------------------------------------
# Common variables
#-----------------------------------------------------------
PRGNAME=$(basename "$0")

echo "${PRGNAME} [INFO] Start Linux helper for installing packages."

#-----------------------------------------------------------
# Parameter check
#-----------------------------------------------------------
#
# Usage: ${PRGNAME} "OS:VERSION"
#
if [ $# -ne 1 ]; then
	echo "${PRGNAME} [ERROR] No container name options specified."
fi
CONTAINER_FULLNAME="$1"

#-----------------------------------------------------------
# OS dependent variables
#-----------------------------------------------------------
if [ "${CONTAINER_FULLNAME}" = "ubuntu:24.04" ]; then
	PACKAGE_MANAGER_BIN="apt-get"
	PACKAGE_UPDATE_OPTIONS="update -y -qq"
	PACKAGE_INSTALL_OPTIONS="install -y"

	INSTALL_PACKAGES="curl git g++ cmake libcurl4-openssl-dev libssl-dev uuid-dev zlib1g-dev libpulse-dev"
	INSTALL_REPO_OPTIONS=""

elif [ "${CONTAINER_FULLNAME}" = "ubuntu:22.04" ]; then
	PACKAGE_MANAGER_BIN="apt-get"
	PACKAGE_UPDATE_OPTIONS="update -y -qq"
	PACKAGE_INSTALL_OPTIONS="install -y"

	INSTALL_PACKAGES="curl git g++ cmake libcurl4-openssl-dev libssl-dev uuid-dev zlib1g-dev libpulse-dev"
	INSTALL_REPO_OPTIONS=""

elif [ "${CONTAINER_FULLNAME}" = "ubuntu:20.04" ]; then
	PACKAGE_MANAGER_BIN="apt-get"
	PACKAGE_UPDATE_OPTIONS="update -y -qq"
	PACKAGE_INSTALL_OPTIONS="install -y"

	INSTALL_PACKAGES="curl git g++ cmake libcurl4-openssl-dev libssl-dev uuid-dev zlib1g-dev libpulse-dev"
	INSTALL_REPO_OPTIONS=""

elif [ "${CONTAINER_FULLNAME}" = "debian:bookworm" ]; then
	PACKAGE_MANAGER_BIN="apt-get"
	PACKAGE_UPDATE_OPTIONS="update -y -qq"
	PACKAGE_INSTALL_OPTIONS="install -y"

	INSTALL_PACKAGES="curl git g++ cmake libcurl4-openssl-dev libssl-dev uuid-dev zlib1g-dev libpulse-dev"
	INSTALL_REPO_OPTIONS=""

elif [ "${CONTAINER_FULLNAME}" = "debian:bullseye" ]; then
	PACKAGE_MANAGER_BIN="apt-get"
	PACKAGE_UPDATE_OPTIONS="update -y -qq"
	PACKAGE_INSTALL_OPTIONS="install -y"

	INSTALL_PACKAGES="curl git g++ cmake libcurl4-openssl-dev libssl-dev uuid-dev zlib1g-dev libpulse-dev"
	INSTALL_REPO_OPTIONS=""

elif [ "${CONTAINER_FULLNAME}" = "rockylinux:9" ]; then
	PACKAGE_MANAGER_BIN="dnf"
	PACKAGE_UPDATE_OPTIONS="update -y -qq"
	PACKAGE_INSTALL_OPTIONS="install -y --allowerasing"

	INSTALL_PACKAGES="curl git gcc-c++ cmake libcurl-devel openssl-devel uuid-devel zlib-devel pulseaudio-libs-devel diffutils"
	INSTALL_REPO_OPTIONS="--enablerepo=crb"

elif [ "${CONTAINER_FULLNAME}" = "rockylinux:8" ]; then
	PACKAGE_MANAGER_BIN="dnf"
	PACKAGE_UPDATE_OPTIONS="update -y -qq"
	PACKAGE_INSTALL_OPTIONS="install -y"

	INSTALL_PACKAGES="curl git gcc-c++ cmake libcurl-devel openssl-devel uuid-devel zlib-devel pulseaudio-libs-devel diffutils"
	INSTALL_REPO_OPTIONS="--enablerepo=powertools"

elif [ "${CONTAINER_FULLNAME}" = "fedora:41" ]; then
	PACKAGE_MANAGER_BIN="dnf"
	PACKAGE_UPDATE_OPTIONS="update -y -qq"
	PACKAGE_INSTALL_OPTIONS="install -y"

	INSTALL_PACKAGES="curl git gcc-c++ cmake libcurl-devel openssl-devel openssl-devel-engine uuid-devel zlib-devel pulseaudio-libs-devel diffutils"
	INSTALL_REPO_OPTIONS=""

elif [ "${CONTAINER_FULLNAME}" = "fedora:40" ]; then
	PACKAGE_MANAGER_BIN="dnf"
	PACKAGE_UPDATE_OPTIONS="update -y -qq"
	PACKAGE_INSTALL_OPTIONS="install -y"

	INSTALL_PACKAGES="curl git gcc-c++ cmake libcurl-devel openssl-devel uuid-devel zlib-devel pulseaudio-libs-devel diffutils"
	INSTALL_REPO_OPTIONS=""

elif [ "${CONTAINER_FULLNAME}" = "opensuse/leap:15" ]; then
	PACKAGE_MANAGER_BIN="zypper"
	PACKAGE_UPDATE_OPTIONS="refresh"
	PACKAGE_INSTALL_OPTIONS="install -y"

	INSTALL_PACKAGES="curl git gcc-c++ cmake libcurl-devel openssl-devel uuid-devel zlib-devel libpulse-devel"
	INSTALL_REPO_OPTIONS=""

elif [ "${CONTAINER_FULLNAME}" = "alpine:3.21" ]; then
	PACKAGE_MANAGER_BIN="apk"
	PACKAGE_UPDATE_OPTIONS="update --no-progress"
	PACKAGE_INSTALL_OPTIONS="add --no-progress --no-cache"

	INSTALL_PACKAGES="curl git g++ cmake make curl-dev openssl-dev util-linux-dev zlib-dev pulseaudio-dev diffutils"
	INSTALL_REPO_OPTIONS=""

else
	echo "No container configured for: ${CONTAINER_FULLNAME}"
	exit 1
fi

#-----------------------------------------------------------
# Install
#-----------------------------------------------------------
#
# Update packages (ex. apt-get update -y -qq)
#
echo "${PRGNAME} [INFO] Updates."
if ! /bin/sh -c "${PACKAGE_MANAGER_BIN} ${PACKAGE_UPDATE_OPTIONS}"; then
	exit 1
fi

#
# Install packages
#
echo "${PRGNAME} [INFO] Install packages."
if ! /bin/sh -c "${PACKAGE_MANAGER_BIN} ${INSTALL_REPO_OPTIONS} ${PACKAGE_INSTALL_OPTIONS} ${INSTALL_PACKAGES}"; then
	exit 1
fi

#-----------------------------------------------------------
# Finish
#-----------------------------------------------------------
echo "${PRGNAME} [INFO] Finish Linux helper for installing packages."

exit 0

#
# Local variables:
# tab-width: 4
# c-basic-offset: 4
# End:
# vim600: noexpandtab sw=4 ts=4 fdm=marker
# vim<600: noexpandtab sw=4 ts=4
#
