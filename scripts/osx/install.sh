#!/usr/bin/env bash

set -e
set -o pipefail

mapbox_time "checkout_mason" \
git submodule update --init .mason

mapbox_time "install_xcpretty" \
gem install xcpretty --no-rdoc --no-ri --no-document --quiet

if [ ! -z "${AWS_ACCESS_KEY_ID}" ] && [ ! -z "${AWS_SECRET_ACCESS_KEY}" ] ; then
    # Install and add awscli to PATH for copying Qt 4.7 prebuilt binaries
    mapbox_time "install_awscli" \
    brew install awscli

    S3_PREFIX=s3://mason-binaries/prebuilt
    QT_TARBALL=qt-4.7-osx-yosemite.tgz

    mapbox_time "download_prebuilt_qt47" \
    aws s3 cp ${S3_PREFIX}/${QT_TARBALL} .

    mapbox_time "ensure_opt_directory" \
    sudo mkdir -p /opt

    mapbox_time "unpack_prebuilt_qt47" \
    sudo tar xzf ${QT_TARBALL} -C /opt
fi
