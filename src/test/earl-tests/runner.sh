#!/bin/bash

set -xe

earl testmgr.earl -- gen true true
earl --verbose < cmds.txt
