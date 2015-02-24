#!/usr/bin/env bash

PATH=`npm bin`:$PATH
docpad generate --env static
