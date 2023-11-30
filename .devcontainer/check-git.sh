#!/bin/bash

if ! command -v git &> /dev/null; then
    echo "Git is not installed!"
    exit 1
fi

need_config=0
if [ -z "$(git config --get user.name)" ]; then
    ((need_config += 1))
fi
if [ -z "$(git config --get user.email)" ]; then
    ((need_config += 2))
fi
if [ -z "$(git config --get core.editor)" ]; then
    ((need_config += 4))
fi

if [ $need_config -gt 0 ]; then
    echo Git needs configuration!
    echo
    echo It is suggested to add local configurations, since global configurations
    echo will get overwritten by container rebuilds.
    echo
fi
if [ $((need_config & 1)) -gt 0 ]; then
    echo "  git config --local user.name \"Your Name\""
fi
if [ $((need_config & 2)) -gt 0 ]; then
    echo "  git config --local user.email \"you@domain.com\""
fi
if [ $((need_config & 4)) -gt 0 ]; then
    echo "  git config --local core.editor \"code --wait\""
fi
