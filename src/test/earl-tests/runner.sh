#!/bin/bash

set -xe

earl testmgr.earl --verbose -- gen true true
earl --verbose < cmds.txt
