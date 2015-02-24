#!/usr/bin/env bash

if [ "$TRAVIS_BRANCH" == 'master' ]; then
    wput out ftp://$FTP_USER:$FTP_PASS@$FTP_HOST/ --less-verbose --reupload --dont-continue --basename=out/ | sed "s/=>.*:21//"
fi
