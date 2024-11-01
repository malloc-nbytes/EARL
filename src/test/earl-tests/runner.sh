#!/bin/bash

set -xe

earl testmgr.rl -- gen true true
earl < cmds.txt
