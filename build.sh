#!/bin/bash
# Copyright (c) Microsoft Corporation. All rights reserved.
# Licensed under the MIT License.

# Get directory this script is in
DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

#requires python2.7 or higher
python $DIR/tools/ci_build/build.py --use_openmp --build_dir $DIR/build/Linux "$@"
