#!/bin/bash

set -xe

earl testmgr.earl -- gen true false
earl < cmds.txt
